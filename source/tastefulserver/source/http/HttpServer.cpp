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

#include <tastefulserver/HttpServer.h>
#include <tastefulserver/WebsocketProtocol.h>

namespace tastefulserver {

HttpServer::HttpServer(const RequestCallback & callback)
: m_callback(callback)
{
}

SocketFactory * HttpServer::createSocketFactory(qintptr socketDescriptor)
{
    return new TcpSocketFactory(socketDescriptor);
}

Connection * HttpServer::createConnection() const
{
    HttpProtocol * protocol = new HttpProtocol();

    connect(protocol, &HttpProtocol::requestsReady, this, &HttpServer::requestsReady, Qt::DirectConnection);

    return new Connection(protocol);
}

void HttpServer::requestsReady(HttpProtocol * protocol)
{
    while (protocol->hasRequest())
    {
        HttpRequest request = protocol->getNextRequest();

        if (request.isBad())
        {
            HttpResponse response(http::BadRequest);

            protocol->sendResponse(response);
        }
        else if (request.getHeader(http::Upgrade).getValue() == "websocket")
        {
            protocol->sendResponse(WebsocketProtocol::handshake(request));
            WebsocketProtocol * p = new WebsocketProtocol();
            connect(p, &WebsocketProtocol::framesReady, this, &HttpServer::framesReady, Qt::DirectConnection);
            protocol->connection()->setProtocol(p);
            break;
        }
        else
        {
            protocol->sendResponse(m_callback(request));
        }
    }
}

QString foo(const QByteArray & arr)
{
    QString s;
    for (char c : arr)
    {
        s += QString::number((int)(unsigned char)c)+" ";
    }

    return s;
}

void HttpServer::framesReady(WebsocketProtocol * protocol)
{
    while (protocol->hasFrame())
    {
        WebsocketFrame frame = protocol->getNextFrame();
        if (frame.isText())
        {
            //qDebug() << frame.getContent();
        }

        protocol->sendFrame(frame);

        WebsocketFrame f(WebsocketFrame::OpCode::Text);
        f.setRandomMask();
        f.setContent("Hello, this is server");

        //qDebug() << foo(f.toByteArray());

        protocol->sendFrame(f);
    }
}

} // namespace tastefulserver
