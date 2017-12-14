#include <QDebug>

#include "echoplugin.h"

void EchoPlugin::printMessage(const QString &msg)
{
    qDebug() << "EchoPlugin:" << msg;
}

void EchoPlugin::processSamples(float *buffer, int length)
{
    qDebug() << "EchoPlugin: received" << length << "samples";
}
