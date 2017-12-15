#include <QDebug>

#include "dockplugin.h"


DockPlugin::DockPlugin(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockPlugin)
{
    ui->setupUi(this);
}


DockPlugin::~DockPlugin()
{
    delete ui;
}

void DockPlugin::initialize(QObject *parent)
{
    this->setParent(qobject_cast<QWidget *>(parent));
}

void DockPlugin::printMessage(const QString &msg)
{
    qDebug() << "DockPlugin:" << msg;
}

/*! \brief Process new set of samples. */
void DockPlugin::processSamples(float *buffer, int length)
{

}
