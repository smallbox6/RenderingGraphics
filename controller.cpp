#include "controller.h"
#include "renderwindow.h"
#include "pipeline.h"
#include <QDebug>


Controller::Controller()
{
    m_pPipeLine = NULL;
    m_pRenderWnd = NULL;
}

void Controller::RebuildScene()
{
    m_pPipeLine->RebuildScene();
}

int  Controller::PickupTriIndex(QPoint pos)
{
    return m_pPipeLine->GeSelectedScreenTri(pos);
}

void Controller::setPipeLine(PipeLine* pipeLine)
{
    m_pPipeLine = pipeLine;
}

void Controller::setRenderWindow(RenderWindow* renderWnd)
{
    m_pRenderWnd = renderWnd;
}

void Controller::RenderTriangles(const GloabalRender& gRender)
{
    m_pRenderWnd->RenderTriangles(gRender);
}
