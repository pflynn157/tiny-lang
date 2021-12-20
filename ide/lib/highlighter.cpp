#include <QFile>
#include <QtXml>
#include <QMap>
#include <iostream>

#include "highlighter.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
    setTheme("default");
}

void SyntaxHighlighter::setTheme(QString name) {
    currentTheme = name;
    formatMap.clear();

    QFile file(":/data/theme/" + name + ".xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Unable to open theme." << std::endl;
        return;
    }
    
    // Parse
    QDomDocument doc("default");
    if (!doc.setContent(&file)) {
        std::cout << "Unable to read theme." << std::endl;
        file.close();
        return;
    }
    file.close();
    
    QDomElement element = doc.documentElement();
    QDomNode node = element.firstChild();
    
    while (!node.isNull()) {
        element = node.toElement();
        if (element.isNull()) {
            node = node.nextSibling();
            continue;
        }
        
        if (element.tagName() == "name") {
            node = node.nextSibling();
            continue;
        } else if (element.tagName() != "category") {
            std::cout << "Unknown tag name: " << element.tagName().toStdString() << std::endl;
            node = node.nextSibling();
            continue;
        }
        
        QString name = element.attribute("id", "");
        if (name.length() == 0) {
            std::cout << "Error: Expected ID tag" << std::endl;
            node = node.nextSibling();
            continue;
        }
        
        QTextCharFormat format;
        QDomNode syntaxNode = element.firstChild();
        
        while (!syntaxNode.isNull()) {
            QDomElement syntaxElement = syntaxNode.toElement();
            if (syntaxElement.isNull()) {
                syntaxNode = syntaxNode.nextSibling();
                continue;
            }
            
            if (syntaxElement.tagName() == "fg") {
                format.setForeground(QColor(syntaxElement.text()));
            } else if (syntaxElement.tagName() == "bg") {
                //TODO
            } else if (syntaxElement.tagName() == "b") {
                if (syntaxElement.text() == "true") {
                    format.setFontWeight(QFont::Bold);
                }
            } else if (syntaxElement.tagName() == "i") {
                //TODO
            }
            
            syntaxNode = syntaxNode.nextSibling();
        }
        
        formatMap[name] = format;
        node = node.nextSibling();
    }
    
    this->rehighlight();
    if (currentSyntax != "") {
        setLanguage(currentSyntax);
    }
}

void SyntaxHighlighter::setLanguage(QString lang) {
    currentSyntax = lang;
    syntaxRules.clear();
    
    QFile file(":/data/syntax/" + lang + ".xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Unable to open syntax." << std::endl;
        return;
    }
    
    // Parse
    QDomDocument doc("syntax");
    if (!doc.setContent(&file)) {
        std::cout << "Unable to read syntax." << std::endl;
        file.close();
        return;
    }
    file.close();
    
    QDomElement element = doc.documentElement();
    QDomNode node = element.firstChild();
    
    // Needed for classes
    QVector<QString> allClasses;
    QMap<QString, QString> classStyles;
    QMap<QString, QString> classExpr;
    QMap<QString, QStringList> classData;   
    
    // Read the document
    while (!node.isNull()) {
        element = node.toElement();
        if (element.isNull()) {
            node = node.nextSibling();
            continue;
        }
        
        if (element.tagName() == "name") {
            // blah
        // Read the format section
        } else if (element.tagName() == "format") {
            QDomNode formatNode = element.firstChild();
            
            while (!formatNode.isNull()) {
                QDomElement formatElement = formatNode.toElement();
                if (formatElement.isNull()) {
                    formatNode = formatNode.nextSibling();
                    continue;
                }
                
                // The Regex specifies a direct expression; this is the easiest to handle
                if (formatElement.tagName() == "regex") {
                    QString style = formatElement.attribute("style");
                    QString regex = formatElement.text();
                    addSingleRule(style, regex);
                    
                // Line comments
                } else if (formatElement.tagName() == "comment") {
                    QString style = formatElement.attribute("style");
                    QString regex = formatElement.text();
                    addSingleRule(style, regex, true);
                    
                // The mutliline specifies an expression spanning multiple lines
                } else if (formatElement.tagName() == "multiline") {
                    hasMultiLine = true;
                    QString style = formatElement.attribute("style");
                    QString startExpr = "";
                    QString endExpr = "";
                    
                    QDomNode mlnode = formatElement.firstChild();
                    while (!mlnode.isNull()) {
                        QDomElement mlelement = mlnode.toElement();
                        if (mlelement.isNull()) {
                            mlnode = mlnode.nextSibling();
                            continue;
                        }
                        
                        if (mlelement.tagName() == "start") startExpr = mlelement.text();
                        else if (mlelement.tagName() == "end") endExpr = mlelement.text();
                        
                        mlnode = mlnode.nextSibling();
                    }
                    
                    if (startExpr == "" || endExpr == "") {
                        std::cout << "Error: Invalid multiline specification." << std::endl;
                    } else {
                        addDoubleRule(style, startExpr, endExpr);
                    }
                    
                // The class specifies a large group that follow the same expression and style
                } else if (formatElement.tagName() == "class") {
                    QString className = formatElement.attribute("id");
                    classStyles[className] = formatElement.attribute("style");
                    classExpr[className] = formatElement.text();
                    allClasses.append(className);
                }
                
                formatNode = formatNode.nextSibling();
            }
          
        // Read the data section
        } else if (element.tagName() == "data") {
            QDomNode dataNode = element.firstChild();
            
            while (!dataNode.isNull()) {
                QDomElement dataElement = dataNode.toElement();
                if (dataElement.isNull()) {
                    dataNode = dataNode.nextSibling();
                    continue;
                }
                
                if (dataElement.tagName() != "class") {
                    std::cout << "Error: Unknown data element." << std::endl;
                    dataNode = dataNode.nextSibling();
                    continue;
                }
                
                QString id = dataElement.attribute("id");
                QStringList items;
                
                // Read all items from the class
                QDomNode itemNode = dataElement.firstChild();
                
                while (!itemNode.isNull()) {
                    QDomElement itemElement = itemNode.toElement();
                    if (itemElement.isNull()) {
                        itemNode = itemNode.nextSibling();
                        continue;
                    }
                    
                    if (itemElement.tagName() == "item") {
                        items.push_back(itemElement.text());
                    } else {
                        std::cout << "Error: Unknown class list item." << std::endl;
                    }
                    
                    itemNode = itemNode.nextSibling();
                }
                
                // Add the vector to the map
                classData[id] = items;
                
                dataNode = dataNode.nextSibling();
            }
        
        // Unknown nodes
        } else {
            std::cout << "Error: Unknown syntax element." << std::endl;
        }
        
        node = node.nextSibling();
    }
    
    // Add all the class elements
    for (QString className : allClasses) {
        QString style = classStyles[className];
        QString expr = classExpr[className];
        QStringList items = classData[className];
        
        for (QString item : items) {
            QString newExpr = expr;
            newExpr.replace("$", item);
            addSingleRule(style, newExpr);
        }
    }
    
    this->rehighlight();
}

void SyntaxHighlighter::addSingleRule(QString category, QString expression, bool isComment) {
    SyntaxRule rule;
    rule.multiLine = false;
    rule.lineComment = isComment;
    rule.format = formatMap[category];
    rule.pattern = QRegularExpression(expression);
    rule.startExpr = expression;
    syntaxRules.append(rule);
}

void SyntaxHighlighter::addDoubleRule(QString category, QString startExpr, QString endExpr) {
    SyntaxRule rule;
    rule.multiLine = true;
    rule.lineComment = false;
    rule.format = formatMap[category];
    rule.startExpr = startExpr;
    rule.endExpr = endExpr;
    rule.statePos = statePos;
    syntaxRules.append(rule);
    
    ++statePos;
}

void SyntaxHighlighter::highlightBlock(const QString &text) {
    // Handle single rules
    for (SyntaxRule rule : syntaxRules) {
        if (rule.multiLine) continue;
        if (rule.lineComment) continue;
        QRegularExpressionMatchIterator iterator = rule.pattern.globalMatch(text);
        while (iterator.hasNext()) {
            QRegularExpressionMatch match = iterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    
    // Handle single-line comments
    for (SyntaxRule rule : syntaxRules) {
        if (!rule.lineComment) continue;
        int start = 0;
        int state = previousBlockState();
        
        for (int i = 0; i<text.length(); i++) {
            if (text.mid(i, rule.startExpr.length()) == rule.startExpr) {
                start = i;
                state = 2;
            } else if (text[i] == '\n') {
                state = -1;
                setFormat(start, i - start + rule.endExpr.length(), rule.format);
            }
        }
        
        if (state > 0)
            setFormat(start, text.length() - start, rule.format);
     
        setCurrentBlockState(state);
    }
    
    // Handle double-line comments
    if (!hasMultiLine) return;
    int state = previousBlockState();
    int start = 0;
    
    for (SyntaxRule rule : syntaxRules) {
        if (!rule.multiLine) continue;
        
        for (int i = 0; i<text.length(); i++) {
            if (state == rule.statePos) {
                if (text.mid(i, rule.endExpr.length()) == rule.endExpr) {
                    state = -1;
                    setFormat(start, i - start + rule.endExpr.length(), rule.format);
                }
            } else {
                if (text.mid(i, rule.startExpr.length()) == rule.startExpr) {
                    start = i;
                    state = rule.statePos;
                }
            }
        }
        
        if (state > 0)
            setFormat(start, text.length() - start, rule.format);
     
        setCurrentBlockState(state);
    }
}

