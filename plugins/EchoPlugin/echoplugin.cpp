#include <QDebug>

#include "echoplugin.h"

EchoPlugin::~EchoPlugin()
{
    qDebug() << "EchoPlugin unloaded";
}

void EchoPlugin::printMessage(const QString &msg)
{
    qDebug() << "EchoPlugin:" << msg;
}

void EchoPlugin::processSamples(float *buffer, int length)
{
    qDebug() << "EchoPlugin: received" << length << "samples";
}
