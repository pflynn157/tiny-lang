#include <QApplication>
#include <QPixmap>

#include <menubar/file_menu.hpp>
#include <actions.hpp>

FileMenu::FileMenu() {
    this->setTitle("File");
    
    newFile = new QAction(QPixmap(":/icons/document-new.svg"), "New");
    openFile = new QAction(QPixmap(":/icons/document-open.svg"), "Open");
    saveFile = new QAction(QPixmap(":/icons/document-save.svg"), "Save");
    saveFileAs = new QAction(QPixmap(":/icons/document-save-as.svg"), "Save As");
    quit = new QAction(QPixmap(":/icons/window-close.svg"), "Quit");
    
    connect(newFile, &QAction::triggered, this, &FileMenu::onNewFileClicked);
    connect(openFile, &QAction::triggered, this, &FileMenu::onOpenFileClicked);
    connect(saveFile, &QAction::triggered, this, &FileMenu::onSaveFileClicked);
    connect(saveFileAs, &QAction::triggered, this, &FileMenu::onSaveFileAsClicked);
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);
    
    this->addAction(newFile);
    this->addAction(openFile);
    this->addSeparator();
    this->addAction(saveFile);
    this->addAction(saveFileAs);
    this->addSeparator();
    this->addAction(quit);
}

FileMenu::~FileMenu() {

}

void FileMenu::onNewFileClicked() {
    Actions::newFile();
}

void FileMenu::onOpenFileClicked() {
    Actions::openFile();
}

void FileMenu::onSaveFileClicked() {
    Actions::saveFile();
}

void FileMenu::onSaveFileAsClicked() {
    Actions::saveFileAs();
}
