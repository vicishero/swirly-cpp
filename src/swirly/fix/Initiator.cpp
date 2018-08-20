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
#include "Initiator.hpp"

#include <swirly/util/Log.hpp>

namespace swirly {
inline namespace fix {
using namespace std;

FixInitiator::FixInitiator(Reactor& r, const Endpoint& ep, const FixConfig& config,
                           const FixSessId& sess_id, FixApp& app, Time now)
: reactor_(r)
, ep_{ep}
, config_(config)
, sess_id_{sess_id}
, app_(app)
{
    // Immediate and then at 2s intervals.
    tmr_ = reactor_.timer(now, 2s, Priority::Low, bind<&FixInitiator::on_timer>(this));
}

FixInitiator::~FixInitiator()
{
    sess_list_.clear_and_dispose([](auto* sess) { delete sess; });
}

void FixInitiator::do_connect(IoSocket&& sock, const Endpoint& ep, Time now)
{
    inprogress_ = false;

    // High performance TCP servers could use a custom allocator.
    auto* const sess = new FixSess{reactor_, move(sock), ep, config_, app_, now};
    sess_list_.push_back(*sess);
    sess->logon(sess_id_, now);
}

void FixInitiator::do_connect_error(const std::exception& e, Time now)
{
    SWIRLY_ERROR << "failed to connect: " << e.what();
    inprogress_ = false;
}

void FixInitiator::on_timer(Timer& tmr, Time now)
{
    if (sess_list_.empty() && !inprogress_) {
        SWIRLY_INFO << "reconnecting";
        if (!connect(reactor_, ep_, now)) {
            inprogress_ = true;
        }
    }
}

} // namespace fix
} // namespace swirly