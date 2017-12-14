#include <QtPlugin>
#include <QMainWindow>
#include <QString>
#include <QVarLengthArray>
#include <QMessageBox>
#include <QFileDialog>

#include <plugininterface.h>
#include "afsk1200/cafsk12.h"
#include "ui_afsk1200.h"


class AFSK1200 : public QMainWindow, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "dk.gqrx.Gqrx.PluginInterface" FILE "afsk1200.json")
    Q_INTERFACES(PluginInterface)

public:
    explicit AFSK1200(QWidget *parent=0);
    ~AFSK1200();

    /* PluginInterface */
    void printMessage(const QString &msg) override;
    void processSamples(float *buffer, int length) override;

private slots:
    void on_actionClear_triggered();
    void on_actionSave_triggered();
    void on_actionInfo_triggered();

private:
    Ui::AFSK1200 *ui;
    CAfsk12 *decoder;     /*! The AFSK1200 decoder object. */
    QVarLengthArray<float, 16384> tmpbuf;   /*! Needed to remember "overlap" smples. */
};
