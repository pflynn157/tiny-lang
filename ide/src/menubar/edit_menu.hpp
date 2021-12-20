#pragma once

#include <QMenu>
#include <QAction>

#include <editor.hpp>

class EditMenu : public QMenu {
    Q_OBJECT
public:
    EditMenu(Editor *editor);
    ~EditMenu();
private:
    Editor *editor;
    QAction *cut, *copy, *paste;
    QAction *undo, *redo;
private slots:
    void onCutClicked();
    void onCopyClicked();
    void onPasteClicked();
    void onUndoClicked();
    void onRedoClicked();
};
