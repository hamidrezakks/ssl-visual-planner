//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    robocup_ssl_server.cpp
  \brief   C++ Implementation: robocup_ssl_server
  \author  Stefan Zickler, 2009
  \author  Jan Segre, 2012
*/
//========================================================================
#include "server.h"
#include <QtNetwork>
#include <iostream>


using namespace std;

Server::Server(QObject *parent, const quint16 &port, const string &net_address, const string &net_interface) :
    _socket(new QUdpSocket(parent)),
    _port(port),
    _net_address(new QHostAddress(QString(net_address.c_str()))),
    _net_interface(new QNetworkInterface(QNetworkInterface::interfaceFromName(QString(net_interface.c_str()))))
{
    _socket->setSocketOption(QAbstractSocket::MulticastTtlOption, 1);
}

Server::~Server()
{
    mutex.lock();
    mutex.unlock();
    delete _socket;
    delete _net_address;
    delete _net_interface;
}

void Server::change_port(const quint16 & port)
{
    _port = port;
}

void Server::change_address(const string & net_address)
{
    delete _net_address;
    _net_address = new QHostAddress(QString(net_address.c_str()));
}

void Server::change_interface(const string & net_interface)
{
    delete _net_interface;
    _net_interface = new QNetworkInterface(QNetworkInterface::interfaceFromName(QString(net_interface.c_str())));
}


bool Server::send(const PlanBook* packet)
{
    QByteArray datagram;
    if(packet != NULL)
    {
        datagram.resize(packet->ByteSize());
        bool success = packet->SerializeToArray(datagram.data(), datagram.size());
        //success = false;
        if(!success) {
            //TODO: print useful info
            qDebug() << "Serializing packet to array failed.";
            return false;
        }

        mutex.lock();
        quint64 bytes_sent = _socket->writeDatagram(datagram, QHostAddress::LocalHost, _port);
        mutex.unlock();
        if (bytes_sent != datagram.size()) {
            qDebug() << QString("Sending UDP datagram failed (maybe too large?). Size was: %1 byte(s).").arg(datagram.size());
            return false;
        }
    }
    return true;
}

