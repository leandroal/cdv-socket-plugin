/*
 * Copyright (c) 2013 BlackBerry Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <fcntl.h>
#include <list>
#include <memory>
#include <string>
#include <sys/slog2.h>
#include <sys/siginfo.h>
#include "quantogastei_ndk.hpp"
#include "quantogastei_js.hpp"
#include "QuantoGasteiSleeper.hpp"

#include <QThread>

TCPSocketLib_NDK::TCPSocketLib_NDK(TCPSocketLib_JS *parent, QObject *qtParent):
    QObject(qtParent),
	m_pParent(parent) {

    m_pParent->getLog()->info("TCPSocketLib_NDK Created");
    m_clientSocket = new QTcpSocket(this);
    m_serverAddress = "";
    m_serverPort = 0;
    m_callbackId = "";

    bool ok;
    // establish signal/slot connection with socket for being informed of new established connection
    ok = connect(m_clientSocket, SIGNAL(connected()), this, SLOT(connectedNDK()));
    // TODO: handle this better
    Q_ASSERT (ok);
}

TCPSocketLib_NDK::~TCPSocketLib_NDK() {
    if (m_clientSocket) {
        qDebug() << "TCPSocketLib: closing socket";
        disconnect(m_clientSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
        m_clientSocket->close();
        m_clientSocket->deleteLater();
    }
}

// Take in input and return a value
QString TCPSocketLib_NDK::ping(const QVariantMap& args) {
    return m_pParent->getJson()->resultInJson(true);
}

void TCPSocketLib_NDK::connectedNDK() {
    m_pParent->getLog()->info("TCPSocketLib_NDK::connectedNDK: writing to socket.");
}

void TCPSocketLib_NDK::waitForConnecting(int waitMiliseconds, int maxLoopCount) {
    int counter = 0;
    while (!isConnectedNDK() && counter < waitMiliseconds) {
        QuantoGasteiSleeper::msleep(waitMiliseconds);
        counter++;
    }
}

QString TCPSocketLib_NDK::connectNDK(const QVariantMap& args) {
    if (args.isEmpty()) {
        m_pParent->getLog()->error("TCPSocketLib_NDK::connectNDK: no arguments provided!");
        return m_pParent->getJson()->errorInJson(UNKNOWN_ERROR, "No arguments provided!");
    }

    if (!m_clientSocket->isOpen()) {
        m_pParent->getLog()->info("TCPSocketLib_NDK::connect: connecting to server socket");
        m_serverAddress = args["host"].toString();
        m_serverPort = args["port"].toUInt();
        m_clientSocket->connectToHost(m_serverAddress, m_serverPort);
        bool ok = connect(m_clientSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnected()));
        // TODO: handle this better
        Q_ASSERT(ok);
        ok = connect(m_clientSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        // TODO: handle this better
        Q_ASSERT(ok);
        Q_UNUSED(ok);
        waitForConnecting(500, 10);
        QVariantMap result;
        result["result"] = isConnectedNDK();
        result["connectionId"] = m_serverAddress.append(":").append(args["port"].toString());
        return m_pParent->getJson()->resultInJson(result);
    }
    QVariantMap result;
    result["connectionId"] = m_serverAddress.append(":").append(args["port"].toString());
    return m_pParent->getJson()->resultInJson(result);
}

QString TCPSocketLib_NDK::disconnectNDK(const QVariantMap& args) {
    if (args.isEmpty()) {
        m_pParent->getLog()->error("TCPSocketLib_NDK::disconnectNDK: no arguments provided!");
        return m_pParent->getJson()->errorInJson(UNKNOWN_ERROR, "No arguments provided!");
    }
    disconnect(m_clientSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnected()));
    disconnect(m_clientSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    m_clientSocket->close();
    return m_pParent->getJson()->resultInJson(true);
}

QString TCPSocketLib_NDK::isConnected(const QVariantMap& args) {
    return m_pParent->getJson()->resultInJson(isConnectedNDK());
}

bool TCPSocketLib_NDK::isConnectedNDK() {
    return (m_clientSocket && m_clientSocket->state() == QTcpSocket::ConnectedState);
}

QString TCPSocketLib_NDK::send(const QVariantMap& args) {
    if (args.isEmpty()) {
        m_pParent->getLog()->error("TCPSocketLib_NDK::send: no arguments provided!");
        return m_pParent->getJson()->errorInJson(UNKNOWN_ERROR, "No arguments provided!");
    }

    if (isConnectedNDK()) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << args["message"].toString();
        out.device()->seek(0);
        m_clientSocket->write(block);
        m_clientSocket->flush();
        m_pParent->getLog()->info("TCPSocketLib_NDK::send: message is: %1", args["message"].toString().toStdString().c_str());
        return m_pParent->getJson()->resultInJson(true);
    }
    return m_pParent->getJson()->resultInJson(false);
}

QString TCPSocketLib_NDK::disconnectAllNDK() {
    return "";
}

void TCPSocketLib_NDK::receive(QString callbackId, const QVariantMap& args) {
    m_pParent->getLog()->info("TCPSocketLib_NDK::receive: registering callback");
    m_callbackId = callbackId;
}

void TCPSocketLib_NDK::handleDisconnected() {
    m_pParent->getLog()->info("TCPSocketLib_NDK::disconnectedNDK: disconnected...");
}

void TCPSocketLib_NDK::readyRead() {
    QString message = QString::fromUtf8(m_clientSocket->readAll());
    m_pParent->getLog()->info("CommunicationChannel::handleMessageReceived: new call arrived. Command: %1", message.toStdString().c_str());
    QVariantMap result;
    result["id"] = "";
    result["host"] = m_serverAddress;
    result["port"] = m_serverPort;
    result["data"] = message;
    m_pParent->NotifyEvent((m_callbackId + " " + m_pParent->getJson()->resultInJson(result)).toStdString());
}
