/*
 * QuantoGasteiSleeper.hpp
 *
 *  Created on: 16/10/2014
 *      Author: leandro
 */

#ifndef QUANTOGASTEISLEEPER_HPP_
#define QUANTOGASTEISLEEPER_HPP_

#include <QThread>

class QuantoGasteiSleeper : public QThread {
    Q_OBJECT
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};

#endif /* QUANTOGASTEISLEEPER_HPP_ */
