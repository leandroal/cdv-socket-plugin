/*
 * QuantoGasteiJSON.hpp
 *
 *  Created on: 15/10/2014
 *      Author: leandro
 */

#ifndef QUANTOGASTEIJSON_HPP_
#define QUANTOGASTEIJSON_HPP_

#include <QObject>

/*
 * The following constants are defined based on Cordova Globalization
 * plugin definition. They should match exactly.
 * https://github.com/apache/cordova-plugin-globalization/blob/master/doc/index.md
*/
const int UNKNOWN_ERROR = 0;
const int FORMATTING_ERROR = 1;
const int PARSING_ERROR = 2;
const int PATTERN_ERROR = 3;

class QuantoGasteiJSON: public QObject {
    Q_OBJECT
public:
    QuantoGasteiJSON(QObject *parent = 0);
    ~QuantoGasteiJSON();

    bool parse(const QString &document, QVariant &jsonObject, QString &errorMsg);
    bool parse(const QString &document, QVariant &jsonObject);

    QString stringify(const QVariant &jsonObject);
    QString errorInJson(int code, const QString& message);

    QString resultInJson(const QVariant& value);
//    QString resultInJson(bool value);
//    QString resultInJson(int value);
//    QString resultInJson(double value);

private:

};

#endif /* QUANTOGASTEIJSON_HPP_ */
