#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "androidrfcomm.h"
#include <QMainWindow>
#include <QMediaPlayer>
#include "client.h"
#include <QTextTableFormat>
#include <QtNetwork>
#include <QDialog>
#include <QListWidgetItem>
#include <QtConcurrent>
QT_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString macrfcomm,AndroidRfComm *bluecomm,QWidget *parent = 0);
    ~MainWindow();
    void SaveMyImage(QString Com);
    void SaveMySong(QString Com);
    void InitMyButton();
    void changemyconfig(QString Com);

public slots:
    void BGamb(QMediaPlayer::MediaStatus);
    void ping();
    void sendping();
    void PingTest();
    void RfcommReload();
    void on_STOP_clicked();    
    void itstime();
    void TimeToInit();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void appendMessage(const QString &from, const QString &message);
    void toldmewhatIshoulddo(QString Com);
    void MyTimer();
    void MyTimerSlot();
    void Timer_reception_PC();


signals:
    void RfcommResult(QString);

private:
    int ACTIF;
    int STATEHS;
    int Recapok;
    int TestNumber;
    int CycleNumber;
    int PingState;
    int nbrping;
    int BASEVOLUME;
    QString ReadVOL;
    QString StringRecap;
    QString StringRecapResult;
    QString IDGROUPE;
    QMediaPlayer *blablaquiveutriendire;
    AndroidRfComm *rfcomm;
    Ui::MainWindow *ui;
    QString IDMACHINE, IDPUPITRE, IDBPTABLETTE, IDBPTELEC, GENERALPATH;
    int VOLUME;
    int TEMPHS;
    int OIFG;
    int WIFISTATE;
    QTimer *RFTime;
    QTimer *ScanTime;
    QTimer *ScanTime2;
    QTimer *ScanTime3;
    QMediaPlayer *player;
    QMediaPlayer *BACKGROUNDSOUND;
    float height;
    float heightP;
    float width;
    float widthP;
    int my_init;
    bool bfr;
    bool ok;
    Client client;
    QString myNickName;
    QTextTableFormat tableFormat;
    QString SonString;
    int SonValue;
    QStringList DeviceList;
    QStringList DeviceScan;
    QString     LastDevice;
    QString     DeviceRfcomm;
    int TeleCommandePresente;
        QFuture<QString> t1;

};


#endif // MAINWINDOW_H
