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
#include <QString>
#include "quantogastei_js.hpp"
#include "quantogastei_ndk.hpp"

using namespace std;

/**
 * Default constructor.
 */
TCPSocketLib_JS::TCPSocketLib_JS(const std::string& id) :
		m_id(id) {
	m_pLogger = new Logger("TCPSocketLib_JS", this);
	m_pTCPSocketLibController = new TCPSocketLib_NDK(this);
	m_json = new QuantoGasteiJSON();
}

/**
 * TCPSocketLib_JS destructor.
 */
TCPSocketLib_JS::~TCPSocketLib_JS() {
	if (m_pTCPSocketLibController)
		delete m_pTCPSocketLibController;
	if (m_pLogger)
		delete m_pLogger;
	if (m_json)
	        delete m_json;
}

Logger* TCPSocketLib_JS::getLog() {
	return m_pLogger;
}

QuantoGasteiJSON* TCPSocketLib_JS::getJson() {
    return m_json;
}

/**
 * This method returns the list of objects implemented by this native
 * extension.
 */
char* onGetObjList() {
	static char name[] = "TCPSocketLib_JS";
	return name;
}

/**
 * This method is used by JNext to instantiate the TCPSocketLib_JS object when
 * an object is created on the JavaScript server side.
 */
JSExt* onCreateObject(const string& className, const string& id) {
	if (className == "TCPSocketLib_JS") {
		return new TCPSocketLib_JS(id);
	}

	return NULL;
}

/**
 * Method used by JNext to determine if the object can be deleted.
 */
bool TCPSocketLib_JS::CanDelete() {
	return true;
}

/**
 * It will be called from JNext JavaScript side with passed string.
 * This method implements the interface for the JavaScript to native binding
 * for invoking native code. This method is triggered when JNext.invoke is
 * called on the JavaScript side with this native objects id.
 */
string TCPSocketLib_JS::InvokeMethod(const string& cmd) {
    m_pLogger->critical("TCPSocketLib_JS::InvokeMethod: %s", cmd.c_str());
    // format must be: "command callbackId params"

    QString command = QString(cmd.c_str());
    qint64 commandIndex = command.indexOf(" ");
    QString strCommand = command.mid(0, commandIndex);
    qint64 callbackIndex = command.indexOf(" ", commandIndex + 1);
    QString callbackId;
    QString strArgs;
    if (callbackIndex == -1) {
        callbackId = command.mid(commandIndex + 1, command.length());
        strArgs = "";
    } else {
        callbackId = command.mid(commandIndex + 1, callbackIndex - commandIndex - 1);
        strArgs = command.mid(callbackIndex + 1, command.length());
    }

    QVariantMap args;
    if (strArgs.isEmpty()) {
        QVariant varArgs;
        QString errorMessage;
        bool parse = m_json->parse(strArgs, varArgs, errorMessage);
        if (!parse || varArgs.type() != QVariant::Map) {
            m_pLogger->error("TCPSocketLib_JS::InvokeMethod: invalid json data: %1", strArgs.toStdString().c_str());
            return (m_json->errorInJson(PARSING_ERROR, "Parameters not valid json format!")).toStdString();
        } else {
            args = varArgs.value<QVariantMap>();
        }
    }

    // based on the command given, run the appropriate method in quantogastei_ndk.cpp
    if (strCommand == "ping") {
        return (m_pTCPSocketLibController->ping(args)).toStdString();
    } else if (strCommand == "connect") {
        return (m_pTCPSocketLibController->connectNDK(args)).toStdString();
    } else if (strCommand == "disconnect") {
        return m_pTCPSocketLibController->disconnectNDK(args).toStdString();
    } else if (strCommand == "disconnectAll") {
        return m_pTCPSocketLibController->disconnectAllNDK().toStdString();
    } else if (strCommand == "send") {
        return m_pTCPSocketLibController->send(args).toStdString();
    } else if (strCommand == "isConnected") {
        return m_pTCPSocketLibController->isConnected(args).toStdString();
    } else if (strCommand == "receive") {
        m_pTCPSocketLibController->receive(callbackId, args);
    }

    return strCommand.append(";").append(command).toStdString();
}

// Notifies JavaScript of an event
void TCPSocketLib_JS::NotifyEvent(const std::string& event) {
	std::string eventString = m_id + " ";
	eventString.append(event);
	SendPluginEvent(eventString.c_str(), m_pContext);
}
