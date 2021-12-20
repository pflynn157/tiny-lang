#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtXml>
#include <iostream>

#include "repository.hpp"

// Returns a list of valid syntaxes
QStringList Repository::syntaxList() {
    QStringList dirEntries = getEntries(":/data/syntax");
    QStringList names;
    
    for (QString item : dirEntries) {
        QString name = getName(":/data/syntax/" + item);
        names.append(name);
    }

    return names;
}

// Returns the definition file for a given syntax name
QString Repository::defForName(QString name) {
    QStringList dirEntries = getEntries(":/data/syntax");
    QStringList names;
    
    for (QString item : dirEntries) {
        QString currentName = getName(":/data/syntax/" + item);
        if (currentName == name) {
            return QFileInfo(item).baseName();
        }
    }
    
    return "";
}

// Returns a list of valid syntax themes
QStringList Repository::themeList() {
    QStringList dirEntries = getEntries(":/data/theme");
    QStringList names;
    
    for (QString item : dirEntries) {
        QString name = getName(":/data/theme/" + item);
        names.append(name);
    }

    return names;
}

// Returns the theme file for a given theme name
QString Repository::themeForName(QString name) {
    QStringList dirEntries = getEntries(":/data/theme");
    QStringList names;
    
    for (QString item : dirEntries) {
        QString currentName = getName(":/data/theme/" + item);
        if (currentName == name) {
            return QFileInfo(item).baseName();
        }
    }
    
    return "";
}

// Returns a list of given entries
QStringList Repository::getEntries(QString path) {
    QDir dir(path);
    return dir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
}

// Parses the XML file and returns the name for the syntax
QString Repository::getName(QString path) {
    QString name = "(unknown)";
    
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        std::cout << "Unable to open file: " << path.toStdString() << std::endl;
        return name;
    }
    
    QDomDocument doc("doc");
    if (!doc.setContent(&file)) {
        std::cout << "Unable to read file: " << path.toStdString() << std::endl;
        return name;
    }
    
    QDomElement element = doc.documentElement();
    QDomNode node = element.firstChild();
    
    while (!node.isNull()) {
        element = node.toElement();
        if (element.isNull()) {
            node = node.nextSibling();
            continue;
        }
        
        if (element.tagName() == "name") {
            name = element.text();
            break;
        }
        
        node = node.nextSibling();
    }
    
    return name;
}

