#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include "basicelement.h"
#include <pipeline.h>

using namespace std;

class PipeLine;
class RenderWindow;


class Controller
{
public:
    Controller();

public:
    void setPipeLine(PipeLine* pipeLine);
    void setRenderWindow(RenderWindow* renderWnd);
    void RenderTriangles(const GloabalRender& gRender);
    inline GloabalRender& getGlobalRender() { return m_pPipeLine->getGLobalRender(); }
    void RebuildScene();
    int  PickupTriIndex(QPoint pos);

private:
    PipeLine*      m_pPipeLine;
    RenderWindow*  m_pRenderWnd;
};

#endif // CONTROLLER_H
