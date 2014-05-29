/**
 * (C) LGPL-3
 *
 * Tasteful Server <https://github.com/scheibel/tasteful-server>
 *
 * Copyright: 2012-2014 Lux, Scheibel
 * Authors:
 *     Roland Lux <rollux2000@googlemail.com>
 *     Willy Scheibel <willyscheibel@gmx.de>
 *
 * This file is part of Tasteful Server.
 *
 * Tasteful Server is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tasteful Server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Tasteful Server.  If not, see <http://www.gnu.org/licenses/>.
 **/

#pragma once

#include <tastefulserver/tastefulserver_api.h>

#include <tastefulserver/Protocol.h>
#include <tastefulserver/WebsocketFrame.h>
#include <tastefulserver/WebsocketFrameParser.h>
#include <tastefulserver/http.h>

namespace tastefulserver {

class WebsocketHandler;

class TASTEFULSERVER_API WebsocketProtocol : public Protocol
{
    Q_OBJECT
public:
    static const QString MagicString;
    static QString hashKey(const QString & key);
    static HttpResponse handshake(const HttpRequest & request);

public:
    WebsocketProtocol(WebsocketHandler * handler);

    void send(const WebsocketFrame & frame);

    void sendText(const QString & text);
    void sendBinary(const QByteArray & binary);
    void sendPing();
    void sendPong();
    void sendConnectionClose();
protected:
    WebsocketHandler * m_handler;
    WebsocketFrameParser m_parser;

    virtual void receiveData(const QByteArray & data) override;

protected slots:
    void badFrame();
};

} // namespace tastefulserver
