#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include "basicelement.h"
#include "controller.h"
#include <QPen>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMainWindow>

struct MyPoint
{
    float x, y, z, w;
    Color4f color;
    TextureCoord texture;
};

struct MyTriangle
{
    MyPoint p1, p2, p3;
};

struct MyDepth
{
    int triIdx;
    float z;
};

class Controller;

class RenderWindow : public QMainWindow
{
    Q_OBJECT

    enum DoubleCache{FRONT, BACkGROUND};

public:
    RenderWindow(QWidget *parent = 0);
    ~RenderWindow();
    void paintEvent(QPaintEvent *event);
    void SetControler(Controller* controler);
    void RenderTriangles(const GloabalRender& gRender);
    void DrawTriangleFace(MyTriangle& tri, QPainter& tempPainter, QPen& pen, const QImage& image, int idx);
    void DrawTriangleLines(MyTriangle& tri, QPainter& tempPainter, QPen& pen);

private:
    void InitDepthCache();
    void SplitStriangle(MyTriangle& tri, QPainter& tempPainter, QPen& pen, const QImage& image);
    void DrawTopTriangle(MyTriangle& tri, QPainter& temppainter, QPen& pen, const QImage& image,
                         bool tp1_tp2, bool tp1_tp3, bool tp2_tp3, int idx);
    void DrawBottomTriangle(MyTriangle& tri, QPainter& temppainter, QPen& pen, const QImage& image,
                            bool tp1_tp2, bool tp1_tp3, bool tp2_tp3, int idx);
    void DrawOtherTriangle(MyTriangle& tri, QPainter& temppainter, QPen& pen, const QImage& image, int idx);

    void RenderText(QPainter& painter, bool bfront);
    void RenderHightSelRect(MyTriangle& tri, QPainter& painter);

    bool pushZtoQueue(int w, int h, MyDepth mydp);
    void PickupTriangle(QPoint pos);

protected:
    void keyPressEvent(QKeyEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    float **m_depth;
    vector<MyDepth>** m_picDepth;
    bool m_bShowPlanes;
    bool m_bShowLines;
    int  m_selectedTriIdx;
    QPen m_Pen;
    QPixmap *m_FrontPixmap;
    QPixmap *m_BackPixmap;
    DoubleCache m_dblCache;
    Controller* m_pControler;
    MyTriangle  m_tri;
    QPoint m_mouseSreenPos;
    QString m_currentModelName;
};

#endif // RENDERWINDOW_H
