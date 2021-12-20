#include <QPixmap>

#include <toolbar.hpp>
#include <actions.hpp>

ToolBar::ToolBar(Editor *editor) {
    this->setContextMenuPolicy(Qt::PreventContextMenu);
    this->setMovable(false);
    
    this->editor = editor;
    
    newFile = new QToolButton;
    openFile = new QToolButton;
    saveFile = new QToolButton;
    saveFileAs = new QToolButton;
    cut = new QToolButton;
    copy = new QToolButton;
    paste = new QToolButton;
    undo = new QToolButton;
    redo = new QToolButton;
    
    newFile->setIcon(QPixmap(":/icons/document-new.svg"));
    openFile->setIcon(QPixmap(":/icons/document-open.svg"));
    saveFile->setIcon(QPixmap(":/icons/document-save.svg"));
    saveFileAs->setIcon(QPixmap(":/icons/document-save-as.svg"));
    cut->setIcon(QPixmap(":/icons/edit-cut.svg"));
    copy->setIcon(QPixmap(":/icons/edit-copy.svg"));
    paste->setIcon(QPixmap(":/icons/edit-paste.svg"));
    undo->setIcon(QPixmap(":/icons/edit-undo.svg"));
    redo->setIcon(QPixmap(":/icons/edit-redo.svg"));
    
    connect(newFile, &QToolButton::clicked, this, &ToolBar::onNewFileClicked);
    connect(openFile, &QToolButton::clicked, this, &ToolBar::onOpenFileClicked);
    connect(saveFile, &QToolButton::clicked, this, &ToolBar::onSaveFileClicked);
    connect(saveFileAs, &QToolButton::clicked, this, &ToolBar::onSaveFileAsClicked);
    connect(cut, &QToolButton::clicked, this, &ToolBar::onCutClicked);
    connect(copy, &QToolButton::clicked, this, &ToolBar::onCopyClicked);
    connect(paste, &QToolButton::clicked, this, &ToolBar::onPasteClicked);
    connect(undo, &QToolButton::clicked, this, &ToolBar::onUndoClicked);
    connect(redo, &QToolButton::clicked, this, &ToolBar::onRedoClicked);
    
    this->addWidget(newFile);
    this->addWidget(openFile);
    this->addWidget(saveFile);
    this->addWidget(saveFileAs);
    this->addSeparator();
    this->addWidget(cut);
    this->addWidget(copy);
    this->addWidget(paste);
    this->addWidget(undo);
    this->addWidget(redo);
}

ToolBar::~ToolBar() {
    delete newFile;
    delete openFile;
    delete saveFile;
    delete saveFileAs;
    delete cut;
    delete copy;
    delete paste;
    delete undo;
    delete redo;
}

void ToolBar::onNewFileClicked() {
    Actions::newFile();
}

void ToolBar::onOpenFileClicked() {
    Actions::openFile();
}

void ToolBar::onSaveFileClicked() {
    Actions::saveFile();
}

void ToolBar::onSaveFileAsClicked() {
    Actions::saveFileAs();
}

void ToolBar::onCutClicked() {
    editor->cut();
}

void ToolBar::onCopyClicked() {
    editor->copy();
}

void ToolBar::onPasteClicked() {
    editor->paste();
}

void ToolBar::onUndoClicked() {
    editor->undo();
}

void ToolBar::onRedoClicked() {
    editor->redo();
}
