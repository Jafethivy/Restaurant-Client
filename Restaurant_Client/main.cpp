#include <QtWidgets/QApplication>
#include <QSurfaceFormat>
#include "AppContext.h"
#include "src/MainWindow.h"

int main(int argc, char *argv[]){
    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);

    AppContext context;
    auto* window = context.initialize();

    window->show();
    return app.exec();
}
