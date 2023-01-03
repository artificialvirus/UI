/*
 *
 *    ______
 *   /_  __/___  ____ ___  ___  ____
 *    / / / __ \/ __ `__ \/ _ \/ __ \
 *   / / / /_/ / / / / / /  __/ /_/ /
 *  /_/  \____/_/ /_/ /_/\___/\____/
 *              video for sports enthusiasts...
 *
 *  2811 cw3 : twak
 */

#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QSlider>
#include <QLabel>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QAction>
#include <QShortcut>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QFileDialog>
#include "the_player.h"
#include "the_button.h"
#include "QScrollArea"



using namespace std;




// read in videos and thumbnails to this directory
vector<TheButtonInfo> getInfoIn (string loc) {

    vector<TheButtonInfo> out =  vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

        if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

                QString thumb = f.left( f .length() - 4) +".png";
                if (QFile(thumb).exists()) { // if a png thumbnail exists
                    QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list


                    }
                    else
                        qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << Qt::endl;
                }
                else
                    qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << Qt::endl;
            }
    }


    return out;
}


int main(int argc, char *argv[]) {



    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR ;

    // create the Qt Application
    QApplication app(argc, argv);

    // collect all the videos in the folder
    vector<TheButtonInfo> videos;

    //< Create path variable
    string path = argc < 2 ?  "" : argv[1];

    if (argc < 2 || QMessageBox::question(NULL, QString("Default Directory"), QString("Press Yes to keep the default directory, otherwise press No to change"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No){
        path = QFileDialog::getExistingDirectory(NULL,QString("Open Directory"), NULL).toStdString();  //< Get a directory path from user
    }

    //< Error checking whether path is empty
    if (path == ""){
        QMessageBox::critical(NULL, QString("Tomeo"), QString("No directory selected!"));
        exit(EXIT_FAILURE);
    }

    videos = getInfoIn( path);

    if (videos.size() == 0) {

        const int result = QMessageBox::question(
                    NULL,
                    QString("Tomeo"),
                    QString("no videos found! download, unzip, and add command line argument to \"quoted\" file location. Download videos from Tom's OneDrive?"),
                    QMessageBox::Yes |
                    QMessageBox::No );

        switch( result )
        {
        case QMessageBox::Yes:
            QDesktopServices::openUrl(QUrl("https://leeds365-my.sharepoint.com/:u:/g/personal/scstke_leeds_ac_uk/EcGntcL-K3JOiaZF4T_uaA4BHn6USbq2E55kF_BTfdpPag?e=n1qfuN"));
            break;
        default:
            break;
        }
        exit(-1);
    }

    // the widget that will show the video

    VideoWidget *vidWid = new VideoWidget;

    // the QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(vidWid);
    player->playlistvids=getInfoIn( path);

    //full screen mode
    // Double click video for full screen
    QHBoxLayout *fullScreenLayout = new QHBoxLayout;

    fullScreenLayout->addWidget(vidWid);

    QHBoxLayout* playlist = new QHBoxLayout;
    QHBoxLayout* vid = new QHBoxLayout;
    vid->addWidget(vidWid);

    //< Add the slider and duration(progress) label of the player to a QHBoxLayout.
    QWidget *sliderArea = new QWidget();
    sliderArea->setFixedHeight(40);
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    sliderLayout->addWidget(player->slider);
    sliderLayout->addWidget(player->durationLabel);
    sliderArea->setLayout(sliderLayout);

    // a row of buttons
    QWidget *buttonWidget = new QWidget();
    // a list of the buttons
    vector<TheButton*> buttons;

    // the buttons are arranged horizontally
    QHBoxLayout *layout = new QHBoxLayout();
    buttonWidget->setLayout(layout);

    // create the four buttons

    for ( uint i = 0; i < videos.size(); i++ ) {

        TheButton *button = new TheButton(buttonWidget);
        QVBoxLayout* lay  = new QVBoxLayout;
        lay ->addWidget(button);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo* ))); // when clicked, tell the player to play.
        buttons.push_back(button);
        layout->addLayout(lay);
        button->init(&videos.at(i));
        //shows the name of the video
        QUrl* button_info = button->info->url; //gets the file path of the video
        QString filename = button_info->fileName(); //gets the name of the video
        QLabel *name = new QLabel(filename);  //creates a label for the video name
        //name->setAlignment( Qt::AlignCenter);
        QHBoxLayout* form = new QHBoxLayout;
        form->addWidget(name); //adds the name of the video
        //icons that allow the user to add the video to playlist
        TheButton* favourite = new TheButton(buttonWidget);
        favourite->init(&videos.at(i));
        favourite->connect(favourite,SIGNAL( addPlaylist(TheButtonInfo*)),player,SLOT(addPlaylist(TheButtonInfo*)));
        favourite->setIcon(QIcon(":/blackHeart.png"));
        favourite->setIconSize(QSize(20,20));
        favourite->resize(QSize(20,20));
        form->addWidget(favourite);
        lay->addLayout(form);
    }
    //creats the playlist
    //adds the video to the playlist
    QVBoxLayout *ftitle = new QVBoxLayout;
    QLabel* title = new QLabel("Favourites");
    title->setAlignment( Qt::AlignCenter);
    ftitle->addWidget(title);
    ftitle->addWidget(player->lists);
    player->addtofave->setLayout(player->fbuttonLayout);
    player->addtofave->setMinimumHeight(0);
    player->lists->setMaximumWidth(250);
    player->lists->setMinimumWidth(250);
    playlist->addWidget(vidWid);
    playlist->addLayout(ftitle); //add playlists to the video player

    //creates a horizontal scroll area for the buttons above
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidget(buttonWidget);
    scroll->setMinimumHeight(150);
    scroll->setMaximumHeight(150);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    //creates a layout for play/pause and mute etc
    QHBoxLayout* playback = new QHBoxLayout;

    //creating play/pause button
    QPushButton *playButton = new QPushButton();
    playButton -> setIcon(QIcon(":/play_pause.png")); //creating icon for the button
    playButton->connect(playButton, SIGNAL(clicked()), player, SLOT (toPlay()));
    playback->addWidget(playButton);
    //creating stop button
    QPushButton *stopButton = new QPushButton();
    stopButton->setIcon(QIcon(":/stop.png"));//creating icon for the button
    stopButton->connect(stopButton, SIGNAL(clicked()), player, SLOT (toStop()));
    playback->addWidget(stopButton);
    //creating skip button to first video
    QPushButton* skipStartButton = new QPushButton();
    skipStartButton->setIcon(QIcon(":/start.png")); //creating icon for the button
    skipStartButton->connect(skipStartButton, SIGNAL(clicked()), player, SLOT(skipToStart()));
    playback->addWidget(skipStartButton);
    //creating previous button
    QPushButton* prevButton = new QPushButton();
    prevButton->setIcon(QIcon(":/previous.png")); //creating icon for the button
    prevButton->connect(prevButton, SIGNAL(clicked()), player, SLOT(toPrev()));
    playback->addWidget(prevButton);
    //creating next button
    QPushButton* nextButton = new QPushButton();
    nextButton->setIcon(QIcon(":/next.png")); //creating icon for the button
    nextButton->connect(nextButton, SIGNAL(clicked()), player, SLOT(toNext()));
    playback->addWidget(nextButton);
    //creating skip button to last video
    QPushButton* skipEndButton = new QPushButton();
    skipEndButton->setIcon(QIcon(":/end.png")); //creating icon for the button
    skipEndButton->connect(skipEndButton, SIGNAL(clicked()), player, SLOT(skipToEnd()));
    playback->addWidget(skipEndButton);

    //creating a mute button
    QPushButton* muteButton = new QPushButton();
    muteButton->setIcon(QIcon(":/speaker.png"));
    muteButton->connect(muteButton, SIGNAL(clicked()), player, SLOT(muteClicked()));
    playback->addWidget(muteButton);

    //creating a slider for the volume
    QSlider* volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setFixedWidth(100);
    volumeSlider->setValue(0); //sets the volume to zero
    volumeSlider->connect(volumeSlider, SIGNAL(valueChanged(int)), player, SLOT(setVolume(int)));
    playback->addWidget(volumeSlider);



    //shortcuts
    //when the keyboard input are correct the slot is activated

    //controls Ctrl + M
    //mute shortcut
    QAction *mute_short = new QAction();
    mute_short->setShortcut(Qt::Key_M | Qt::CTRL);
    mute_short->connect(mute_short, SIGNAL(triggered()), player, SLOT(muteClicked()));
    //controls Ctrl + P
    //play/pause shortcut
    QAction *play_short = new QAction();
    play_short->setShortcut(Qt::Key_P | Qt::CTRL);
    play_short->connect(play_short, SIGNAL(triggered()), player, SLOT(toPlay()));
    //controls Ctrl + S
    //stop shortcut
    QAction *stop_short = new QAction();
    stop_short->setShortcut(Qt::Key_S | Qt::CTRL);
    stop_short->connect(stop_short, SIGNAL(triggered()), player, SLOT(toStop()));
    //controls Ctrl + D
    //next shortcut
    QAction *next_short = new QAction();
    next_short->setShortcut(Qt::Key_D | Qt::CTRL);
    next_short->connect(next_short, SIGNAL(triggered()), player, SLOT(toNext()));
    //controls Ctrl + A
    //prev shortcut
    QAction *prev_short = new QAction();
    prev_short->setShortcut(Qt::Key_A | Qt::CTRL);
    prev_short->connect(prev_short, SIGNAL(triggered()), player, SLOT(toPrev()));
    //controls Ctrl + E
    //skip to the end shortcut
    QAction *end_short = new QAction();
    end_short->setShortcut(Qt::Key_E | Qt::CTRL);
    end_short->connect(end_short, SIGNAL(triggered()), player, SLOT(skipToEnd()));
    //controls Ctrl + W
    //skip to the start shortcut
    QAction *start_short = new QAction();
    start_short->setShortcut(Qt::Key_W | Qt::CTRL);
    start_short->connect(start_short, SIGNAL(triggered()), player, SLOT(skipToStart()));

    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);

    // create the main window and layout
    QWidget window;
    QVBoxLayout *top = new QVBoxLayout();

    //Adding shortvut area and separating the controls
    QVBoxLayout* allcontrols = new QVBoxLayout;
    allcontrols->addWidget(sliderArea);
    allcontrols->addLayout(playback);
    allcontrols->addWidget(scroll);
    QHBoxLayout *lower = new QHBoxLayout;
    QVBoxLayout* shortcuts = new QVBoxLayout;
    //shortcuts list
    QLabel* stitle = new QLabel("\n\n\n\n\n\n\nShortcuts:\n"
                                "Ctrl + P: Play and Pause\n"
                                "Ctrl + M: Mute and unmute\n"
                                "Ctrl + S: Stop\n"
                                "Ctrl + D: Next\n"
                                "Ctrl + A: Previous\n"
                                "Ctrl + E: Skip to the end\n"
                                "Ctrl + W: Skip to the start\n"
                                "Double click Video: Enter Fullscreen\n"
                                "Esc: Leave Fullscreen "
                                "");
    stitle->setAlignment( Qt::AlignLeft);
    stitle->setMinimumSize(200,250);
    stitle->setMaximumSize(200,250);
    shortcuts->addWidget(stitle);
    //Making the layouts for video player
    lower->addLayout(allcontrols);
    lower->addLayout(shortcuts);

    window.setLayout(top);

    window.setWindowTitle("tomeo");
    window.setMinimumSize(800, 680);

    // add keyboard shortcuts
    window.addAction(mute_short);
    window.addAction(play_short);
    window.addAction(stop_short);
    window.addAction(next_short);
    window.addAction(prev_short);
    window.addAction(end_short);
    window.addAction(start_short);
    top->addLayout(playlist);
    //top->addWidget(fullScreenArea);
    top->addLayout(lower);
    //allows the video player to accept both keyboard and mouse inputs
    window.setFocusPolicy(Qt::FocusPolicy::StrongFocus);

    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}
