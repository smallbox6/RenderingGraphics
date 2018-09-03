#include <QApplication>
#include "renderwindow.h"
#include "controller.h"
#include "pipeline.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RenderWindow w;

    PipeLine pln;
    Controller  ctr;

    w.SetControler(&ctr);

    ctr.setPipeLine(&pln);
    ctr.setRenderWindow(&w);

    pln.SetControler(&ctr);
    pln.CreateModels();

    w.show();

    return a.exec();
}
