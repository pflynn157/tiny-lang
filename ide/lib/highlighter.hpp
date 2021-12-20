#pragma once

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QStringLiteral>
#include <QMap>

struct SyntaxRule {
    QRegularExpression pattern;
    QString startExpr;
    QString endExpr;
    QTextCharFormat format;
    bool multiLine;
    bool lineComment;
    int statePos;
};

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit SyntaxHighlighter(QTextDocument *parent);
    void setTheme(QString name);
    void setLanguage(QString lang);
    void addSingleRule(QString category, QString expression, bool isComment = false);
    void addDoubleRule(QString category, QString startExpr, QString endExpr);
protected:
    void highlightBlock(const QString &text) override;
private:
    QString currentTheme, currentSyntax;
    QMap<QString, QTextCharFormat> formatMap;
    QVector<SyntaxRule> syntaxRules;
    int statePos = 3;
    bool hasMultiLine = false;
};

