#include <QPixmap>

#include <menubar/edit_menu.hpp>

EditMenu::EditMenu(Editor *editor) {
    this->editor = editor;
    this->setTitle("Edit");
    
    cut = new QAction(QPixmap(":/icons/edit-cut.svg"), "Cut");
    copy = new QAction(QPixmap(":/icons/edit-copy.svg"), "Copy");
    paste = new QAction(QPixmap(":/icons/edit-paste.svg"), "Paste");
    undo = new QAction(QPixmap(":/icons/edit-undo.svg"), "Undo");
    redo = new QAction(QPixmap(":/icons/edit-redo.svg"), "Redo");
    
    connect(cut, &QAction::triggered, this, &EditMenu::onCutClicked);
    connect(copy, &QAction::triggered, this, &EditMenu::onCopyClicked);
    connect(paste, &QAction::triggered, this, &EditMenu::onPasteClicked);
    connect(undo, &QAction::triggered, this, &EditMenu::onUndoClicked);
    connect(redo, &QAction::triggered, this, &EditMenu::onRedoClicked);
    
    this->addAction(cut);
    this->addAction(copy);
    this->addAction(paste);
    this->addSeparator();
    this->addAction(undo);
    this->addAction(redo);
}

EditMenu::~EditMenu() {
    delete cut;
    delete copy;
    delete paste;
    delete undo;
    delete redo;
}

void EditMenu::onCutClicked() {
    editor->cut();
}

void EditMenu::onCopyClicked() {
    editor->copy();
}

void EditMenu::onPasteClicked() {
    editor->paste();
}

void EditMenu::onUndoClicked() {
    editor->undo();
}

void EditMenu::onRedoClicked() {
    editor->redo();
}
