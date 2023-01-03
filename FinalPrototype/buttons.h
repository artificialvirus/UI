#ifndef BUTTONS_H
#define BUTTONS_H


#include <QMediaPlayer>
#include <QWidget>

class QAbstractButton;
class QAbstractSlider;
class QComboBox;


class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    PlayerControls(QWidget *parent = 0);

    QMediaPlayer::State state() const;


public slots:
    void setState(QMediaPlayer::State state);


signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();


private slots:
    void playClicked();



private:
    QMediaPlayer::State playerState;

    QAbstractButton *playButton;
    QAbstractButton *stopButton;
    QAbstractButton *nextButton;
    QAbstractButton *previousButton;



};








#endif // BUTTONS_H
