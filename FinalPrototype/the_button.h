//
// Created by twak on 11/11/2019.
//

#ifndef CW2_THE_BUTTON_H
#define CW2_THE_BUTTON_H


#include <QPushButton>
#include <QUrl>
#include <QStyle>
#include <QMediaPlayer>
#include <QAbstractButton>
#include <QToolButton>


class TheButtonInfo {

public:
    QUrl* url; // video file to play
    QIcon* icon; // icon to display
    TheButtonInfo ( QUrl* url, QIcon* icon) : url (url), icon (icon) {}

};


class TheButton : public QPushButton {
    Q_OBJECT

public:


    TheButtonInfo* info;
    QMediaPlayer::State state() const;

    TheButton(QWidget *parent) :  QPushButton(parent) {

        setIconSize(QSize(150,75));
        connect(this, SIGNAL(released()), this, SLOT (clicked() )); // if QPushButton clicked...then run clicked() below

    }

    void init(TheButtonInfo* i);


private slots:
    void clicked();


signals:
    void jumpTo(TheButtonInfo*);
    void addPlaylist(TheButtonInfo*);  // adds the videos to the playlist


};

#endif //CW2_THE_BUTTON_H
