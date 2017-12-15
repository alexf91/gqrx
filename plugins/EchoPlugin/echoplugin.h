#include <QtPlugin>
#include <QString>

#include <plugininterface.h>

class EchoPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "dk.gqrx.Gqrx.PluginInterface" FILE "echoplugin.json")
    Q_INTERFACES(PluginInterface)

public:
    ~EchoPlugin();
    void initialize(QObject *parent);
    void printMessage(const QString &msg) override;
    void processSamples(float *buffer, int length) override;
};
