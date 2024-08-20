#include "NCriticalPathWidget.h"
#include "Path.h"

#include <QApplication>

int main(int argc, char** argv) {
    QApplication app{argc, argv};

    Path::instance().copyResourceFileToRuntimePath(":/data/test_post_verilog_arch.xml");
    Path::instance().copyResourceFileToRuntimePath(":/data/unconnected.eblif");

    NCriticalPathWidget w;
    w.resize(800, 600);
    w.show();

    return app.exec();
}
