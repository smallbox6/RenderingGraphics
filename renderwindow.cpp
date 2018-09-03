#include  <QPainter>
#include  <QPen>
#include  <QFont>
#include  <QFontMetrics>
#include <QDebug>
#include "renderwindow.h"
#include "controller.h"

bool IsEqual(float a, float b)
{
    if(fabs(a - b) == 0.0f)
        return true;
    else
        return false;
}

RenderWindow::RenderWindow(QWidget *parent) : QMainWindow(parent)
{
    setMouseTracking(true);
    m_pControler = NULL;
    m_bShowPlanes = true;
    m_bShowLines = false;
    m_selectedTriIdx = -1;
    setFixedSize(800, 600);

    m_picDepth = new vector<MyDepth>* [this->width()];
    for(int i = 0; i < this->width(); i++)
        m_picDepth[i] = new vector<MyDepth>[ this->height() ];

    InitDepthCache();

    m_FrontPixmap = new QPixmap(this->width(), this->height());
    m_FrontPixmap->fill(Qt::black);
    m_BackPixmap = new QPixmap(this->width(), this->height());
}

RenderWindow::~RenderWindow()
{
    for(int i = 0; i < this->width(); i++)
        delete []m_picDepth[i];
    delete []m_picDepth;
}

void RenderWindow::keyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
    case Qt::Key_Q:
        m_bShowLines = !m_bShowLines;
        m_pControler->RebuildScene();
        break;
    case Qt::Key_A:
        m_bShowPlanes = !m_bShowPlanes;
        m_pControler->RebuildScene();
        break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

void RenderWindow::mousePressEvent(QMouseEvent *event)
{
    if(Qt ::LeftButton == event->button())
    {
        //m_selectedTriIdx = m_pControler->PickupTriIndex(m_mouseSreenPos);
        PickupTriangle(m_mouseSreenPos);
        m_pControler->RebuildScene();
    }
}

void RenderWindow::mouseMoveEvent(QMouseEvent *event)
{
    m_mouseSreenPos = QPoint(event->pos().x(), this->height() - event->pos().y());
    update();
}

void RenderWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPixmap(0, 0, *m_FrontPixmap);
    RenderText(painter, true);
}

void RenderWindow::InitDepthCache()
{
    for(int i = 0; i < this->width(); i++)
        for(int j = 0; j < this->height(); j++)
            m_picDepth[i][j].clear();
}

void RenderWindow::PickupTriangle(QPoint pos)
{
    if(m_picDepth[pos.x()][pos.y()].size() == 0)
        m_selectedTriIdx = -1;
    else
        m_selectedTriIdx = m_picDepth[pos.x()][pos.y()][0].triIdx;
}

bool RenderWindow::pushZtoQueue(int w, int h, MyDepth mydp)
{
    if(m_picDepth[w][h].size() == 0)
    {
        m_picDepth[w][h].push_back(mydp);
        return true;
    }
    else
    {
        if(m_picDepth[w][h].front().z > mydp.z )
        {
            m_picDepth[w][h].insert(m_picDepth[w][h].begin(), mydp);
            return true;
        }
        for(int i = 1; i < m_picDepth[w][h].size(); i++)
            if(m_picDepth[w][h][i].z < mydp.z)
            {
                 m_picDepth[w][h].insert(m_picDepth[w][h].begin() + i, mydp);
                 return false;
            }
        m_picDepth[w][h].push_back(mydp);
        return false;
    }
}

void RenderWindow::RenderTriangles(const GloabalRender& gRender)
{
    QPainter painter(m_BackPixmap);
    MyTriangle myTri;

    InitDepthCache();

    painter.drawImage(0, 0, QImage(":/Images/starInSky.jpg"));

    for(int i = 0; i < gRender.triangleList.size(); i++)
    {
        myTri.p1.x = gRender.pointList[gRender.triangleList[i].v1].pos(0);
        myTri.p1.y = gRender.pointList[gRender.triangleList[i].v1].pos(1);
        myTri.p1.z = gRender.pointList[gRender.triangleList[i].v1].pos(2);
        myTri.p1.w = gRender.pointList[gRender.triangleList[i].v1].pos(3);
        myTri.p1.texture = gRender.triangleList[i].texV1;
        myTri.p1.color   = gRender.pointList[gRender.triangleList[i].v1].color;

        myTri.p2.x = gRender.pointList[gRender.triangleList[i].v2].pos(0);
        myTri.p2.y = gRender.pointList[gRender.triangleList[i].v2].pos(1);
        myTri.p2.z = gRender.pointList[gRender.triangleList[i].v2].pos(2);
        myTri.p2.w = gRender.pointList[gRender.triangleList[i].v2].pos(3);
        myTri.p2.texture = gRender.triangleList[i].texV2;
        myTri.p2.color   = gRender.pointList[gRender.triangleList[i].v2].color;

        myTri.p3.x = gRender.pointList[gRender.triangleList[i].v3].pos(0);
        myTri.p3.y = gRender.pointList[gRender.triangleList[i].v3].pos(1);
        myTri.p3.z = gRender.pointList[gRender.triangleList[i].v3].pos(2);
        myTri.p3.w = gRender.pointList[gRender.triangleList[i].v3].pos(3);
        myTri.p3.texture = gRender.triangleList[i].texV3;
        myTri.p3.color   = gRender.pointList[gRender.triangleList[i].v3].color;

        if(m_bShowPlanes)
        {
            if(gRender.textureList.size())
            {
                DrawTriangleFace(myTri, painter, m_Pen, gRender.textureList[gRender.triangleList[i].textureIndex].image, i);
            }
            else
                DrawTriangleFace(myTri, painter, m_Pen, QImage(), i);
        }

        if(m_bShowLines)
            DrawTriangleLines(myTri, painter, m_Pen);

        if(m_selectedTriIdx == i && (m_bShowLines || m_bShowPlanes))
        {
            m_currentModelName = *gRender.triangleList[i].modelName;
            RenderHightSelRect(myTri, painter);
        }

        /*if(gRender.textureList.size())
            SplitStriangle(myTri, painter, m_Pen, gRender.textureList[gRender.triangleList[0].textureIndex].image);
        else
            SplitStriangle(myTri, painter, m_Pen, QImage());*/
   }

    RenderText(painter, false);
    m_FrontPixmap =  m_BackPixmap;
    update();
}

void RenderWindow::RenderText(QPainter& painter, bool bfront)
{
    QString tpStr;
    QFont font("微软雅黑", 12);
    m_Pen.setColor(QColor(255, 0, 0));
    painter.setFont(font);
    painter.setPen(m_Pen);

    QFontMetrics metrics = painter.fontMetrics();
    int textHeight = metrics.height();
    painter.drawText(550, 30, QString("press Q to show lines") );
    painter.drawText(550, textHeight + 30, QString("press A to show model") );
    painter.drawText(550, 2 * textHeight + 30, QString("click leftmouse to pick up plane") );

    if(bfront)
        tpStr = QString("screen position: ( %1, %2 )").arg(m_mouseSreenPos.x()).arg(m_mouseSreenPos.y());
    else
        tpStr = QString("screen position:");
    painter.drawText(550, 3 * textHeight + 30, tpStr );

    if(m_selectedTriIdx >= 0 && (m_bShowLines || m_bShowPlanes))
        painter.drawText(550, 4 * textHeight + 30, QString("current selected model: " + m_currentModelName) );
}

void RenderWindow::RenderHightSelRect(MyTriangle& tri, QPainter& painter)
{
    QBrush brush(QColor(255, 0, 0, 80));
    painter.setBrush(brush);
    QPoint p[3] = {
        QPoint(tri.p1.x, this->height() - tri.p1.y),
        QPoint(tri.p2.x, this->height() - tri.p2.y),
        QPoint(tri.p3.x, this->height() - tri.p3.y)
    };
    painter.drawPolygon(p, 3);
}

void RenderWindow::DrawTriangleLines(MyTriangle& tri, QPainter& painter, QPen& pen)
{
    QPoint p1, p2, p3;

    p1.setX((int) tri.p1.x);
    p1.setY((int) (this->height() - tri.p1.y));
    p2.setX((int) tri.p2.x);
    p2.setY((int) (this->height() - tri.p2.y));
    p3.setX((int) tri.p3.x);
    p3.setY((int) (this->height() - tri.p3.y));

    m_Pen.setColor(QColor(229, 166, 21));
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(pen);
    painter.drawLine(p1, p2);
    painter.drawLine(p1, p3);
    painter.drawLine(p2, p3);
}

void RenderWindow::SplitStriangle(MyTriangle& tri, QPainter& temppainter, QPen& pen, const QImage& image)
{
    float ratio;
    int ymin, ymax;
    MyTriangle t;
    MyPoint p1, p2, p3, mid;
    bool tp1_tp2 = fabs(tri.p1.y - tri.p2.y) < 1e-6;
    bool tp1_tp3 = fabs(tri.p1.y - tri.p3.y) < 1e-6;
    bool tp2_tp3 = fabs(tri.p2.y - tri.p3.y) < 1e-6;

    if(!(tp1_tp2 || tp1_tp3 || tp2_tp3))
    {
        mid = p1;
        p1 = tri.p1.y > tri.p2.y ? (ymax = 1, tri.p1) : (ymax = 2, tri.p2);
        p1 = p1.y > tri.p3.y ? p1 : (ymax = 3, tri.p3);

        p3 = tri.p1.y < tri.p2.y ? (ymin = 1, tri.p1) : (ymin = 2, tri.p2);
        p3 = p3.y < tri.p3.y ? p3 : (ymin = 3, tri.p3);

        if((ymax + ymin) == 3) p2 = tri.p3;
        else if((ymax + ymin) == 4) p2 = tri.p2;
        else p2 = tri.p1;

        float delta_y;
        if(fabs(p1.x - p3.x) < 1e-6)
        {
            mid.x = p1.x;
            mid.y = p2.y;
        }
        else
        {
            ratio = (p1.y - p3.y) / (p1.x - p3.x);
            mid.y = p2.y;
            mid.x = (mid.y - p3.y) / ratio + p3.x;
        }

        delta_y = p1.y - mid.y;
        mid.w = p3.w -(p3.w  - p1.w) / (p3.y - p1.y) * delta_y;
        mid.z = (p3.z * p3.w -(p3.z * p3.w - p1.z * p1.w) / (p3.y - p1.y) * delta_y) / mid.w;
        mid.color.r = (p3.color.r * p3.w -(p3.color.r * p3.w - p1.color.r * p1.w) / (p3.y - p1.y) * delta_y) / mid.w;
        mid.color.g = (p3.color.g * p3.w -(p3.color.g * p3.w - p1.color.g * p1.w) / (p3.y - p1.y) * delta_y) / mid.w;
        mid.color.b = (p3.color.b * p3.w -(p3.color.b * p3.w - p1.color.b * p1.w) / (p3.y - p1.y) * delta_y) / mid.w;
        mid.color.a = (p3.color.a * p3.w -(p3.color.a * p3.w - p1.color.a * p1.w) / (p3.y - p1.y) * delta_y) / mid.w;
        mid.texture.u = (p3.texture.u * p3.w -(p3.texture.u * p3.w - p1.texture.u * p1.w) / (p3.y - p1.y) * delta_y) / mid.w;
        mid.texture.v = (p3.texture.v * p3.w -(p3.texture.v * p3.w - p1.texture.v * p1.w) / (p3.y - p1.y) * delta_y) / mid.w;
    }
}

void RenderWindow::DrawTopTriangle(MyTriangle& tri, QPainter& temppainter, QPen& pen, const QImage& image,
                                   bool tp1_tp2, bool tp1_tp3, bool tp2_tp3, int idx)
{
    MyPoint p1, p2, p3;
    //扫描线变化量，水平扫描线起始和结束
    float kleft, kright, Xbegin, Xend;
    /////////////////////////////////////
    float rLeftStep,  gLeftStep,  bLeftStep,  rLeftBegin, gLeftBegin, bLeftBegin;
    float rRightStep, gRightStep, bRightStep, rRightEnd,  gRightEnd,  bRightEnd;
    float r_x, g_x, b_x, r_x_step, g_x_step, b_x_step;
    /////////////////////////////////////
    QRgb  rgb;
    float uLeftStep,  vLeftStep,  uLeftBegin, vLeftBegin;
    float uRightStep, vRightStep, uRightEnd,  vRightEnd;
    float u_x, v_x, u_x_step, v_x_step;
    float utemp, vtemp;
    ///////////////1/Z插值////////////////
    float zLeftStep,  zLeftBegin;
    float zRightStep, zRightEnd;
    float z_x, z_x_step;
    float rtemp, gtemp, btemp;

    if(tp2_tp3)
    {
        p2 = tri.p1;
        p1 = tri.p2.x < tri.p3.x ? (p3 = tri.p3, tri.p2) : (p3 = tri.p2, tri.p3);
    }
    else if(tp1_tp3)
    {
        p2 = tri.p2;
        p1 = tri.p1.x < tri.p3.x ? (p3 = tri.p3, tri.p1) : (p3 = tri.p1, tri.p3);
    }
    else if(tp1_tp2)
    {
        p2 = tri.p3;
        p1 = tri.p1.x < tri.p2.x ? (p3 = tri.p2, tri.p1) : (p3 = tri.p1, tri.p2);
    }

    /////////////////////设置坐标///////////////
    kleft  = (p2.x - p1.x) / (p2.y - p1.y);
    kright = (p2.x - p3.x) / (p2.y - p3.y);
    Xbegin = p1.x;
    Xend   = p3.x;
    //////////////////////设置Z值//////////////
    zLeftBegin = p1.w;
    zRightEnd  = p3.w;
    zLeftStep  = (p2.w - p1.w) / (p2.y - p1.y);
    zRightStep = (p2.w - p3.w) / (p2.y - p1.y);

    //////////////////////设置颜色///////////////////////////////
    rLeftStep = (p2.color.r * p2.w - p1.color.r * p1.w) / (p2.y - p1.y);
    gLeftStep = (p2.color.g * p2.w - p1.color.g * p1.w) / (p2.y - p1.y);
    bLeftStep = (p2.color.b * p2.w - p1.color.b * p1.w) / (p2.y - p1.y);

    rLeftBegin = p1.color.r * p1.w;
    gLeftBegin = p1.color.g * p1.w;
    bLeftBegin = p1.color.b * p1.w;

    rRightStep = (p2.color.r * p2.w - p3.color.r * p3.w) / (p2.y - p3.y);
    gRightStep = (p2.color.g * p2.w - p3.color.g * p3.w) / (p2.y - p3.y);
    bRightStep = (p2.color.b * p2.w - p3.color.b * p3.w) / (p2.y - p3.y);

    rRightEnd = p3.color.r * p3.w;
    gRightEnd = p3.color.g * p3.w;
    bRightEnd = p3.color.b * p3.w;

    //////////////////////////设置纹理/////////////////////////////
    uLeftStep  = (p2.texture.u * p2.w - p1.texture.u * p1.w) / (p2.y - p1.y);
    vLeftStep  = (p2.texture.v * p2.w - p1.texture.v * p1.w) / (p2.y - p1.y);
    uLeftBegin = p1.texture.u * p1.w;
    vLeftBegin = p1.texture.v * p1.w;
    uRightStep = (p2.texture.u * p2.w - p3.texture.u * p3.w) / (p2.y - p3.y);
    vRightStep = (p2.texture.v * p2.w - p3.texture.v * p3.w) / (p2.y - p3.y);
    uRightEnd  = p3.texture.u * p3.w;
    vRightEnd  = p3.texture.v * p3.w;

    for(float y = p1.y; y >= p2.y; y--)
    {
        if(y > 600) goto NEXT_Y1;
        if(y < 0) break;

        ////////////////////////颜色///////////////////////////
        r_x = rLeftBegin;
        g_x = gLeftBegin;
        b_x = bLeftBegin;
        r_x_step = (rRightEnd - rLeftBegin) / (Xend - Xbegin);
        g_x_step = (gRightEnd - gLeftBegin) / (Xend - Xbegin);
        b_x_step = (bRightEnd - bLeftBegin) / (Xend - Xbegin);

        ////////////////////////纹理//////////////////////////
        u_x = uLeftBegin;
        v_x = vLeftBegin;
        u_x_step = (uRightEnd - uLeftBegin) / (Xend - Xbegin);
        v_x_step = (vRightEnd - vLeftBegin) / (Xend - Xbegin);

        /////////////////////////Z插值/////////////////////////
        z_x = zLeftBegin;
        z_x_step = (zRightEnd - zLeftBegin) / (Xend - Xbegin);

        for(float x = Xbegin; x <= Xend; x++)
        {
            if(x > 800) break;
            if(x < 0)
            {
                //////////////颜色//////////////
                r_x += r_x_step;
                g_x += g_x_step;
                b_x += b_x_step;

                /////////////纹理///////////////
                u_x += u_x_step;
                v_x += v_x_step;

                //////////////1/Z///////////
                z_x += z_x_step;
                continue;
            }

            utemp = u_x / z_x ;
            vtemp = v_x / z_x ;

            rtemp = r_x / z_x;
            gtemp = g_x / z_x;
            btemp = b_x / z_x;

            rgb   = image.pixel( QPoint( int (utemp * image.width() + 0.5) % image.width(), int(image.height() * (1 - vtemp ) +0.5) %  image.height()) );
            rtemp = qRed(rgb) * rtemp;
            gtemp = qGreen(rgb) * gtemp;
            btemp = qBlue(rgb) * btemp;

            int picW = int(x + 0.5) % 800;
            int picH = int(y + 0.5) % 600;
            if( pushZtoQueue( picW, picH, {idx, 1 / z_x} ) )
            {
                rgb = qRgb(rtemp, gtemp, btemp);
                //pen.setColor(QColor(int(r_x + 0.5), int(g_x + 0.5), int(b_x + 0.5)));
                pen.setColor(rgb);
                temppainter.setPen(pen);
                temppainter.drawPoint(int(x + 0.5), int(600 - y + 0.5));
            }

            //////////////颜色//////////////
            r_x += r_x_step;
            g_x += g_x_step;
            b_x += b_x_step;

            /////////////纹理///////////////
            u_x += u_x_step;
            v_x += v_x_step;

            /////////////Z值///////////////
            z_x += z_x_step;
        }

        ///////////////////坐标///////////////////
NEXT_Y1: Xbegin -= kleft;
        Xend -= kright;

        /////////////////颜色/////////////////////
        rLeftBegin -= rLeftStep;
        gLeftBegin -= gLeftStep;
        bLeftBegin -= bLeftStep;
        rRightEnd  -= rRightStep;
        gRightEnd  -= gRightStep;
        bRightEnd  -= bRightStep;

        //////////////////纹理//////////////////
        uLeftBegin  -= uLeftStep;
        vLeftBegin  -= vLeftStep;
        uRightEnd   -= uRightStep;
        vRightEnd   -= vRightStep;

        ///////////////////Z值///////////////////
        zLeftBegin -= zLeftStep;
        zRightEnd  -= zRightStep;
        /////////////////////////////////////////
    }
}

void RenderWindow::DrawBottomTriangle(MyTriangle& tri, QPainter& temppainter, QPen& pen, const QImage& image,
                        bool tp1_tp2, bool tp1_tp3, bool tp2_tp3, int idx)
{
    MyPoint p1, p2, p3;
    /////////////////////////////////////
    float kleft, kright, Xbegin, Xend; //扫描线变化量，水平扫描线起始和结束
    /////////////////////////////////////
    float rLeftStep,  gLeftStep,  bLeftStep,  rLeftBegin, gLeftBegin, bLeftBegin;
    float rRightStep, gRightStep, bRightStep, rRightEnd,   gRightEnd,   bRightEnd;
    float r_x, g_x, b_x, r_x_step, g_x_step, b_x_step;
    /////////////////////////////////////
    QRgb  rgb;
    float uLeftStep, vLeftStep, uLeftBegin, vLeftBegin;
    float uRightStep, vRightStep, uRightEnd, vRightEnd;
    float u_x, v_x, u_x_step, v_x_step;
    float utemp, vtemp;
    //////////////////////Z倒数插值////////
    float zLeftStep,  zLeftBegin;
    float zRightStep, zRightEnd;
    float z_x, z_x_step;
    float rtemp, gtemp, btemp;

    if(tp1_tp2)
    {
        p1 = tri.p3;
        p2 = tri.p1.x < tri.p2.x ? (p3 = tri.p2, tri.p1): (p3 = tri.p1, tri.p2);
    }
    else if(tp1_tp3)
    {
        p1 = tri.p2;
        p2 = tri.p1.x < tri.p3.x ? (p3 = tri.p3, tri.p1) : (p3 = tri.p1, tri.p3);
    }
    else if(tp2_tp3)
    {
        p1 = tri.p1;
        p2 = tri.p2.x < tri.p3.x ? (p3 = tri.p3, tri.p2) : (p3 = tri.p2, tri.p3);
    }

    ////////////////////////设置坐标//////////////////////////////////
    kleft  = (p2.x - p1.x) / (p2.y - p1.y);
    kright = (p3.x - p1.x) / (p3.y - p1.y);
    Xbegin = p1.x;
    Xend   = p1.x;

    //////////////////////设置颜色/////////////////////////////////
    rLeftStep  = (p2.color.r * p2.w - p1.color.r * p1.w) / (p2.y - p1.y);
    gLeftStep  = (p2.color.g * p2.w - p1.color.g * p1.w) / (p2.y - p1.y);
    bLeftStep  = (p2.color.b * p2.w - p1.color.b * p1.w) / (p2.y - p1.y);
    rLeftBegin = p1.color.r * p1.w;
    gLeftBegin = p1.color.g * p1.w;
    bLeftBegin = p1.color.b * p1.w;

    rRightStep = (p3.color.r * p3.w - p1.color.r * p1.w) / (p3.y - p1.y);
    gRightStep = (p3.color.g * p3.w - p1.color.g * p1.w) / (p3.y - p1.y);
    bRightStep = (p3.color.b * p3.w - p1.color.b * p1.w) / (p3.y - p1.y);
    rRightEnd  = p1.color.r * p1.w;
    gRightEnd  = p1.color.g * p1.w;
    bRightEnd  = p1.color.b * p1.w;

    //////////////////////设置Z值///////////////////////////////
    zLeftBegin = p1.w;
    zRightEnd  = p1.w;
    zLeftStep  = (p2.w - p1.w) / (p2.y - p1.y);
    zRightStep = (p3.w - p1.w) / (p3.y - p1.y);

    //////////////////////////设置纹理/////////////////////////////
    uLeftStep  = (p2.texture.u * p2.w - p1.texture.u * p1.w) / (p2.y - p1.y);
    vLeftStep  = (p2.texture.v * p2.w - p1.texture.v * p1.w) / (p2.y - p1.y);
    uLeftBegin = p1.texture.u * p1.w;
    vLeftBegin = p1.texture.v * p1.w;
    uRightStep = (p3.texture.u * p3.w - p1.texture.u * p1.w) / (p2.y - p1.y);
    vRightStep = (p3.texture.v * p3.w - p1.texture.v * p1.w) / (p2.y - p1.y);
    uRightEnd  = p1.texture.u * p1.w;
    vRightEnd  = p1.texture.v * p1.w;

    for(float y = p1.y; y >= p2.y; y--)
    {
        if(y > 600) goto NEXT_Y2;
        if(y < 0) break;

        ////////////////////////颜色///////////////////////////
        r_x = rLeftBegin;
        g_x = gLeftBegin;
        b_x = bLeftBegin;

        r_x_step = (rRightEnd - rLeftBegin) / (Xend - Xbegin);
        g_x_step = (gRightEnd - gLeftBegin) / (Xend - Xbegin);
        b_x_step = (bRightEnd - bLeftBegin) / (Xend - Xbegin);

        ////////////////////////纹理///////////////////////////
        u_x = uLeftBegin;
        v_x = vLeftBegin;
        u_x_step = (uRightEnd - uLeftBegin) / (Xend - Xbegin);
        v_x_step = (vRightEnd - vLeftBegin) / (Xend - Xbegin);

        /////////////////////////Z插值/////////////////////////
        z_x = zLeftBegin;
        z_x_step = (zRightEnd - zLeftBegin) / (Xend - Xbegin);

        for(float x = Xbegin; x <= Xend; x++)
        {
            if(x > 800) break;
            if(x < 0)
            {
                //////////////颜色//////////////
                r_x += r_x_step;
                g_x += g_x_step;
                b_x += b_x_step;

                /////////////纹理///////////////
                u_x += u_x_step;
                v_x += v_x_step;

                //////////////1 / Z///////////
                z_x += z_x_step;
                continue;
            }

            utemp = u_x / z_x ;
            vtemp = v_x / z_x ;

            rtemp = r_x / z_x;
            gtemp = g_x / z_x;
            btemp = b_x / z_x;

            rgb = image.pixel( QPoint( int (utemp * image.width() + 0.5) % image.width(), int(image.height() * (1 - vtemp ) +0.5) %  image.height()) );
            rtemp = qRed(rgb) * rtemp;
            gtemp = qGreen(rgb) * gtemp;
            btemp = qBlue(rgb) * btemp;

            int picW = int(x + 0.5) % 800;
            int picH = int(y + 0.5) % 600;
            if( pushZtoQueue( picW, picH, {idx, 1 / z_x} ) )
            {
                rgb = qRgb(rtemp, gtemp, btemp);
                //pen.setColor(QColor(int(r_x + 0.5), int(g_x + 0.5), int(b_x + 0.5)));
                pen.setColor(rgb);
                temppainter.setPen(pen);
                temppainter.drawPoint(int(x + 0.5), int(600 - y + 0.5));
            }

            //////////////颜色//////////////
            r_x += r_x_step;
            g_x += g_x_step;
            b_x += b_x_step;

            /////////////纹理///////////////
            u_x += u_x_step;
            v_x += v_x_step;

            //////////////1 / Z///////////
            z_x += z_x_step;
        }

NEXT_Y2: Xbegin -= kleft;
        Xend -= kright;

        ///////////////////颜色/////////////////////
        rLeftBegin -= rLeftStep;
        gLeftBegin -= gLeftStep;
        bLeftBegin -= bLeftStep;
        rRightEnd  -= rRightStep;
        gRightEnd  -= gRightStep;
        bRightEnd  -= bRightStep;

        //////////////////纹理//////////////////
        uLeftBegin -= uLeftStep;
        vLeftBegin -= vLeftStep;
        uRightEnd  -= uRightStep;
        vRightEnd  -= vRightStep;

        ///////////////////Z值///////////////////
        zLeftBegin -= zLeftStep;
        zRightEnd  -= zRightStep;
    }
}

void RenderWindow::DrawOtherTriangle(MyTriangle& tri, QPainter& temppainter,
                                     QPen& pen, const QImage& image, int idx)
{
    bool  bigEnd = false;
    float temp;
    /////////////////////////////////////
    MyPoint p1, p2, p3;
    /////////////////////////////////////
    float kleft, kright, Xbegin, Xend; //扫描线变化量，水平扫描线起始和结束
    /////////////////////////////////////
    float rLeftStep,  gLeftStep,  bLeftStep,  rLeftBegin, gLeftBegin, bLeftBegin;
    float rRightStep, gRightStep, bRightStep, rRightEnd,   gRightEnd,   bRightEnd;
    float r_x, g_x, b_x, r_x_step, g_x_step, b_x_step;
    /////////////////////////////////////
    QRgb  rgb;
    float uLeftStep, vLeftStep, uLeftBegin, vLeftBegin;
    float uRightStep, vRightStep, uRightEnd, vRightEnd;
    float u_x, v_x, u_x_step, v_x_step;
    float utemp, vtemp;
    //////////////////////////////////////
    int sec = 1;  //记录特殊三角形序号（最大或最小值）
    //////////////Z倒数插值////////////////
    float zLeftStep,  zLeftBegin;
    float zRightStep, zRightEnd;
    float z_x, z_x_step;
    float rtemp, gtemp, btemp;
    ////////////////////

    //顶点排序，方便从上到下--从左到右扫描
    p1 = tri.p1.y > tri.p2.y ? tri.p1 : (sec = 2, tri.p2);
    p1 = p1.y > tri.p3.y ? p1 : (sec = 3, tri.p3);
    switch(sec)
    {
        case 1: p2 = tri.p2.x < tri.p3.x ? (p3 = tri.p3, tri.p2) : (p3 = tri.p2, tri.p3); break;
        case 2: p2 = tri.p1.x < tri.p3.x ? (p3 = tri.p3, tri.p1) : (p3 = tri.p1, tri.p3); break;
        case 3: p2 = tri.p1.x < tri.p2.x ? (p3 = tri.p2, tri.p1) : (p3 = tri.p1, tri.p2); break;
    }

    //计算最小Y值
    int minY = tri.p1.y <  tri.p2.y ? tri.p1.y : tri.p2.y;
    minY = minY < tri.p3.y ? minY : tri.p3.y;

    //////////////////////计算坐标/////////////////////////////////
    kleft  = (p2.x - p1.x) / (p2.y - p1.y);
    kright = (p3.x - p1.x) / (p3.y - p1.y);
    Xbegin = p1.x;
    Xend   = p1.x;
    bool isChange = false;

    //////////////////////设置颜色/////////////////////////////////
    rLeftStep  = (p2.color.r * p2.w - p1.color.r * p1.w) / (p2.y - p1.y);
    gLeftStep  = (p2.color.g * p2.w - p1.color.g * p1.w) / (p2.y - p1.y);
    bLeftStep  = (p2.color.b * p2.w - p1.color.b * p1.w) / (p2.y - p1.y);
    rLeftBegin = p1.color.r * p1.w;
    gLeftBegin = p1.color.g * p1.w;
    bLeftBegin = p1.color.b * p1.w;

    rRightStep = (p3.color.r * p3.w - p1.color.r * p1.w) / (p3.y - p1.y);
    gRightStep = (p3.color.g * p3.w- p1.color.g * p1.w)  / (p3.y - p1.y);
    bRightStep = (p3.color.b * p3.w - p1.color.b * p1.w) / (p3.y - p1.y);
    rRightEnd  = p1.color.r * p1.w;
    gRightEnd  = p1.color.g * p1.w;
    bRightEnd  = p1.color.b * p1.w;
    /////////////////////////////////////////////////////////////

    //////////////////////////设置纹理///////////////////////////////////
    uLeftStep  = (p2.texture.u * p2.w - p1.texture.u * p1.w) / (p2.y - p1.y);
    vLeftStep  = (p2.texture.v * p2.w - p1.texture.v * p1.w) / (p2.y - p1.y);
    uLeftBegin = p1.texture.u * p1.w;
    vLeftBegin = p1.texture.v * p1.w;
    uRightStep = (p3.texture.u * p3.w - p1.texture.u * p1.w) / (p3.y - p1.y);
    vRightStep = (p3.texture.v * p3.w - p1.texture.v * p1.w) / (p3.y - p1.y);
    uRightEnd  = p1.texture.u * p1.w;
    vRightEnd  = p1.texture.v * p1.w;

    //////////////////////设置Z值///////////////////////////////
    zLeftBegin = p1.w;
    zRightEnd  = p1.w;
    zLeftStep  = (p2.w - p1.w) / (p2.y - p1.y);
    zRightStep = (p3.w - p1.w) / (p3.y - p1.y);

    for(float y = p1.y; y >= minY; )
    {
        if(y > 600) goto NEXT_Y;
        if(y < 0) break;

        ////////////////////////颜色///////////////////////////
        r_x = rLeftBegin;
        g_x = gLeftBegin;
        b_x = bLeftBegin;
        r_x_step = (rRightEnd - rLeftBegin) / (Xend - Xbegin);
        g_x_step = (gRightEnd - gLeftBegin) / (Xend - Xbegin);
        b_x_step = (bRightEnd - bLeftBegin) / (Xend - Xbegin);

        ////////////////////////纹理///////////////////////////
        u_x = uLeftBegin;
        v_x = vLeftBegin;
        u_x_step = (uRightEnd - uLeftBegin) / (Xend - Xbegin);
        v_x_step = (vRightEnd - vLeftBegin) / (Xend - Xbegin);

        /////////////////////////Z插值/////////////////////////
        z_x = zLeftBegin;
        z_x_step = (zRightEnd - zLeftBegin) / (Xend - Xbegin);

        for(float x = Xbegin; x <= Xend; x++)
        {
            if(x > 800) break;
            if(x < 0)
            {
                //////////////颜色//////////////
                r_x += r_x_step;
                g_x += g_x_step;
                b_x += b_x_step;

                /////////////纹理///////////////
                u_x += u_x_step;
                v_x += v_x_step;

                //////////////1 / Z///////////
                z_x += z_x_step;
                continue;
            }

            utemp = u_x / z_x ;
            vtemp = v_x / z_x ;
            rtemp = r_x / z_x;
            gtemp = g_x / z_x;
            btemp = b_x / z_x;

            rgb = image.pixel( QPoint( int (utemp * image.width() + 0.5) % image.width(), int(image.height() * ( 1 - vtemp ) +0.5) % image.height()) );
            rtemp = qRed(rgb) * rtemp;
            gtemp = qGreen(rgb) * gtemp;
            btemp = qBlue(rgb) * btemp;

            int picW = int(x + 0.5) % 800;
            int picH = int(y + 0.5) % 600;
            if( pushZtoQueue( picW, picH, {idx, 1 / z_x} ) )
            {
                rgb = qRgb(rtemp, gtemp, btemp);
                //pen.setColor(QColor(int(r_x + 0.5), int(g_x + 0.5), int(b_x + 0.5)));
                pen.setColor(rgb);
                temppainter.setPen(pen);
                temppainter.drawPoint(int(x + 0.5), int(600 - y + 0.5));
            }

            //////////////颜色//////////////
            r_x += r_x_step;
            g_x += g_x_step;
            b_x += b_x_step;

            /////////////纹理///////////////
            u_x += u_x_step;
            v_x += v_x_step;

            //////////////1 / Z///////////
            z_x += z_x_step;
        }

 NEXT_Y:  y--;

        if(p2.y > y && y > p3.y && isChange == false)
        {
            if(bigEnd == false)
            {
                kleft  = (p3.x - p2.x)/(p3.y - p2.y);
                Xbegin = p2.x;
                isChange = true;

                ////////////////////////颜色///////////////////////////////
                rLeftStep  = (p3.color.r * p3.w - p2.color.r * p2.w) / (p3.y - p2.y);
                gLeftStep  = (p3.color.g * p3.w - p2.color.g * p2.w) / (p3.y - p2.y);
                bLeftStep  = (p3.color.b * p3.w - p2.color.b * p2.w) / (p3.y - p2.y);
                rLeftBegin = p2.color.r * p2.w;
                gLeftBegin = p2.color.g * p2.w;
                bLeftBegin = p2.color.b * p2.w;

                ////////////////////////纹理///////////////////////////////
                uLeftStep  = (p3.texture.u * p3.w - p2.texture.u * p2.w) / (p3.y - p2.y);
                vLeftStep  = (p3.texture.v * p3.w - p2.texture.v * p2.w) / (p3.y - p2.y);
                uLeftBegin = p2.texture.u * p2.w;
                vLeftBegin = p2.texture.v * p2.w;

                ////////////////////////Z值///////////////////////////////
                zLeftStep  = (p3.w - p2.w) / (p3.y - p2.y);
                zLeftBegin = p2.w;
            }
            else
            {
                kright = (p3.x - p2.x) / (p3.y - p2.y);
                Xend = p2.x;
                isChange = true;

                ////////////////////////颜色///////////////////////////////
                rRightStep = (p3.color.r * p3.w - p2.color.r * p2.w) / (p3.y - p2.y);
                gRightStep = (p3.color.g * p3.w - p2.color.g * p2.w) / (p3.y - p2.y);
                bRightStep = (p3.color.b * p3.w - p2.color.b * p2.w) / (p3.y - p2.y);
                rRightEnd  = p2.color.r * p2.w;
                gRightEnd  = p2.color.g * p2.w;
                bRightEnd  = p2.color.b * p2.w;

                ////////////////////////纹理///////////////////////////////
                uRightStep = (p3.texture.u * p3.w - p2.texture.u * p2.w) / (p3.y - p2.y);
                vRightStep = (p3.texture.v * p3.w - p2.texture.v * p2.w) / (p3.y - p2.y);
                uRightEnd  = p2.texture.u * p2.w;
                vRightEnd  = p2.texture.v * p2.w;

                ////////////////////////Z值///////////////////////////////
                zRightStep = (p3.w - p2.w) / (p3.y - p2.y);
                zRightEnd  = p2.w;
            }

        }
        else if(p2.y < y && y < p3.y && isChange == false)
        {

            if(bigEnd == false)
            {
                kright = (p2.x - p3.x) / (p2.y - p3.y);
                Xend   = p3.x;
                isChange = true;

                ///////////////////////////颜色/////////////////////////////
                rRightStep = (p2.color.r * p2.w - p3.color.r * p3.w) / (p2.y - p3.y);
                gRightStep = (p2.color.g * p2.w - p3.color.g * p3.w) / (p2.y - p3.y);
                bRightStep = (p2.color.b * p2.w - p3.color.b * p3.w) / (p2.y - p3.y);
                rRightEnd  = p3.color.r * p3.w;
                gRightEnd  = p3.color.g * p3.w;
                bRightEnd  = p3.color.b * p3.w;

                ////////////////////////纹理///////////////////////////////
                uRightStep = (p2.texture.u * p2.w - p3.texture.u * p3.w) / (p2.y - p3.y);
                vRightStep = (p2.texture.v * p2.w - p3.texture.v * p3.w) / (p2.y - p3.y);
                uRightEnd  = p3.texture.u * p3.w;
                vRightEnd  = p3.texture.v * p3.w;

                ////////////////////////Z值///////////////////////////////
                zRightStep = (p2.w - p3.w) / (p2.y - p3.y);
                zRightEnd  = p3.w;
            }
            else
            {
                kleft = (p2.x - p3.x) / (p2.y - p3.y);
                Xbegin = p3.x;
                isChange = true;

                ///////////////////////////颜色/////////////////////////////
                rLeftStep  = (p2.color.r * p2.w - p3.color.r * p3.w) / (p2.y - p3.y);
                gLeftStep  = (p2.color.g * p2.w - p3.color.g * p3.w) / (p2.y - p3.y);
                bLeftStep  = (p2.color.b * p2.w - p3.color.b * p3.w) / (p2.y - p3.y);
                rLeftBegin = p3.color.r * p3.w;
                gLeftBegin = p3.color.g * p3.w;
                bLeftBegin = p3.color.b * p3.w;

                ////////////////////////纹理///////////////////////////////
                uLeftStep  = (p2.texture.u * p2.w - p3.texture.u * p3.w) / (p2.y - p3.y);
                vLeftStep  = (p2.texture.v * p2.w - p3.texture.v * p3.w) / (p2.y - p3.y);
                uLeftBegin = p3.texture.u * p3.w;
                vLeftBegin = p3.texture.v * p3.w;

                ////////////////////////Z值///////////////////////////////
                zLeftStep  = (p2.w - p3.w) / (p2.y - p3.y);
                zLeftBegin = p3.w;
            }
        }

        ///////////////////坐标/////////////////////
        Xbegin -= kleft;
        Xend -= kright;

        ///////////////////颜色/////////////////////
        rLeftBegin -= rLeftStep;
        gLeftBegin -= gLeftStep;
        bLeftBegin -= bLeftStep;
        rRightEnd  -= rRightStep;
        gRightEnd  -= gRightStep;
        bRightEnd  -= bRightStep;

        //////////////////纹理/////////////////////
        uLeftBegin -= uLeftStep;
        vLeftBegin -= vLeftStep;
        uRightEnd  -= uRightStep;
        vRightEnd  -= vRightStep;

        ///////////////////Z值///////////////////
        zLeftBegin -= zLeftStep;
        zRightEnd  -= zRightStep;

        if(Xbegin > Xend)
        {
            bigEnd = true;
            ///////////////////坐标/////////////////////
            temp = kleft;
            kleft = kright;
            kright = temp;
            temp = Xbegin;
            Xbegin = Xend;
            Xend = temp;

            ///////////////////颜色/////////////////////
            temp = rLeftStep;
            rLeftStep = rRightStep;
            rRightStep = temp;

            temp = gLeftStep;
            gLeftStep = gRightStep;
            gRightStep = temp;

            temp = bLeftStep;
            bLeftStep = bRightStep;
            bRightStep = temp;

            temp = rLeftBegin;
            rLeftBegin = rRightEnd;
            rRightEnd = temp;

            temp = gLeftBegin;
            gLeftBegin = gRightEnd;
            gRightEnd = temp;

            temp = bLeftBegin;
            bLeftBegin = bRightEnd;
            bRightEnd = temp;

            //////////////////纹理/////////////////////
            temp = uLeftStep;
            uLeftStep = uRightStep;
            uRightStep = temp;

            temp = vLeftStep;
            vLeftStep = vRightStep;
            vRightStep = temp;

            temp = uLeftBegin;
            uLeftBegin = uRightEnd;
            uRightEnd = temp;

            temp = vLeftBegin;
            vLeftBegin = vRightEnd;
            vRightEnd = temp;

            ///////////////////Z值///////////////////
            temp = zLeftStep;
            zLeftStep = zRightStep;
            zRightStep = temp;

            temp = zLeftBegin;
            zLeftBegin = zRightEnd;
            zRightEnd = temp;
        }
    }
}

void RenderWindow::DrawTriangleFace(MyTriangle& tri, QPainter& temppainter, QPen& pen,
                                    const QImage& image, int idx)
{
    //退化三角形
    if(IsEqual(tri.p1.x, tri.p2.x) && IsEqual(tri.p1.x, tri.p3.x) ||
       IsEqual(tri.p1.y, tri.p2.y) && IsEqual(tri.p1.y, tri.p3.y))
        return;

    //////////////平顶或平底//////////////
    bool tp1_tp2 = IsEqual(tri.p1.y, tri.p2.y);
    bool tp1_tp3 = IsEqual(tri.p1.y, tri.p3.y);
    bool tp2_tp3 = IsEqual(tri.p2.y, tri.p3.y);
    if(tp1_tp2 || tp1_tp3 || tp2_tp3)
    {
        /////////////////平顶//////////////
        if(tp1_tp2 && tri.p3.y < tri.p1.y ||
           tp1_tp3 && tri.p2.y < tri.p1.y ||
           tp2_tp3 && tri.p1.y < tri.p2.y)
        {
            DrawTopTriangle(tri, temppainter, pen, image,
                            tp1_tp2, tp1_tp3, tp2_tp3, idx);
        }
        /////////////////平底/////////////
        else
        {
            DrawBottomTriangle(tri, temppainter, pen, image,
                               tp1_tp2, tp1_tp3, tp2_tp3, idx);
        }
    }
    /////////////其他//////////////
    else
    {
        DrawOtherTriangle(tri, temppainter, pen, image, idx);
    }
}


void RenderWindow::SetControler(Controller* controler)
{
    m_pControler = controler;
}
