#include <statusbar.hpp>

StatusBar::StatusBar() {
    openFile = new QLabel("untitled");
    saveStatus = new QLabel("saved");
    
    this->addPermanentWidget(openFile);
    this->addPermanentWidget(saveStatus);
}

StatusBar::~StatusBar() {
}