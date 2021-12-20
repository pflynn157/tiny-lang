#pragma once

#include <QMenu>
#include <QAction>

class FileMenu : public QMenu {
    Q_OBJECT
public:
    FileMenu();
    ~FileMenu();
private:
    QAction *newFile, *openFile;
    QAction *saveFile, *saveFileAs;
    QAction *quit;
private slots:
    void onNewFileClicked();
    void onOpenFileClicked();
    void onSaveFileClicked();
    void onSaveFileAsClicked();
};
