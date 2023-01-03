//
// Created by twak on 11/11/2019.
//

#include "the_player.h"
#include "the_button.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMessageBox>

using namespace std;
// all buttons have been setup, store pointers here
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons -> at(0) -> info);
    play();
    //initiales the array to allow next, previous, skips buttons
    for(uint x = 0; x < i->size(); x++){}

}
//finds the index of which video is at in the videos arryas
void ThePlayer::find_array(TheButtonInfo *button){
    for(uint x = 0; x< infos->size(); x++ ){
        QString file_name;
        file_name = button->url->toDisplayString();
        string name = file_name.toStdString();
        QString title;
        title = infos->at(x).url->toDisplayString();
        string y = title.toStdString();
        if (y.compare(name)==0){

            index = x;
        }
    }
}
//either pause or calls the toPlay button to play/pause the video
void ThePlayer::playClicked()
{
    switch (playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        emit toPlay();
        break;
    case QMediaPlayer::PlayingState:
        pause();
        break;
    }
}


//creates the playlists
void ThePlayer::addPlaylist(TheButtonInfo* button){
    QWidget* tolist = new QWidget;
    for (uint i = 0;i<playlistvids.size();i++){
        QString file_path;
        file_path = button->url->toDisplayString();//gets the file path
        string name = file_path.toStdString();
        QString file_name;
        file_name = playlistvids.at(i).url->toDisplayString(); //gets the file path of the video
        string title = file_name.toStdString();
        if (title.compare(name)==0){
            //adds and display the videos buttons to playlist
            QWidget* onebutton = new QWidget;
            TheButton *buttons = new TheButton(this->addtofave);
            QHBoxLayout* hlayout = new QHBoxLayout;
            QVBoxLayout* list_layout = new QVBoxLayout;
            TheButton* del = new TheButton(this->addtofave);

            hlayout->addWidget(del);
            hlayout->addLayout(list_layout);
            onebutton->setLayout(hlayout);
            list_layout->addWidget(buttons);
            buttons->connect(buttons, SIGNAL(jumpTo(TheButtonInfo* )), this, SLOT (jumpTo(TheButtonInfo* ))); // when clicked, tell the player to play.
            onebutton->setFixedHeight(100);
            fbuttonLayout->addWidget(onebutton);   //extern layout
            buttons->init(&playlistvids.at(i));
            del->init(&playlistvids.at(i));

            del->connect(del,SIGNAL( addPlaylist(TheButtonInfo*)),this,SLOT(delvids(TheButtonInfo*)));
            del->setMaximumSize(20,20);
            //set icon for del button
            del->setIcon(QIcon(":/cross.png"));
            del->setIconSize(QSize(15,15));
            //displays the name of the file
            QUrl* list_button = buttons->info->url;
            QString s = list_button->fileName();
            QLabel *list_name = new QLabel(s);
            list_name->setAlignment( Qt::AlignCenter);
            list_layout->addWidget(list_name);


            addtofave->setMinimumHeight(addtofave->height()+100);
        }

    }

    tolist->setLayout(fbuttonLayout);

    lists->setWidget(tolist);//adds the video butons to the scroll area


}
//shows what mute is on or not
bool ThePlayer::isMuted() const
{
    return playerMuted;
}
//changes the mute bool and set mute or unmute
void ThePlayer::muted(bool muted)
{
    if (muted != playerMuted) {
        playerMuted = muted;
        if(muted==true){
            setMuted(true);
        }
        if(muted==false){
            setMuted(false);
        }
    }
}

void ThePlayer::muteClicked()
{
    muted(!isMuted());
}

void ThePlayer::jumpTo (TheButtonInfo* button) {
    setMedia( * button -> url);
    play();
}
//play/pause video
void ThePlayer::toPlay() {
    switch (playing_state) {
    case QMediaPlayer::State::PlayingState:
        pause(); // starting playing again...
        playing_state = QMediaPlayer::State::PausedState;
        break;
    case QMediaPlayer::State::PausedState:
        setPlaybackRate(1);
        play(); // starting playing again...
        playing_state = QMediaPlayer::State::PlayingState;
        break;
    case QMediaPlayer::State::StoppedState:
        setPlaybackRate(1);
        play(); // starting playing again...
        playing_state = QMediaPlayer::State::PlayingState;
        break;
    default:
        break;
    }
}
//restart the viso playing
void ThePlayer::toStop() {
    stop();
    playing_state = QMediaPlayer::State::StoppedState;
}

//plays the next video
void ThePlayer::toNext(){
    int max = infos->size();
    if(index <max - 1){
        index +=1;
        jumpTo(&infos -> at(index));
    }
}
//plays the previous video
void ThePlayer::toPrev(){
    if(index !=0){
        index -=1;
        jumpTo(&infos -> at(index));
    }
}
//skips to play the first video
void ThePlayer::skipToStart(){
    jumpTo(&infos -> at(0));
}
//skips to play the last video
void ThePlayer::skipToEnd(){
    int end=infos->size();
    jumpTo(&infos -> at(end-1));
}

//< Progress slider and label related slots and function implementation
void ThePlayer::seek(int seconds){
    setPosition(seconds*1000); //< set the position to where the user want's to jump
}

void ThePlayer::durationUpdated(qint64 msec){
    slider->setMaximum(msec/1000); //< update the range of the slider everytime duration changes
}

void ThePlayer::positionChanged(qint64 progress){
    //< Update the slider and duration label progress indicators
    durationLabel->setText(composeDurationLabel(progress/1000, duration()/1000));
    slider->setSliderPosition(progress/1000);
}


QString ThePlayer::composeDurationLabel(int prog, int duration){
    QString str = "--:-- / --:--";  //< default string

    if (duration){
        //< Set string to mm:ss or hh:mm:ss depending on the length of the video
        QTime progress(prog/3600, (prog/60)%60, prog%60, 0);
        QTime max(duration/3600, (duration/60)%60, duration%60, 0);
        QString format = duration <= 3600 ? "mm:ss": "hh:mm:ss";
        str = progress.toString(format) + " / " + max.toString(format);
    }
    return str;
}


//Full Screen function implementation

bool ThePlayer::isPlayerAvailable() const {
    return player->isAvailable();
}

VideoWidget::VideoWidget(QWidget *parent)
    : QVideoWidget(parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

}
//Allows the user to escape the full screen
void VideoWidget::keyPressEvent(QKeyEvent *event) {
    if (event -> key() == Qt::Key_Escape && isFullScreen()) {
        setFullScreen(false);
        event->accept();
    } else if(event->key() == Qt::Key_Enter && event->modifiers() & Qt::Key_Alt) {
        setFullScreen(!isFullScreen());
        event->accept();
    } else {
        QVideoWidget::keyPressEvent(event);
    }
}
//allows the video player to accept double clicks event
void VideoWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    setFullScreen(!isFullScreen());
    event->accept();
}
//allows the video player to accept mouse input
void VideoWidget::mousePressEvent(QMouseEvent *event) {
    QVideoWidget::mousePressEvent(event);
}

