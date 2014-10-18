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

#ifndef TCPSocketLib_NDK_HPP_
#define TCPSocketLib_NDK_HPP_

#include <string>
#include <pthread.h>

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QtNetwork/QTcpSocket>

class TCPSocketLib_JS;

class TCPSocketLib_NDK: public QObject {
    Q_OBJECT
public:
	explicit TCPSocketLib_NDK(TCPSocketLib_JS *parent = 0, QObject *qtParent = 0);
	virtual ~TCPSocketLib_NDK();

public Q_SLOTS:
    QString ping(const QVariantMap& args);
    QString connectNDK(const QVariantMap& args);
    QString disconnectNDK(const QVariantMap& args);
    QString disconnectAllNDK();
    QString send(const QVariantMap& args);
    QString isConnected(const QVariantMap& args);
    void receive(QString callbackId, const QVariantMap& args);

    void connectedNDK();
    void readyRead();
    void disconnectedNDK();
    bool isConnectedNDK();
    void handleDisconnected();

private:
	TCPSocketLib_JS *m_pParent;
	QTcpSocket *m_clientSocket;

	void waitForConnecting(int waitMiliseconds, int maxLoopCount);

    QString m_serverAddress;
    quint16 m_serverPort;

    QString m_callbackId;
};

#endif /* TCPSocketLib_NDK_HPP_ */
