#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QCloseEvent>

#include <editor.hpp>
#include <toolbar.hpp>
#include <statusbar.hpp>
#include <menubar/file_menu.hpp>
#include <menubar/edit_menu.hpp>
#include <menubar/help_menu.hpp>

class Window : public QMainWindow {
    Q_OBJECT
public:
    Window();
    ~Window();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Editor *editor;
    ToolBar *toolbar;
    QMenuBar *menubar;
    StatusBar *statusbar;
    
    FileMenu *fileMenu;
    EditMenu *editMenu;
    HelpMenu *helpMenu;
};
