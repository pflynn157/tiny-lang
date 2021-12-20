#include <QApplication>

#include <window.hpp>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    
    Window *win = new Window;
    win->show();
    
    return app.exec();
}
