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
#ifndef SWIRLYD_FIXAPP_HXX
#define SWIRLYD_FIXAPP_HXX

#include "FixHandler.hxx"

#include <swirly/fix/App.hpp>

namespace swirly {
inline namespace sys {
class Reactor;
} // namespace sys

class FixApp : public FixAppBase {
  public:
    explicit FixApp(Reactor& r)
    : reactor_(r)
    {
    }
    ~FixApp() override;

    // Copy.
    FixApp(const FixApp&) = delete;
    FixApp& operator=(const FixApp&) = delete;

    // Move.
    FixApp(FixApp&&) = delete;
    FixApp& operator=(FixApp&&) = delete;

  protected:
    // FixHandler.

    void do_on_logon(CyclTime now, FixConn& conn, const FixSessId& sess_id) override;
    void do_on_logout(CyclTime now, FixConn& conn, const FixSessId& sess_id,
                      bool disconnect) noexcept override;
    void do_on_message(CyclTime now, FixConn& conn, std::string_view msg, std::size_t body_off,
                       Version ver, const FixHeader& hdr) override;
    void do_on_error(CyclTime now, const FixConn& conn, const std::exception& e) noexcept override;
    void do_on_timeout(CyclTime now, const FixConn& conn) noexcept override;

    // FixApp.

    void do_config(CyclTime now, const FixSessId& sess_id, const Config& config) override;
    void do_prepare(CyclTime now) override;
    void do_on_connect(CyclTime now, FixConn& conn) override;
    void do_on_disconnect(CyclTime now, const FixConn& conn) noexcept override;

  private:
    Reactor& reactor_;
    FixHandlerMap handler_map_;
};

} // namespace swirly

#endif // SWIRLYD_FIXAPP_HXX
