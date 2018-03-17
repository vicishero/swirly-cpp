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
#ifndef SWIRLY_SYS_TCPACCEPTOR_HPP
#define SWIRLY_SYS_TCPACCEPTOR_HPP

#include <swirly/sys/Reactor.hpp>
#include <swirly/sys/TcpSocket.hpp>

namespace swirly {
inline namespace sys {

class SWIRLY_API TcpAcceptor : public EventHandler {
    using IntrusivePtr = boost::intrusive_ptr<TcpAcceptor>;

  public:
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    TcpAcceptor(Reactor& r, const Endpoint& ep);
    ~TcpAcceptor() noexcept override;

    // Copy.
    TcpAcceptor(const TcpAcceptor&) = delete;
    TcpAcceptor& operator=(const TcpAcceptor&) = delete;

    // Move.
    TcpAcceptor(TcpAcceptor&&) = delete;
    TcpAcceptor& operator=(TcpAcceptor&&) = delete;

  protected:
    void doReady(int fd, unsigned events, Time now) override;
    virtual void doAccept(IoSocket&& sock, const Endpoint& ep, Time now) = 0;

  private:
    TcpSocketServ serv_;
    SubHandle sub_;
};

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_TCPACCEPTOR_HPP
