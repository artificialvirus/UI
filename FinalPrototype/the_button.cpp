//
// Created by twak on 11/11/2019.
//

#include "the_button.h"

#include <QBoxLayout>
#include <QSlider>
#include <QStyle>
#include <QToolButton>
#include <QComboBox>

class QAbstractButton;
class QComboBox;


void TheButton::init(TheButtonInfo* i) {
    setIcon( *(i->icon) );
    info =  i;
}

void TheButton::clicked() {
    emit jumpTo(info);
    emit addPlaylist(info);//adds the buttons to the playlist

}

