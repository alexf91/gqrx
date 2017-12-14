#include <QDebug>

#include "echoplugin.h"

void EchoPlugin::printMessage(const QString &msg) {
    qDebug() << "EchoPlugin:" << msg;
}
