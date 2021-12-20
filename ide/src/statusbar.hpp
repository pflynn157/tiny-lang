#pragma once

#include <QStatusBar>
#include <QLabel>

class StatusBar : public QStatusBar {
    Q_OBJECT
public:
    StatusBar();
    ~StatusBar();
    
    void setCurrentFile(QString path) {
        openFile->setText(path);
    }
    
    void setSaved(bool isSaved) {
        if (isSaved) saveStatus->setText("saved");
        else saveStatus->setText("unsaved");
    }
    
    bool isSaved() {
        if (saveStatus->text() == "saved") return true;
        return false;
    }
private:
    QLabel *openFile, *saveStatus;
};
