#pragma once

#include <QToolBar>
#include <QToolButton>

#include <editor.hpp>

class ToolBar : public QToolBar {
    Q_OBJECT
public:
    ToolBar(Editor *editor);
    ~ToolBar();
private:
    Editor *editor = nullptr;
    QToolButton *newFile, *openFile, *saveFile, *saveFileAs;
    QToolButton *cut, *copy, *paste;
    QToolButton *undo, *redo;
private slots:
    void onNewFileClicked();
    void onOpenFileClicked();
    void onSaveFileClicked();
    void onSaveFileAsClicked();
    void onCutClicked();
    void onCopyClicked();
    void onPasteClicked();
    void onUndoClicked();
    void onRedoClicked();
};
