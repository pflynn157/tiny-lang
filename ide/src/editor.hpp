#pragma once

#include <QPlainTextEdit>
#include <QString>

#include <highlighter.hpp>
#include <repository.hpp>

class Editor : public QPlainTextEdit {
    Q_OBJECT
public:
    Editor();
    ~Editor();
    void setText(QString text);
    QString getText();
    
    void setPath(QString path) { this->path = path; }
    QString getPath() { return path; }
    
    void setUntitled() { path = ""; }
    bool isUntitled() {
        if (path == "") return true;
        return false;
    }
private:
    QString path = "";
    SyntaxHighlighter *highlight;
private slots:
    void onModified();
};
