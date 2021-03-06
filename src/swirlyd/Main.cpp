/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include "FixApp.hxx"
#include "RestApp.hxx"
#include "RestImpl.hxx"
#include "RestRequest.hxx"

#include <swirly/db/DbCtx.hpp>

#include <swirly/lob/App.hpp>

#include <swirly/fin/Journ.hpp>
#include <swirly/fin/Model.hpp>
#include <swirly/fin/MsgQueue.hpp>

#include <swirly/fix/Ctx.hpp>

#include <swirly/http/Serv.hpp>

#include <swirly/app/MemCtx.hpp>
#include <swirly/app/Thread.hpp>

#include <swirly/sys/Daemon.hpp>
#include <swirly/sys/EpollReactor.hpp>
#include <swirly/sys/File.hpp>
#include <swirly/sys/PidFile.hpp>
#include <swirly/sys/Signal.hpp>
#include <swirly/sys/System.hpp>

#include <swirly/util/Config.hpp>
#include <swirly/util/Exception.hpp>
#include <swirly/util/Finally.hpp>
#include <swirly/util/Log.hpp>
#include <swirly/util/Options.hpp>

#include <experimental/filesystem>

#include <fstream>
#include <iomanip>
#include <iostream>

#include <fcntl.h> // open()
#include <syslog.h>
#include <unistd.h> // dup2()

namespace fs = std::experimental::filesystem;

namespace swirly {
inline namespace util {
template <>
struct TypeTraits<fs::path> {
    static auto from_string(std::string_view sv) noexcept { return fs::path{sv}; }
    static auto from_string(const std::string& s) noexcept { return fs::path{s}; }
};
} // namespace util
} // namespace swirly

using namespace std;
using namespace swirly;

namespace {

void open_log_file(const char* path)
{
    const auto file = os::open(path, O_RDWR | O_CREAT | O_APPEND, 0644);
    dup2(*file, STDOUT_FILENO);
    dup2(*file, STDERR_FILENO);
}

struct Opts {
    fs::path config;
    bool daemon{false};
    WallTime start_time{};
};

void get_opts(int argc, char* argv[], Opts& opts)
{
    opterr = 0;
    int ch;

    Options options{"Swirly [Options]"};

    // clang-format off
    options('d',"daemon", Switch{opts.daemon}, "Daemonize")
           ('f',"file", Value{opts.config}, "Configuration File")
           ('s',"start", Value{opts.start_time}, "Start time (ms)")
           ('h',"help", Help{}, "Display help")
           ;
    // clang-format on

    options.parse(argc, argv);
}

MemCtx mem_ctx;

// Optional FIX extension.
struct FixExt {
    FixExt(CyclTime now, Reactor& r, const char* config, LobApp& lob_app)
    : app{r, lob_app}
    , ctx{now, r, config, app}
    {
    }
    FixApp app;
    FixCtx ctx;
};

} // namespace

namespace swirly {
inline namespace app {

void* alloc(size_t size)
{
    return mem_ctx.alloc(size);
}

void* alloc(size_t size, align_val_t al)
{
    return mem_ctx.alloc(size, al);
}

void dealloc(void* ptr, size_t size) noexcept
{
    return mem_ctx.dealloc(ptr, size);
}

void dealloc(void* ptr, size_t size, align_val_t al) noexcept
{
    return mem_ctx.dealloc(ptr, size, al);
}

using RestConn = BasicHttpConn<RestRequest, RestApp>;
using RestServ = BasicHttpServ<RestConn, RestApp>;

} // namespace app
} // namespace swirly

int main(int argc, char* argv[])
{
    close_all();

    int ret = 1;
    try {

        Opts opts;
        get_opts(argc, argv, opts);

        const auto start_time
            = is_zero(opts.start_time) ? CyclTime::now() : CyclTime::now(opts.start_time);

        Config config;
        if (!opts.config.empty()) {
            ifstream is{opts.config};
            if (!is.is_open()) {
                throw Exception{make_error_code(errc::io_error),
                                err_msg() << "open failed: " << opts.config};
            }
            config.read_section(is);
        }

        mem_ctx = MemCtx{config.get<size_t>("mem_size", 1) << 20};

        const auto log_level = config.get("log_level", ""sv);
        if (!log_level.empty()) {
            set_log_level(from_string<int>(log_level));
        }

        // Restrict file creation mask if specified. The umask function is always successful.
        const auto file_mode = config.get("file_mode", ""sv);
        if (!file_mode.empty()) {
            // Zero base to auto-detect: if the prefix is 0, the base is octal, if the prefix is 0x or 0X.
            umask(from_string<mode_t>(file_mode));
        } else if (opts.daemon) {
            umask(0027);
        }

        fs::path run_dir{config.get("run_dir", "")};
        if (!run_dir.empty()) {
            // Change the current working directory if specified.
            run_dir = fs::canonical(run_dir, fs::current_path());
            os::chdir(run_dir.c_str());
        } else if (opts.daemon) {
            // Default to root directory if daemon.
            run_dir = fs::current_path().root_path();
        } else {
            // Otherwise, default to current directory.
            run_dir = fs::current_path();
        }

        fs::path log_file{config.get("log_file", "")};
        fs::path pid_file{config.get("pid_file", "")};

        PidFile pf;
        if (!pid_file.empty()) {

            // Pid file is relative to working directory. We use absolute, rather than canonical
            // here, because canonical requires that the file exists.
            if (pid_file.is_relative()) {
                pid_file = fs::absolute(pid_file, run_dir);
            }
            fs::create_directory(pid_file.parent_path());
            pf = open_pid_file(pid_file.c_str(), 0644);
        }

        if (opts.daemon) {
            // Daemonise process.
            daemon();
        }

        write_pid_file(pf);
        if (log_file.empty()) {

            // If a log file is not specified, then use syslog().

#if SWIRLY_BUILD_DEBUG
            setlogmask(LOG_UPTO(LOG_DEBUG));
#endif
            set_logger(sys_logger);

        } else if (log_file == "-") {

            // A single dash in place of a file name is used to indicate stdout/stderr.
            // This is effectively a no-op and there is no need to open a file.

            // Clear so that SIGHUP does not attempt to reopen the log file.
            log_file.clear();

        } else {

            // Log file is relative to working directory. We use absolute, rather than canonical
            // here, because canonical requires that the file exists.
            if (log_file.is_relative()) {
                log_file = fs::absolute(log_file, run_dir);
            }
            fs::create_directory(log_file.parent_path());

            SWIRLY_NOTICE << "opening log file: " << log_file;
            open_log_file(log_file.c_str());
        }

        const fs::path fix_config{config.get("fix_config", "")};
        const fs::path mq_file{config.get("mq_file", "")};
        const char* const http_port{config.get("http_port", "8080")};
        const auto max_execs = config.get<size_t>("max_execs", 1 << 4);

        SWIRLY_NOTICE << "initialising daemon";
        SWIRLY_INFO << "config:      " << opts.config;
        SWIRLY_INFO << "daemon:      " << (opts.daemon ? "yes" : "no");
        SWIRLY_INFO << "start_time:  " << start_time.wall_time();

        SWIRLY_INFO << "file_mode:   " << setfill('0') << setw(3) << oct << swirly::file_mode();
        SWIRLY_INFO << "http_port:   " << http_port;
        SWIRLY_INFO << "log_file:    " << log_file;
        SWIRLY_INFO << "log_level:   " << get_log_level();
        SWIRLY_INFO << "fix_config:  " << fix_config;
        SWIRLY_INFO << "max_execs:   " << max_execs;
        SWIRLY_INFO << "mem_size:    " << (mem_ctx.max_size() >> 20) << "MiB";
        SWIRLY_INFO << "mq_file:     " << mq_file;
        SWIRLY_INFO << "pid_file:    " << pid_file;
        SWIRLY_INFO << "run_dir:     " << run_dir;

        DbCtx db_ctx{config};
        MsgQueue mq;
        if (!mq_file.empty()) {
            mq = MsgQueue{mq_file.c_str()};
        } else {
            mq = MsgQueue{1 << 10};
        }
        LobApp lob_app{mq, max_execs};
        {
            Model& model = db_ctx.model();
            lob_app.load(start_time, model);
        }

        EpollReactor reactor{1024};
        const TcpEndpoint ep{Tcp::v4(), from_string<uint16_t>(http_port)};

        RestApp rest_app{lob_app};
        RestServ rest_serv{start_time, reactor, ep, rest_app};

        unique_ptr<FixExt> fix_ext;
        if (!fix_config.empty()) {
            fix_ext.reset(new FixExt{start_time, reactor, fix_config.c_str(), lob_app});
        }

        ReactorThread reactor_thread{reactor, ThreadConfig{"reactor"s}};
        Journ& journ = db_ctx.journ();
        auto journ_agent = [&mq, &journ]() {
            int n{0};
            Msg msg;
            while (mq.pop(msg)) {
                journ.write(msg);
                ++n;
            }
            return n;
        };
        AgentThread journ_thread{journ_agent, PhasedBackoff{}, ThreadConfig{"journ"s}};

        SWIRLY_NOTICE << "started http server on port " << http_port;

        // Wait for termination.
        SigWait sig_wait;
        for (;;) {
            switch (const auto sig = sig_wait()) {
            case SIGHUP:
                SWIRLY_INFO << "received SIGHUP";
                if (!log_file.empty()) {
                    SWIRLY_NOTICE << "reopening log file: " << log_file;
                    open_log_file(log_file.c_str());
                }
                continue;
            case SIGINT:
                SWIRLY_INFO << "received SIGINT";
                break;
            case SIGTERM:
                SWIRLY_INFO << "received SIGTERM";
                break;
            default:
                SWIRLY_INFO << "received signal: " << sig;
                continue;
            }
            break;
        }
        ret = 0;
    } catch (const exception& e) {
        SWIRLY_ERROR << "exception: " << e.what();
        // Ensure that startup failures are always reported to interactive users.
        if (get_logger() != std_logger) {
            std::cerr << "exception: " << e.what() << std::endl;
        }
    }
    return ret;
}
