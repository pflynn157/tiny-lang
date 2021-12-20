#include <QApplication>
#include <QMessageBox>

#include <menubar/help_menu.hpp>

HelpMenu::HelpMenu() {
    this->setTitle("Help");
    
    about = new QAction("About");
    aboutQt = new QAction("About Qt");
    
    this->addAction(about);
    this->addAction(aboutQt);
    
    connect(about, &QAction::triggered, this, &HelpMenu::onAboutClicked);
    connect(aboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

HelpMenu::~HelpMenu() {
    delete about;
    delete aboutQt;
}

void HelpMenu::onAboutClicked() {
    QMessageBox msg;
    msg.setWindowTitle("About Orka IDE");
    msg.setText("Orka IDE\n"
                "A simple editor for the Orka programming language written in C++ using the Qt libraries.\n");
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}
