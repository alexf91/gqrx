#include <QDebug>

#include "afsk1200.h"


AFSK1200::AFSK1200(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AFSK1200)
{
    ui->setupUi(this);

    /* select font for text viewer */
#ifdef Q_OS_MAC
    ui->textView->setFont(QFont("Monaco", 12));
#else
    ui->textView->setFont(QFont("Monospace", 11));
#endif

    /* Add right-aligned info button */
    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->toolBar->addWidget(spacer);
    ui->toolBar->addAction(ui->actionInfo);

    /* AFSK1200 decoder */
    decoder = new CAfsk12(this);
    connect(decoder, SIGNAL(newMessage(QString)), ui->textView, SLOT(appendPlainText(QString)));
}


AFSK1200::~AFSK1200()
{
    qDebug() << "AFSK1200 decoder destroyed.";

    delete decoder;
    delete ui;
}


void AFSK1200::initialize(QObject *parent)
{
    this->setParent(qobject_cast<QWidget *>(parent));
}


void AFSK1200::printMessage(const QString &msg)
{
    qDebug() << "AFSK1200:" << msg;
}

/*! \brief Process new set of samples. */
void AFSK1200::processSamples(float *buffer, int length)
{
    int overlap = 18;
    int i;

    for (i = 0; i < length; i++) {
        tmpbuf.append(buffer[i]);
    }

    decoder->demod(tmpbuf.data(), length);

    /* clear tmpbuf and store "overlap" */
    tmpbuf.clear();
    for (i = length-overlap; i < length; i++) {
        tmpbuf.append(buffer[i]);
    }
}

/*! \brief User clicked on the Clear button. */
void AFSK1200::on_actionClear_triggered()
{
    ui->textView->clear();
}


/*! \brief User clicked on the Save button. */
void AFSK1200::on_actionSave_triggered()
{
    /* empty text view has blockCount = 1 */
    if (ui->textView->blockCount() < 2) {
        QMessageBox::warning(this, tr("Gqrx error"), tr("Nothing to save."),
                             QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    QDir::homePath(),
                                                    tr("Text Files (*.txt)"));

    if (fileName.isEmpty()) {
        qDebug() << "Save cancelled by user";
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error creating file: " << fileName;
        return;
    }

    QTextStream out(&file);
    out << ui->textView->toPlainText();
    file.close();
}


/*! \brief User clicked Info button. */
void AFSK1200::on_actionInfo_triggered()
{
    QMessageBox::about(this, tr("About AFSK1200 Decoder"),
                       tr("<p>Gqrx AFSK1200 Decoder %1</p>"
                          "<p>The Gqrx AFSK1200 decoder taps directly into the SDR signal path "
                          "eliminating the need to mess with virtual or real audio cables. "
                          "It can decode AX.25 packets and displays the decoded packets in a text view.</p>"
                          "<p>The decoder is based on Qtmm, which is avaialble for Linux, Mac and Windows "
                          "at <a href='http://qtmm.sf.net/'>http://qtmm.sf.net</a>.</p>"
                          ).arg(VERSION));

}
