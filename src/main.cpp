
#include "gui/window.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.setWindowTitle("Student Database");
    window.resize(800, 600);
    window.show();
    
    return app.exec();
}