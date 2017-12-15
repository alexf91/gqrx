#include <QtPlugin>
#include <QDockWidget>
#include <QString>
#include <QVarLengthArray>
#include <QMessageBox>
#include <QFileDialog>

#include <plugininterface.h>
#include "ui_dockplugin.h"


class DockPlugin : public QDockWidget, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "dk.gqrx.Gqrx.PluginInterface" FILE "dockplugin.json")
    Q_INTERFACES(PluginInterface)

public:
    explicit DockPlugin(QWidget *parent=0);
    ~DockPlugin();

    /* PluginInterface */
    void initialize(QObject *parent);
    void printMessage(const QString &msg) override;
    void processSamples(float *buffer, int length) override;

private:
    Ui::DockPlugin *ui;
};
