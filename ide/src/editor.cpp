#include <editor.hpp>
#include <actions.hpp>

Editor::Editor() {
    highlight = new SyntaxHighlighter(this->document());
    highlight->setLanguage("orka");
    
    QFont font("Liberation Mono", 12);
    this->setFont(font);
    
    connect(this, &QPlainTextEdit::textChanged, this, &Editor::onModified);
}

Editor::~Editor() {}

void Editor::setText(QString text) {
    this->setPlainText(text);
}

QString Editor::getText() {
    return this->toPlainText();
}

void Editor::onModified() {
    Actions::statusbar->setSaved(false);
}
