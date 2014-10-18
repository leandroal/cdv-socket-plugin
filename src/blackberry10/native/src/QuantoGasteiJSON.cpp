#include <bb/data/JsonDataAccess>

#include "QuantoGasteiJSON.hpp"

QuantoGasteiJSON::QuantoGasteiJSON(QObject *parent) : QObject(parent) {

}

QuantoGasteiJSON::~QuantoGasteiJSON() {

}

QString QuantoGasteiJSON::stringify(const QVariant &jsonObject) {
    bb::data::JsonDataAccess jda;
    QString jsonStringified;
    jda.saveToBuffer(jsonObject, &jsonStringified);
    return jsonStringified;
}

QString QuantoGasteiJSON::errorInJson(int code, const QString& message) {
    QVariantMap error;
    error["code"] = code;
    error["message"] = message;

    QVariantMap root;
    root["error"] = error;

    return stringify(root);
}

QString QuantoGasteiJSON::resultInJson(const QVariant& value) {
    QVariantMap root;
    root["result"] = value;
    return stringify(root);
}

bool QuantoGasteiJSON::parse(const QString &document, QVariant &jsonObject, QString &errorMsg) {
    bb::data::JsonDataAccess jda;
    QVariant qtData = jda.loadFromBuffer(document);
    if (jda.hasError()) {
        const bb::data::DataAccessError err = jda.error();
        errorMsg = "Error converting JSON data: " + err.errorMessage();
        return false;
    } else {
        jsonObject = qtData;
        return true;
    }
}

bool QuantoGasteiJSON::parse(const QString &document, QVariant &jsonObject) {
    bb::data::JsonDataAccess jda;
    QVariant qtData = jda.loadFromBuffer(document);
    if (jda.hasError()) {
        return false;
    } else {
        jsonObject = qtData;
        return true;
    }
}
