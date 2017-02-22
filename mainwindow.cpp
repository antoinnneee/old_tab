/*
 *     connect(ScanTime, SIGNAL(timeout()), discoveryAgent, SLOT(stop()));
 *     connect(discoveryAgent, SIGNAL(canceled()), this, SLOT(startScan()));
 *
 *
 *
 *
 *
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QTimer>
#include "androidrfcomm.h"
#include <QFile>
#include <QFileInfo>
#include <QTextFormat>
#include <QTime>
#include <QtNetwork>
#include <QMenu>
#include <QDebug>
#include <QLabel>
#include <QMovie>
#include <QSound>
#include <QThread>
#include <QtConcurrent/QtConcurrentRun>


MainWindow::MainWindow(QString rfcommmac, AndroidRfComm *bluecom,QWidget *parent) :
    DeviceRfcomm(rfcommmac),rfcomm(bluecom),QMainWindow(parent),
ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    player = new QMediaPlayer;
    GENERALPATH = "/storage/emulated/0/Download/Pupitre/";

    ScanTime = new QTimer(this);
    ScanTime2 = new QTimer(this);
    ScanTime3 = new QTimer(this);
    ScanTime3->start(1900);
    connect(ScanTime3,SIGNAL(timeout()),this, SLOT(sendping()));
    BACKGROUNDSOUND = new QMediaPlayer;
    BACKGROUNDSOUND->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/void.mp3"));
    QTimer::singleShot(600, this, SLOT(TimeToInit()));
    connect(this, SIGNAL(RfcomResult(QString)), this, SLOT(toldmewhatIshoulddo(QString)));
    connect(ScanTime, SIGNAL(timeout()), this, SLOT(PingTest()));
    connect(&client, SIGNAL(newMessage(QString,QString)),
            this, SLOT(appendMessage(QString,QString)));
    connect(ScanTime2, SIGNAL(timeout()), this, SLOT(PingTest()));
    connect(BACKGROUNDSOUND, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this ,SLOT(BGamb(QMediaPlayer::MediaStatus)));

    Timer_reception_PC();
    sendping();
    myNickName = client.nickName();
    tableFormat.setBorder(0);
    nbrping = 0;
    ui->label->hide();
    ui->label->setStyleSheet("background-repeat: no-repeat;""background-position: center; ""text-align: center ;");
    STATEHS = 0;
    OIFG = 0;

}

/***********************Reception provenance pc par rfcomm toute les secondes************************/
void MainWindow::Timer_reception_PC()
{

    QTimer *timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(itstime()));

    timer2->start(1000);
}

void MainWindow::BGamb(QMediaPlayer::MediaStatus status)
{   if (status == QMediaPlayer::EndOfMedia) {
        //spesify silence period here, if needed
        // example how toF create a 2 sec delay between repeats
        //QTimer::singleShot(2000, mediaplayer, SLOT(play()));
        BACKGROUNDSOUND->play();
    }

}

void MainWindow::ping()
{
    BACKGROUNDSOUND->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/void.mp3"));
    BACKGROUNDSOUND->play();
    if (ACTIF)
    {
        nbrping++;
        if (nbrping >= 200)
        {
            nbrping = 0;
            RfcommReload();
        }
        if(PingState)
        {
            PingState = 0;
        }
        else if (PingState == 0)
        {
            qDebug()<< "WIFI ON";
            QProcess::execute("svc wifi enable");
        }
        ScanTime->start(8000);
    }
}
void MainWindow::sendping()
{
    if (ACTIF)
    {
        ScanTime3->stop();
        ScanTime3->start(1900);
        qDebug()<<IDGROUPE + IDMACHINE + "ping";
        client.sendMessage(IDGROUPE + "ping");
    }
}
void MainWindow::PingTest()
{

    if(ACTIF)
    {
        int i(0);
        qDebug() << "PING TEST\n PING TEST\nPING TEST\nPING TEST\n PING TEST\nPING TEST\n";
        if (PingState == 0)
        {
                    qDebug()<< "WIFI OFF";
                    QProcess::execute("svc wifi disable");
        }

        i = qrand() % ((3000) - 1000) + 1001;
        QTimer::singleShot(i,this,SLOT(ping()));
    }

}

//=============================================================================||
void MainWindow::appendMessage(const QString &from, const QString &message)
{
    if(ACTIF)
    {
        if (from.isEmpty() || message.isEmpty())
            return;
        if (message.mid(0,2).contains(IDGROUPE))
        {
            if (message.contains("!H"))
            {
                if (STATEHS == 1)
                {
                    WIFISTATE = 1;
                    toldmewhatIshoulddo("I");
                    rfcomm->sendLine("$I" + message.mid(2,2));
                }
                else
                {
                    WIFISTATE = 1;
                    toldmewhatIshoulddo("H");
                    rfcomm -> sendLine ("$H" + message.mid(2,2) );
                }
            }
            else if (message.contains("DEGAGE"))
            {
                WIFISTATE = 0;
                ScanTime->stop();
                PingTest();
            }
            else if (message.contains("pong"))
            {
                toldmewhatIshoulddo("pong");
                client.sendMessage("POK");
            }

            else if (message.contains("VALID"))
            {
                client.sendMessage("VALIDOK");
                WIFISTATE = 1;
            }
            else if (message.contains("QUIT"))
            {
               rfcomm -> sendLine("$L" + message.mid(2,2)+ IDPUPITRE + "QUIT" );
               qApp->quit();
            }
            else if (message.contains("!RC"))
            {
                RfcommReload();
                client.sendMessage("OK");

            }
            else if (message.contains("!L"))
            {
                WIFISTATE = 1;
                rfcomm -> sendLine("$L" + message.mid(2,2) + IDPUPITRE + message.mid(6,2) + "00" );
                client.sendMessage("OK");
            }
            else if (message.contains("!V"))
            {
                rfcomm->sendLine("$V" + message.mid(2,2) + message.mid(6,2));
            }
            else if (message.contains("!OIFG"))
            {
                WIFISTATE = 1;
                if (OIFG == 0)
                {
                    OIFG = 1;
                    rfcomm -> sendLine("$F" + message.mid(2,2) );
                }
                else
                {
                    OIFG = 0;
                    rfcomm -> sendLine("$G" + message.mid(2,2) );
                 }
            }

        }
    }
    else
    {
        QProcess::execute("svc wifi disable");
    }
}

void MainWindow::toldmewhatIshoulddo(QString Com)
{
    QFile fichier9(GENERALPATH + "img/HS.gif");
    QString volume(Com.mid(0,1));

    if(Com.contains("F"))
    {
        bfr = true;
        if (STATEHS == 0)
        ui->pushButton->show();
        client.sendMessage("OK");
    }
    else if(Com.contains("G"))
    {
        bfr = false;
        ui->pushButton->hide();
        client.sendMessage("OK");
    }
    else if(Com.contains("H")) //affichage hors service
    {
        ui->pushButton->hide();
        ui->pushButton_2->hide();
        ui->pushButton_3->hide();
        ui->pushButton_4->hide();
        ui->pushButton_5->hide();
        ui->pushButton_6->hide();
        ui->pushButton_7->hide();
        ui->STOP->hide();
        STATEHS = 1;
        if(fichier9.exists())
        {

            ui->centralWidget->show();
            QMovie *movie = new QMovie(GENERALPATH + "img/HS.gif");
            ui->label->setMovie(movie);
            ui->label->setScaledContents(true);
            ui->label->show();
            ui->label->setStyleSheet("background-repeat: no-repeat;""background-position: center; ""text-align: center ;""background-size: 100%;");
            movie->start();


        }
        player->stop();
        client.sendMessage("OK");
        MyTimer();    // lancement timer
    }
    else if (Com.contains("I"))
    {
        MyTimerSlot();
    }
    else if (Com.contains("#C"))
        changemyconfig(Com);
    else if (Com.contains("pong"))
    {
        PingState = 1;

    }
    else if(Com.contains("#RC"))
        RfcommReload();

    if (volume.contains("V"))
    {
        SonString = Com.mid(1); // enleve le "V"
        qDebug() << SonString;
        SonValue = SonString.toInt(&ok);
        player->setVolume(SonValue);
        client.sendMessage("OK");
    }
//    rfcomm->sendLine("Value OK");
}


void MainWindow::TimeToInit() // INITIALISATION
{   /*********************lecture config***************************/
    QString texte;
    GENERALPATH = "/storage/emulated/0/Download/Pupitre/";
    QFile fichier(GENERALPATH + "config/config.text");
    if(fichier.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       QTextStream flux(&fichier);
       IDMACHINE = flux.readLine();
       IDPUPITRE = flux.readLine();
       IDBPTABLETTE = flux.readLine();
       BASEVOLUME = flux.readLine().toInt(&ok, 10);
       TEMPHS = flux.readLine().toInt(&ok,10);
       ACTIF = flux.readLine().toInt(&ok,10);
       IDGROUPE = flux.readLine();
    }
    fichier.close();
    OIFG = 1;
    STATEHS = 0;
    VOLUME = BASEVOLUME;
    height = this->geometry().height();
    width  = this->geometry().width();
    heightP = height/100;
    widthP = width/100;
    TestNumber = 0;
    CycleNumber = 0;
    Recapok = 0;
    PingState = 0;
    ui->gridLayoutWidget->setGeometry(0,0,width,height);
    ui->horizontalSpacer->changeSize(widthP*12,heightP * 1,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->horizontalSpacer_4->changeSize(widthP*2,heightP * 1,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->horizontalSpacer_3->changeSize(widthP*2,heightP * 1,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->horizontalSpacer_9->changeSize(widthP*2,heightP * 1,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->horizontalSpacer_11->changeSize(widthP*2,heightP * 1,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->verticalSpacer->changeSize(widthP*1,heightP * 15,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->verticalSpacer_2->changeSize(widthP*1,heightP * 10,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->verticalSpacer_3->changeSize(widthP*1,heightP * 15,QSizePolicy::Maximum,QSizePolicy::Maximum);
    WIFISTATE = 0;
    LastDevice = "0";
    QProcess::execute("svc wifi disable");
    InitMyButton();
 //ui->list->hide();
    ping();
}

void MainWindow::RfcommReload()
{
    qDebug() << " try rfcomm";
        rfcomm->disconnect();
        if(!rfcomm->isConnected())
        rfcomm->connect(DeviceRfcomm);
}

/**********************reception rfcomm regulierement *****************/
void MainWindow::itstime()
{
    qDebug() << "RECEIVE";
    QString Com = rfcomm->receiveLine(100,0);
    if (!Com.isEmpty())
    {
        qDebug() << "rfcomm \n rfcomm \n               COM                    COM                      cOM            COM\n";
        if (Com.contains("#F"))
        {
            toldmewhatIshoulddo("F");
            rfcomm->sendLine("$!" + IDMACHINE);

        }
        else if (Com.contains("#G"))
        {
            toldmewhatIshoulddo("G");
            rfcomm->sendLine("$!" + IDMACHINE);

        }
        else if (Com.contains("#QUIT"))
        {
           rfcomm -> sendLine("$L" + IDMACHINE + IDPUPITRE + "QUIT" );
           qApp->quit();
        }
        else if (Com.contains("#H"))
        {
            toldmewhatIshoulddo("H");
            rfcomm->sendLine("$!" + IDMACHINE);

        }
        else if (Com.contains("#I"))
        {
            toldmewhatIshoulddo("I");
            rfcomm->sendLine("$!" + IDMACHINE);
        }
        else if (Com.contains("#V"))
        {
            toldmewhatIshoulddo(Com.mid(1,3));
            rfcomm->sendLine("$!" + IDMACHINE);
        }
        else if (Com.contains("#RC"))
            RfcommReload();
        else if (Com.contains("#C"))
        {
            toldmewhatIshoulddo(Com);
            rfcomm->sendLine("$!" + IDMACHINE);
        }
        else if (Com.contains("#Q"))
        {
            qApp->quit();
            rfcomm->sendLine("$!" + IDMACHINE);
        }
        else
        {
            qDebug()<< Com;
            rfcomm->sendLine("$?" + IDMACHINE);
        }
    }

}
/***********************tempo reaffichage icones apres hors service*************************/
void MainWindow::MyTimer()
{
    QTimer::singleShot(TEMPHS*1000, this, SLOT(MyTimerSlot()));
}

void MainWindow::MyTimerSlot()
{
    if (bfr==true)
    {
        STATEHS = 0;
    ui->pushButton->show();
    ui->pushButton_2->show();
    ui->pushButton_3->show();
    ui->pushButton_4->show();
    ui->pushButton_5->show();
    ui->pushButton_6->show();
    ui->pushButton_7->show();
    ui->STOP->show();
    ui->label->hide();
    ui->centralWidget->setStyleSheet("background-image: url(/storage/emulated/0/Download/Pupitre/img/f.png)");
    }
    else
    {
        STATEHS = 0;
    ui->pushButton->hide();
    ui->pushButton_2->show();
    ui->pushButton_3->show();
    ui->pushButton_4->show();
    ui->pushButton_5->show();
    ui->pushButton_6->show();
    ui->pushButton_7->show();
    ui->STOP->show();
    ui->label->hide();
    ui->centralWidget->setStyleSheet("background-image: url(/storage/emulated/0/Download/Pupitre/img/f.png)");

    }
}


void MainWindow::InitMyButton()
{
    int                     Num(1);
    QAndroidJniObject       mediaDir = QAndroidJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
    QAndroidJniObject       mediaPath = mediaDir.callObjectMethod( "getAbsolutePath", "()Ljava/lang/String;" );
    QString                 dataAbsPath = mediaPath.toString()+"/Download/Pupitre/" + Num + ".png";
    QAndroidJniEnvironment  env;
    // fond d'ecran
    QFile fichier8(GENERALPATH + "img/f.png");
    if(fichier8.exists())
        ui->centralWidget->setStyleSheet("background-image: url(" + GENERALPATH + "img/f.png);""background-repeat: no-repeat;""background-position: center; ");
    //Bouton 1
    QFile fichier(GENERALPATH + "img/1.png");
    if(fichier.exists())
        ui->pushButton->setStyleSheet("background-image: url(" + GENERALPATH + "img/1.png);""background-repeat: no-repeat;""background-position: center; ");
    //Bouton2
    QFile fichier1(GENERALPATH + "img/2.png");
    if(fichier1.exists())
        ui->pushButton_2->setStyleSheet("background-image: url( " + GENERALPATH + "img/2.png);""background-repeat: no-repeat;""background-position: center; ");

    //Bouton3
    QFile fichier2(GENERALPATH + "img/3.png");
    if(fichier2.exists())
        ui->pushButton_3->setStyleSheet("background-image: url( " + GENERALPATH + "img/3.png);""background-repeat: no-repeat;""background-position: center; ");

    //Bouton4
    QFile fichier3(GENERALPATH + "img/4.png");
    if(fichier3.exists())
        ui->pushButton_4->setStyleSheet("background-image: url( " + GENERALPATH + "img/4.png);""background-repeat: no-repeat;""background-position: center; ");

    //Bouton5
    QFile fichier4(GENERALPATH + "img/5.png");
    if(fichier4.exists())
        ui->pushButton_5->setStyleSheet("background-image: url( " + GENERALPATH + "img/5.png);""background-repeat: no-repeat;""background-position: center; ");

    //Bouton6
    QFile fichier5(GENERALPATH + "img/6.png");
    if(fichier5.exists())
        ui->pushButton_6->setStyleSheet("background-image: url( " + GENERALPATH + "img/6.png);""background-repeat: no-repeat;""background-position: center; ");

    //Bouton7
    QFile fichier6(GENERALPATH + "img/7.png");
    if(fichier6.exists())
        ui->pushButton_7->setStyleSheet("background-image: url( " + GENERALPATH + "img/7.png);""background-repeat: no-repeat;""background-position: center; ");

    //Bouton stop
    QFile fichier7(GENERALPATH + "img/8.png");
    if(fichier7.exists())
        ui->STOP->setStyleSheet("background-image: url( " + GENERALPATH + "img/8.png);""background-repeat: no-repeat;""background-position: center; ");
    else
        exit(0);
}
void MainWindow::changemyconfig(QString Com)
{

    QString idpupitre       = Com.mid(4,2);
    QString idbptablette    = Com.mid(6,2);
    QString volume          = Com.mid(8,2);
    QString tempo           = Com.mid(10,3);
    QString actif           = Com.mid(13,1);
    QString idgroupe        = Com.mid(14,2);
    QString texte           = IDMACHINE + "\n" + idpupitre + "\n" + idbptablette + "\n" + volume + "\n" + tempo + "\n" + actif + "\n" + idgroupe;
    QFile FichierConf(GENERALPATH + "config/config.text");
    QTextStream flux(&FichierConf);
    if(FichierConf.exists())
    {
        FichierConf.open(QIODevice::WriteOnly |  QIODevice::Truncate | QIODevice::Text);
        flux << texte;
    }
    FichierConf.close();
}

void MainWindow::on_pushButton_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "01");
    player->stop();
    player->setVolume(VOLUME);
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/01.mp3"));
    player->play();
    VOLUME = BASEVOLUME;

}
void MainWindow::on_pushButton_2_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "02");
    player->stop();
    player->setVolume(VOLUME);
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/02.mp3"));
    player->play();
    VOLUME = BASEVOLUME;
}

void MainWindow::on_pushButton_3_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "03");
    player->stop();
    player->setVolume(VOLUME);
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/03.mp3"));
    player->play();
    VOLUME = BASEVOLUME;
}
void MainWindow::on_pushButton_4_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "04");
    player->stop();
        player->setVolume(VOLUME);
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/04.mp3"));
    player->play();
    VOLUME = BASEVOLUME;
}
void MainWindow::on_pushButton_5_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "05");
    player->stop();
        player->setVolume(VOLUME);
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/05.mp3"));
    player->play();
    VOLUME = BASEVOLUME;
}
void MainWindow::on_pushButton_6_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "06");
    player->stop();
        player->setVolume(VOLUME);
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/06.mp3"));
    player->play();
    VOLUME = BASEVOLUME;
}
void MainWindow::on_pushButton_7_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "07");
    player->stop();
        player->setVolume(VOLUME);
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/07.mp3"));
    player->play();
    VOLUME = BASEVOLUME;
}
void MainWindow::on_STOP_clicked()
{
    VOLUME = BASEVOLUME;
    player->stop();
}

MainWindow::~MainWindow()
{
    delete ui;
}
