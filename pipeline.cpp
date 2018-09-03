#include "pipeline.h"
#include "controller.h"
#include <QImage>
#include <QDir>
#include <QDebug>

using namespace Eigen;

int PipeLine::m_LightNums = 0;

PipeLine::PipeLine()
{
    m_pControler = NULL;

    m_SphereRotateY = 90;

    m_Camera.camPos = Vector4f(0, 0, 0, 1);
    m_Camera.camDir = Vector4f(0, 0, -1, 1);
    m_Camera.up = Vector4f(0, 1, 0, 1);
    m_Camera.theta = 90;
    m_Camera.zNear = -100;
    m_Camera.zFar = -900;
    m_Camera.viewWidth = 800;
    m_Camera.viewHeight = 600;
    m_Camera.nearBottomLeft = Vector4f(-400, -400, -100, 1);
    m_Camera.farTopRight = Vector4f(400, 400, -900, 1);
    m_Camera.aspect = m_Camera.viewWidth / m_Camera.viewHeight;

    m_Matrix.LookAt(m_Camera.up, m_Camera.camPos, m_Camera.camDir);
    //m_Matrix.Ortho(m_Camera.nearBottomLeft, m_Camera.farTopRight);
    m_Matrix.Perspective(m_Camera.theta, m_Camera.aspect, m_Camera.zNear, m_Camera.zFar);
    m_Matrix.Viewport(Vector4f(0, 0, 0, 1), Vector4f(m_Camera.viewWidth, m_Camera.viewHeight, 0, 1));

    g_globalRebder.imgPath.push_back(QString(":/Images/grass.jpg"));
    g_globalRebder.imgPath.push_back(QString(":/Images/Earth.jpg"));
    g_globalRebder.imgPath.push_back(QString(":/Images/container.jpg"));
    g_globalRebder.imgPath.push_back(QString(":/Images/wood.jpg"));
}

PipeLine::~PipeLine()
{
    for(int i = 0; i < g_globalRebder.modelName.size(); i++)
        delete g_globalRebder.modelName[i];
}

void PipeLine::SetControler(Controller* control)
{
    m_pControler = control;
}

void  PipeLine::RebuildScene()
{
    m_pControler->RenderTriangles(g_globalRebder);
}

int  PipeLine::GeSelectedScreenTri(QPoint pos)
{
    float u, v;
    float v0v0, v1v1, v0v1, v1v0, v2v0, v2v1;
    Vector2f v0, v1, v2;
    vector<Point> &pointlist = g_globalRebder.pointList;
    vector<Triangle> &triangleList = g_globalRebder.triangleList;
    for(int i = 0; i < triangleList.size(); i++)
    {
        v0 = Vector2f((pointlist[triangleList[i].v2].pos(0) - pointlist[triangleList[i].v1].pos(0)),
                      (pointlist[triangleList[i].v2].pos(1) - pointlist[triangleList[i].v1].pos(1)) );
        v1 = Vector2f((pointlist[triangleList[i].v3].pos(0) - pointlist[triangleList[i].v1].pos(0)),
                      (pointlist[triangleList[i].v3].pos(1) - pointlist[triangleList[i].v1].pos(1)) );
        v2 = Vector2f((pos.x() - pointlist[triangleList[i].v1].pos(0)),
                      (pos.y() - pointlist[triangleList[i].v1].pos(1)) );
        v0v0 = v0.dot(v0);
        v1v1 = v1.dot(v1);
        v0v1 = v0.dot(v1);
        v1v0 = v1.dot(v0);
        v2v0 = v2.dot(v0);
        v2v1 = v2.dot(v1);

        u = (v1v1 * v2v0 - v1v0 * v2v1) / (v0v0 * v1v1 - v0v1 * v1v0);
        v = (v0v0 * v2v1 - v0v1 * v2v0) / (v0v0 * v1v1 - v0v1 * v1v0);

        if (u < 0 || u > 1)
            continue;
        if (v < 0 || v > 1)
            continue;
        if( u + v <= 1)
        {
            return i;
        }
    }
    return -1;
}

int PipeLine::SearchPointInSet(vector<Point>& pointSet, Point& p)
{
    static int index = 0;
    int i = 0;
    for( ; i < pointSet.size(); i++)
    {
        if(fabs(pointSet[i].pos(0) - p.pos(0)) < 1e-4
           && fabs(pointSet[i].pos(1) - p.pos(1)) < 1e-4
           && fabs(pointSet[i].pos(2) - p.pos(2)) < 1e-4 )
            return i;
    }

    p.nNum = index ++;
    pointSet.push_back(p);
    return i;
}


int PipeLine::SearchTriangleInSet(vector<Triangle>& triangleList, Triangle& tri)
{
    static int index = 0;
    int i = 0;
    for( ; i < triangleList.size(); i++)
    {
        if(    (triangleList[i].localv1 == tri.localv1 || triangleList[i].localv1 == tri.localv2 || triangleList[i].localv1 == tri.localv3)
            && (triangleList[i].localv2 == tri.localv1 || triangleList[i].localv2 == tri.localv2 || triangleList[i].localv2 == tri.localv3)
            && (triangleList[i].localv3 == tri.localv1 || triangleList[i].localv3 == tri.localv2 || triangleList[i].localv3 == tri.localv3) )
            return i;
    }
    tri.nNum = index ++;
    triangleList.push_back(tri);
    return i;
}


int PipeLine::SearchEyeTriangleInSet(vector<Triangle>& triangleList, Triangle& tri)
{
    static int index = 0;
    int i = 0;
    for( ; i < triangleList.size(); i++)
    {
        if(    (triangleList[i].v1 == tri.v1 || triangleList[i].v1 == tri.v2 || triangleList[i].v1 == tri.v3)
            && (triangleList[i].v2 == tri.v1 || triangleList[i].v2 == tri.v2 || triangleList[i].v2 == tri.v3)
            && (triangleList[i].v3 == tri.v1 || triangleList[i].v3 == tri.v2 || triangleList[i].v3 == tri.v3))
            return i;
    }
    tri.nNum = index ++;
    triangleList.push_back(tri);
    return i;
}


int PipeLine::InsertModelTexture(vector<Texture>& textureList, Texture& tex)
{
    static int index = 0;
    int i = 0;
    for( ; i < textureList.size(); i++)
    {
        if(tex.picPath == textureList[i].picPath)
            return i;
    }
    tex.id = index ++;
    textureList.push_back(tex);
    return textureList.size() - 1;
}


Vector3f  PipeLine::Vector4fTo3f(const Vector4f& vec4f)
{
    Vector3f vec3f;
    vec3f(0) = vec4f(0);
    vec3f(1) = vec4f(1);
    vec3f(2) = vec4f(2);
    return vec3f;
}


Vector4f  PipeLine::Vector3fTo4f(const Vector3f& vec3f)
{
    Vector4f vec4f;
    vec4f(0) = vec3f(0);
    vec4f(1) = vec3f(1);
    vec4f(2) = vec3f(2);
    vec4f(2) = 1;
    return vec4f;
}

void PipeLine::CalculateNormal(Model& model, Triangle& triangle, vector<Point>& pointList)
{
    static int i = 0;
    Vector3f vec1, vec2, normal;
    int triSrc = SearchTriangleInSet(model.triangleList, triangle);
    int surfIndex = FindSurfaceByTri(model, triSrc);
    if(model.surfList[surfIndex].bHandle)
        return;
    else model.surfList[surfIndex].bHandle = true;
    int triDst = GetAnotherSurfTri(model.surfList[surfIndex], triSrc);
    int pointIndex = -1;
    if(triDst > 0)
        pointIndex = FindPointNotIntriSrc(model, triSrc, triDst);
    vec1 = Vector4fTo3f(pointList[triangle.v2].pos) - Vector4fTo3f(pointList[triangle.v1].pos);
    vec2 = Vector4fTo3f(pointList[triangle.v3].pos) - Vector4fTo3f(pointList[triangle.v2].pos);
    normal = vec1.cross(vec2);
    normal.normalize();
    triangle.worldNormal = normal;
    pointList[triangle.v1].worldNormal += normal;
    pointList[triangle.v1].normalCount ++;
    pointList[triangle.v2].worldNormal += normal;
    pointList[triangle.v2].normalCount ++;
    pointList[triangle.v3].worldNormal += normal;
    pointList[triangle.v3].normalCount ++;

    if(triDst > 0)
    {
        model.triangleList[triDst].worldNormal = normal;
        pointList[pointIndex].worldNormal += normal;
        pointList[pointIndex].normalCount ++;
    }
}

int  PipeLine::FindPointNotIntriSrc(Model& model, int triSrc, int triDst)
{
    if( model.triangleList[triSrc].v1 != model.triangleList[triDst].v1 &&
        model.triangleList[triSrc].v2 != model.triangleList[triDst].v1 &&
        model.triangleList[triSrc].v3 != model.triangleList[triDst].v1 )
        return model.triangleList[triDst].v1;
    else if( model.triangleList[triSrc].v1 != model.triangleList[triDst].v2 &&
             model.triangleList[triSrc].v2 != model.triangleList[triDst].v2 &&
             model.triangleList[triSrc].v3 != model.triangleList[triDst].v2 )
        return model.triangleList[triDst].v2;
    else
        return model.triangleList[triDst].v3;
}

int   PipeLine::FindSurfaceByTri(Model& model, int index)
{
    for(int i = 0; i < model.surfList.size(); i++)
    {
        if(model.surfList[i].triIndex_a == index || model.surfList[i].triIndex_b == index)
            return i;
    }
    return -1;
}


void PipeLine::CalculateWorldNormal(Model& model)
{
    for(int i = 0; i < model.surfList.size(); i++)
    {
        model.surfList[i].bHandle = false;
    }
    for(int i = 0; i < model.triangleList.size(); i++)
    {
         CalculateNormal(model, model.triangleList[i], model.pointList);
    }
}

void PipeLine::BackfaceCulling()
{
    Triangle tri;   tri.init();
    Vector3f vec1;

    for(int i = 0; i < m_ModelList.size(); i++)
    {
        for(int j = 0; j < m_ModelList[i].triangleList.size(); j++)
        {
            vec1 = Vector4fTo3f(m_ModelList[i].pointList[m_ModelList[i].triangleList[j].v2].pos) - Vector4fTo3f(m_Camera.camPos);
            vec1.normalize();
            if(vec1.dot(m_ModelList[i].triangleList[j].worldNormal) < 0)
            {
                tri = m_ModelList[i].triangleList[j];
                tri.v1 = SearchPointInSet(g_globalRebder.pointList, m_ModelList[i].pointList[m_ModelList[i].triangleList[j].v1]);
                tri.v2 = SearchPointInSet(g_globalRebder.pointList, m_ModelList[i].pointList[m_ModelList[i].triangleList[j].v2]);
                tri.v3 = SearchPointInSet(g_globalRebder.pointList, m_ModelList[i].pointList[m_ModelList[i].triangleList[j].v3]);
                if(m_ModelList[i].textureList.size())
                     tri.textureIndex = InsertModelTexture(g_globalRebder.textureList, m_ModelList[i].textureList[m_ModelList[i].triangleList[j].textureIndex]);
                g_globalRebder.triangleList.push_back(tri);
            }
        }
    }
}

void PipeLine::FrustumsCulling()
{
    //左右上下前(上)后(上)：1, 2, 4, 8, 16, 32, 64, 128
    int LEFT = 1, RIGHT = 2, UP = 4, DOWN = 8, FRONT = 16, FRONT_ON = 32, BACK = 64, BACK_ON = 128;
    float kwl = 0, kwr = 0, khu = 0, khd = 0, t1 = 0, t2 = 0;
    int inout1 = 0, inout2 = 0, inout3 = 0;
    Point p1, p2, p3, point1, point2, point3;
    p1.init(); p2.init(); p3.init();
    point1.init(); point2.init(); point3.init();
    Vector4f vL, vR;
    TextureCoord texL, texR, tex1, tex2, tex3;
    texL.init(); texR.init(); tex1.init();; tex2.init(); tex3.init();
    Color4f colorL, colorR;  colorL.init(); colorR.init();
    Triangle tri;  tri.init();
    float zNear = 0, zFar = 0;
    vector<Point> tempPointList;   //顶点渲染队列
    vector<Triangle> tempTriangleList;  //三角形渲染队列

    zNear = m_Camera.zNear;
    zFar =  m_Camera.zFar;

    kwl = 2 * m_Camera.zNear / m_Camera.viewWidth;
    kwr = -kwl;
    khu = -tan(m_Camera.theta / 360.0f * PI);
    khd = -khu;

    for(int i = 0; i < g_globalRebder.triangleList.size(); i++)
        g_globalRebder.triangleList[i].state = false;

    for(int i = 0; i < g_globalRebder.triangleList.size(); i++)
    {
        inout1 =0, inout2 = 0, inout3 = 0;
        p1 = g_globalRebder.pointList[g_globalRebder.triangleList[i].v1];
        p2 = g_globalRebder.pointList[g_globalRebder.triangleList[i].v2];
        p3 = g_globalRebder.pointList[g_globalRebder.triangleList[i].v3];

        //左边界
        if((p1.pos(2) - kwl * p1.pos(0)) > 0)
            inout1 += LEFT;
        if((p2.pos(2) - kwl * p2.pos(0)) > 0)
            inout2 += LEFT;
        if((p3.pos(2) - kwl * p3.pos(0)) > 0)
            inout3 += LEFT;
        if( inout1 & inout2 & inout3 & LEFT )
            continue;

        //右边界
        if((p1.pos(2) - kwr * p1.pos(0)) > 0)
            inout1 += LEFT;
        if((p2.pos(2) - kwr * p2.pos(0)) > 0)
            inout2 += LEFT;
        if((p3.pos(2) - kwr * p3.pos(0)) > 0)
            inout3 += LEFT;
        if( inout1 & inout2 & inout3 & RIGHT )
            continue;

        //上边界
        if((p1.pos(1) - khu * p1.pos(2)) > 0)
            inout1 += UP;
        if((p2.pos(1) - khu * p2.pos(2)) > 0)
            inout2 += UP;
        if((p3.pos(1) - khu * p3.pos(2)) > 0)
            inout3 += UP;
        if( inout1 & inout2 & inout3 & UP )
            continue;

        //下边界
        if((p1.pos(1) - khd * p1.pos(2)) < 0)
            inout1 += DOWN;
        if((p2.pos(1) - khd * p2.pos(2)) < 0)
            inout2 += DOWN;
        if((p3.pos(1) - khd * p3.pos(2)) < 0)
            inout3 += DOWN;
        if( inout1 & inout2 & inout3 & DOWN )
            continue;

        //近裁剪面
        if(p1.pos(2) > zNear)
            inout1 += FRONT;
        else if(fabs(p1.pos(2) - zNear) < 1e-6)
            inout1 += FRONT_ON;
        if(p2.pos(2) > zNear)
            inout2 += FRONT;
        else if (fabs(p2.pos(2) - zNear) < 1e-6)
            inout2 += FRONT_ON;
        if(p3.pos(2) > zNear)
            inout3 += FRONT;
        else if(fabs(p3.pos(2) - zNear) < 1e-6)
            inout3 += FRONT_ON;

        if( inout1 & inout2 & inout3 & FRONT || (inout1 & FRONT_ON && inout2 & inout3 & FRONT) \
            || (inout2 & FRONT_ON && inout1 & inout3 & FRONT) || (inout3 & FRONT_ON && inout1 & inout2 & FRONT) \
            || (inout1 & FRONT && inout2 & inout3 & FRONT_ON) || (inout2 & FRONT && inout1 & inout3 & FRONT_ON) \
            || (inout3 & FRONT && inout1 & inout2 & FRONT_ON))
            continue;

        if(!(inout1 & FRONT) && inout2 & FRONT && inout3 & FRONT_ON)   //1
        {
            t1 = (zNear - p1.pos(2)) / (p2.pos(2) - p1.pos(2));
            GetInsecValue(p1, p2, t1, zNear, g_globalRebder.triangleList[i].texV1,
                          g_globalRebder.triangleList[i].texV2, vL, colorL, texL);
            p2.pos = vL;
            p2.color = colorL;
            g_globalRebder.triangleList[i].texV2 = texL;
        }
        else if(!(inout3 & FRONT) && inout1 & FRONT && inout2 & FRONT_ON)  //2
        {
            t1 = (zNear - p3.pos(2)) / (p1.pos(2) - p3.pos(2));
            GetInsecValue(p3, p1, t1, zNear, g_globalRebder.triangleList[i].texV3,
                          g_globalRebder.triangleList[i].texV1, vL, colorL, texL);
            p1.pos = vL;
            p1.color = colorL;
            g_globalRebder.triangleList[i].texV1 = texL;
        }
        else if(!(inout2 & FRONT) && inout3 & FRONT && inout1 & FRONT_ON)  //3
        {
            t1 = (zNear - p2.pos(2)) / (p3.pos(2) - p2.pos(2));
            GetInsecValue(p2, p3, t1, zNear, g_globalRebder.triangleList[i].texV2,
                          g_globalRebder.triangleList[i].texV3, vL, colorL, texL);
            p3.pos = vL;
            p3.color = colorL;
            g_globalRebder.triangleList[i].texV3 = texL;
        }
        else if(!(inout1 & FRONT) && inout3 & FRONT && inout2 & FRONT_ON)  //1
        {
            t1 = (zNear - p1.pos(2)) / (p3.pos(2) - p1.pos(2));
            GetInsecValue(p1, p3, t1, zNear, g_globalRebder.triangleList[i].texV1,
                          g_globalRebder.triangleList[i].texV3, vR, colorR, texR);
            p3.pos = vR;
            p3.color = colorR;
            g_globalRebder.triangleList[i].texV3 = texR;
        }
        else if(!(inout3 & FRONT) && inout2 & FRONT && inout1 & FRONT_ON)  //2
        {
            t1 = (zNear - p3.pos(2)) / (p2.pos(2) - p3.pos(2));
            GetInsecValue(p3, p2, t1, zNear, g_globalRebder.triangleList[i].texV3,
                          g_globalRebder.triangleList[i].texV2, vR, colorR, texR);
            p2.pos = vR;
            p2.color = colorR;
            g_globalRebder.triangleList[i].texV2 = texR;
        }
        else if(!(inout2 & FRONT) && inout1 & FRONT && inout3 & FRONT_ON)  //3
        {
            t1 = (zNear - p2.pos(2)) / (p1.pos(2) - p2.pos(2));
            GetInsecValue(p2, p1, t1, zNear, g_globalRebder.triangleList[i].texV2,
                          g_globalRebder.triangleList[i].texV1, vR, colorR, texR);
            p1.pos = vR;
            p1.color = colorR;
            g_globalRebder.triangleList[i].texV1 = texR;
        }
        else if( !(inout1 & FRONT) && inout2 & inout3 & FRONT )  //1
        {
            t1 = (zNear - p1.pos(2)) / (p2.pos(2) - p1.pos(2));
            GetInsecValue(p1, p2, t1, zNear, g_globalRebder.triangleList[i].texV1,
                          g_globalRebder.triangleList[i].texV2, vL, colorL, texL);
            point2.pos = vL;
            point2.color = colorL;
            g_globalRebder.triangleList[i].texV2 = texL;

            t2 = (zNear - p1.pos(2)) / (p3.pos(2) - p1.pos(2));
            GetInsecValue(p1, p3, t2, zNear, g_globalRebder.triangleList[i].texV1,
                          g_globalRebder.triangleList[i].texV3, vR, colorR, texR);
            point3.pos = vR;
            point3.color = colorR;
            g_globalRebder.triangleList[i].texV3 = texR;

            p2 = point2;
            p3 = point3;
        }
        else if(!(inout2 & FRONT) && inout1 & inout3 & FRONT)  //2
        {
            t1 = (zNear - p2.pos(2)) / (p3.pos(2) - p2.pos(2));
            GetInsecValue(p2, p3, t1, zNear, g_globalRebder.triangleList[i].texV2,
                          g_globalRebder.triangleList[i].texV3, vL, colorL, texL);
            point3.pos = vL;
            point3.color = colorL;
            g_globalRebder.triangleList[i].texV3 = texL;

            t2 = (zNear - p2.pos(2)) / (p1.pos(2) - p2.pos(2));
            GetInsecValue(p2, p1, t2, zNear, g_globalRebder.triangleList[i].texV2,
                          g_globalRebder.triangleList[i].texV1, vR, colorR, texR);
            point1.pos = vR;
            point1.color = colorR;
            g_globalRebder.triangleList[i].texV1 = texR;

            p3 = point3;
            p1 = point1;
        }
        else if(!(inout3 & FRONT) && inout1 & inout2 & FRONT)   //3
        {
            t1 = (zNear - p3.pos(2)) / (p1.pos(2) - p3.pos(2));
            GetInsecValue(p3, p1, t1, zNear, g_globalRebder.triangleList[i].texV3,
                          g_globalRebder.triangleList[i].texV1, vL, colorL, texL);
            point1.pos = vL;
            point1.color = colorL;
            g_globalRebder.triangleList[i].texV1 = texL;

            t2 = (zNear - p3.pos(2)) / (p2.pos(2) - p3.pos(2));
            GetInsecValue(p3, p2, t2, zNear, g_globalRebder.triangleList[i].texV3,
                          g_globalRebder.triangleList[i].texV2, vR, colorR, texR);
            point2.pos = vR;
            point2.color = colorR;
            g_globalRebder.triangleList[i].texV2 = texR;

            p1 = point1;
            p2 = point2;
        }
        else if(!(inout1 & FRONT) && !(inout2 & FRONT) && inout3 & FRONT)   //1
        {
            point1 = p1;
            point2 = p2;
            point3 = p3;
            tex1 = g_globalRebder.triangleList[i].texV1;
            tex2 = g_globalRebder.triangleList[i].texV2;
            tex3 = g_globalRebder.triangleList[i].texV3;
            t1 = (zNear - p2.pos(2)) / (p3.pos(2) - p2.pos(2));
            GetInsecValue(p2, p3, t1, zNear, g_globalRebder.triangleList[i].texV2,
                          g_globalRebder.triangleList[i].texV3, vL, colorL, texL);
            point1.color = colorL;
            point1.pos = vL;

            g_globalRebder.triangleList[i].texV1 = tex1;
            g_globalRebder.triangleList[i].texV2 = tex2;
            g_globalRebder.triangleList[i].texV3 = texL;

            t2 = (zNear - p1.pos(2)) / (p3.pos(2) - p1.pos(2));
            GetInsecValue(p1, p3, t2, zNear, tex1, tex3, vR, colorR, texR);

            point2.color = colorR;
            point2.pos = vR;
            tri = g_globalRebder.triangleList[i];
            tri.texV1 = texL;
            tri.texV2 = texR;
            tri.texV3 = tex1;
            tri.v1 = SearchPointInSet(g_globalRebder.pointList, point1);
            tri.v2 = SearchPointInSet(g_globalRebder.pointList, point2);
            tri.v3 = SearchPointInSet(g_globalRebder.pointList, p1);
            SearchEyeTriangleInSet(g_globalRebder.triangleList, tri);
            p3 = point1;
        }
        else if(!(inout1 & FRONT) && !(inout3 & FRONT) && inout2 & FRONT)  //2
        {
            point1 = p1;
            point2 = p2;
            point3 = p3;
            tex1 = g_globalRebder.triangleList[i].texV1;
            tex2 = g_globalRebder.triangleList[i].texV2;
            tex3 = g_globalRebder.triangleList[i].texV3;
            t1 = (zNear - p1.pos(2)) / (p2.pos(2) - p1.pos(2));
            GetInsecValue(p1, p2, t1, zNear, g_globalRebder.triangleList[i].texV1,
                          g_globalRebder.triangleList[i].texV2, vL, colorL, texL);
            point1.color = colorL;
            point1.pos = vL;

            g_globalRebder.triangleList[i].texV1 = tex1;
            g_globalRebder.triangleList[i].texV2 = texL;
            g_globalRebder.triangleList[i].texV3 = tex3;

            t2 = (zNear - p3.pos(2)) / (p2.pos(2) - p3.pos(2));
            GetInsecValue(p3, p2, t2, zNear, tex3, tex2, vR, colorR, texR);

            point2.color = colorR;
            point2.pos = vR;
            tri = g_globalRebder.triangleList[i];
            tri.texV1 = texL;
            tri.texV2 = texR;
            tri.texV3 = tex3;
            tri.v1 = SearchPointInSet(g_globalRebder.pointList, point1);
            tri.v2 = SearchPointInSet(g_globalRebder.pointList, point2);
            tri.v3 = SearchPointInSet(g_globalRebder.pointList, p3);
            SearchEyeTriangleInSet(g_globalRebder.triangleList, tri);
            p2 = point1;
        }
        else if(!(inout2 & FRONT) && !(inout3 & FRONT) && inout1 & FRONT)  //3
        {
            point1 = p1;
            point2 = p2;
            point3 = p3;
            tex1 = g_globalRebder.triangleList[i].texV1;
            tex2 = g_globalRebder.triangleList[i].texV2;
            tex3 = g_globalRebder.triangleList[i].texV3;
            t1 = (zNear - p3.pos(2)) / (p1.pos(2) - p3.pos(2));
            GetInsecValue(p3, p1, t1, zNear, g_globalRebder.triangleList[i].texV3,
                          g_globalRebder.triangleList[i].texV1, vL, colorL, texL);
            point1.color = colorL;
            point1.pos = vL;

            g_globalRebder.triangleList[i].texV1 = tex3;
            g_globalRebder.triangleList[i].texV2 = texL;
            g_globalRebder.triangleList[i].texV3 = tex2;

            t2 = (zNear - p2.pos(2)) / (p1.pos(2) - p2.pos(2));
            GetInsecValue(p2, p1, t2, zNear, tex2, tex1, vR, colorR, texR);

            point2.color = colorR;
            point2.pos = vR;
            tri = g_globalRebder.triangleList[i];
            tri.texV1 = texL;
            tri.texV2 = texR;
            tri.texV3 = tex2;
            tri.v1 = SearchPointInSet(g_globalRebder.pointList, point1);
            tri.v2 = SearchPointInSet(g_globalRebder.pointList, point2);
            tri.v3 = SearchPointInSet(g_globalRebder.pointList, p2);
            SearchEyeTriangleInSet(g_globalRebder.triangleList, tri);
            p1 = point1;
        }

        //远裁剪面
        if(p1.pos(2) < zFar)
            inout1 += BACK;
        else if(fabs(p1.pos(2) - zFar) < 1e-6)
            inout1 += BACK_ON;
        if(p2.pos(2) < zFar)
            inout2 += BACK;
        else if (fabs(p2.pos(2) - zFar) < 1e-6)
            inout2 += BACK_ON;
        if(p3.pos(2) < zFar)
            inout3 += BACK;
        else if(fabs(p3.pos(2) - zFar) < 1e-6)
            inout3 += BACK_ON;

        if( inout1 & inout2 & inout3 & BACK || (inout1 & BACK_ON && inout2 & inout3 & BACK) \
            || (inout2 & BACK_ON && inout1 & inout3 & BACK) || (inout3 & BACK_ON && inout1 & inout2 & BACK) \
            || (inout1 & BACK && inout2 & inout3 & BACK_ON) || (inout2 & BACK && inout1 & inout3 & BACK_ON) \
            || (inout3 & BACK && inout1 & inout2 & BACK_ON))
            continue;

        if(!(inout1 & BACK) && inout2 & BACK && inout3 & BACK_ON)  //1
        {
            t1 = (zFar - p1.pos(2)) / (p2.pos(2) - p1.pos(2));
            GetInsecValue(p1, p2, t1, zFar, g_globalRebder.triangleList[i].texV1,
                          g_globalRebder.triangleList[i].texV2, vL, colorL, texL);
            p2.pos = vL;
            p2.color = colorL;
            g_globalRebder.triangleList[i].texV2 = texL;
        }
        else if(!(inout3 & BACK) && inout1 & BACK && inout2 & BACK_ON)  //2
        {
            t1 = (zFar - p3.pos(2)) / (p1.pos(2) - p3.pos(2));
            GetInsecValue(p3, p1, t1, zFar, g_globalRebder.triangleList[i].texV3,
                          g_globalRebder.triangleList[i].texV1, vL, colorL, texL);
            p1.pos = vL;
            p1.color = colorL;
            g_globalRebder.triangleList[i].texV1 = texL;
        }
        else if(!(inout2 & BACK) && inout3 & BACK && inout1 & BACK_ON)  //3
        {
            t1 = (zFar - p2.pos(2)) / (p3.pos(2) - p2.pos(2));
            GetInsecValue(p2, p3, t1, zFar, g_globalRebder.triangleList[i].texV2,
                          g_globalRebder.triangleList[i].texV3, vL, colorL, texL);
            p3.pos = vL;
            p3.color = colorL;
            g_globalRebder.triangleList[i].texV3 = texL;
        }
        else if(!(inout1 & BACK) && inout3 & BACK && inout2 & BACK_ON)  //1
        {
            t1 = (zFar - p1.pos(2)) / (p3.pos(2) - p1.pos(2));
            GetInsecValue(p1, p3, t1, zFar, g_globalRebder.triangleList[i].texV1,
                          g_globalRebder.triangleList[i].texV3, vR, colorR, texR);
            p3.pos = vR;
            p3.color = colorR;
            g_globalRebder.triangleList[i].texV3 = texR;
        }
        else if(!(inout3 & BACK) && inout2 & BACK && inout1 & BACK_ON)  //2
        {
            t1 = (zFar - p3.pos(2)) / (p2.pos(2) - p3.pos(2));
            GetInsecValue(p3, p2, t1, zFar, g_globalRebder.triangleList[i].texV3,
                          g_globalRebder.triangleList[i].texV2, vR, colorR, texR);
            p2.pos = vR;
            p2.color = colorR;
            g_globalRebder.triangleList[i].texV2 = texR;
        }
        else if(!(inout2 & BACK) && inout1 & BACK && inout3 & BACK_ON)  //3
        {
            t1 = (zFar - p2.pos(2)) / (p1.pos(2) - p2.pos(2));
            GetInsecValue(p2, p1, t1, zFar, g_globalRebder.triangleList[i].texV2,
                          g_globalRebder.triangleList[i].texV1, vR, colorR, texR);
            p1.pos = vR;
            p1.color = colorR;
            g_globalRebder.triangleList[i].texV1 = texR;
        }
        else if( !(inout1 & BACK) && inout2 & inout3 & BACK )   //1
        {
            t1 = (zFar - p1.pos(2)) / (p2.pos(2) - p1.pos(2));
            GetInsecValue(p1, p2, t1, zFar, g_globalRebder.triangleList[i].texV1,
                          g_globalRebder.triangleList[i].texV2, vL, colorL, texL);
            point2.pos = vL;
            point2.color = colorL;
            g_globalRebder.triangleList[i].texV2 = texL;

            t2 = (zFar - p1.pos(2)) / (p3.pos(2) - p1.pos(2));
            GetInsecValue(p1, p3, t2, zFar, g_globalRebder.triangleList[i].texV1,
                          g_globalRebder.triangleList[i].texV3, vR, colorR, texR);
            point3.pos = vR;
            point3.color = colorR;
            g_globalRebder.triangleList[i].texV3 = texR;

            p2 = point2;
            p3 = point3;
        }
        else if(!(inout2 & BACK) && inout1 & inout3 & BACK)   //2
        {
            t1 = (zFar - p2.pos(2)) / (p3.pos(2) - p2.pos(2));
            GetInsecValue(p2, p3, t1, zFar, g_globalRebder.triangleList[i].texV2,
                          g_globalRebder.triangleList[i].texV3, vL, colorL, texL);
            point3.pos = vL;
            point3.color = colorL;
            g_globalRebder.triangleList[i].texV3 = texL;

            t2 = (zFar - p2.pos(2)) / (p1.pos(2) - p2.pos(2));
            GetInsecValue(p2, p1, t2, zFar, g_globalRebder.triangleList[i].texV2,
                          g_globalRebder.triangleList[i].texV1, vR, colorR, texR);
            point1.pos = vR;
            point1.color = colorR;
            g_globalRebder.triangleList[i].texV1 = texR;

            p3 = point3;
            p1 = point1;
        }
        else if(!(inout3 & BACK) && inout1 & inout2 & BACK)   //3
        {
            t1 = (zFar - p3.pos(2)) / (p1.pos(2) - p3.pos(2));
            GetInsecValue(p3, p1, t1, zFar, g_globalRebder.triangleList[i].texV3,
                          g_globalRebder.triangleList[i].texV1, vL, colorL, texL);
            point1.pos = vL;
            point1.color = colorL;
            g_globalRebder.triangleList[i].texV1 = texL;

            t2 = (zFar - p3.pos(2)) / (p2.pos(2) - p3.pos(2));
            GetInsecValue(p3, p2, t2, zFar, g_globalRebder.triangleList[i].texV3,
                          g_globalRebder.triangleList[i].texV2, vR, colorR, texR);
            point2.pos = vR;
            point2.color = colorR;
            g_globalRebder.triangleList[i].texV2 = texR;

            p1 = point1;
            p2 = point2;
        }
        else if(!(inout1 & BACK) && !(inout2 & BACK) && inout3 & BACK)   //1
        {
            point1 = p1;
            point2 = p2;
            point3 = p3;
            tex1 = g_globalRebder.triangleList[i].texV1;
            tex2 = g_globalRebder.triangleList[i].texV2;
            tex3 = g_globalRebder.triangleList[i].texV3;
            t1 = (zFar - p2.pos(2)) / (p3.pos(2) - p2.pos(2));
            GetInsecValue(p2, p3, t1, zFar, g_globalRebder.triangleList[i].texV2,
                          g_globalRebder.triangleList[i].texV3, vL, colorL, texL);
            point1.color = colorL;
            point1.pos = vL;

            g_globalRebder.triangleList[i].texV1 = tex1;
            g_globalRebder.triangleList[i].texV2 = tex2;
            g_globalRebder.triangleList[i].texV3 = texL;

            t2 = (zFar - p1.pos(2)) / (p3.pos(2) - p1.pos(2));
            GetInsecValue(p1, p3, t2, zFar, tex1, tex3, vR, colorR, texR);

            point2.color = colorR;
            point2.pos = vR;
            tri = g_globalRebder.triangleList[i];
            tri.texV1 = texL;
            tri.texV2 = texR;
            tri.texV3 = tex1;
            tri.v1 = SearchPointInSet(g_globalRebder.pointList, point1);
            tri.v2 = SearchPointInSet(g_globalRebder.pointList, point2);
            tri.v3 = SearchPointInSet(g_globalRebder.pointList, p1);
            SearchEyeTriangleInSet(g_globalRebder.triangleList, tri);
            p3 = point1;
        }
        else if(!(inout1 & BACK) && !(inout3 & BACK) && inout2 & BACK)   //2
        {
            point1 = p1;
            point2 = p2;
            point3 = p3;
            tex1 = g_globalRebder.triangleList[i].texV1;
            tex2 = g_globalRebder.triangleList[i].texV2;
            tex3 = g_globalRebder.triangleList[i].texV3;
            t1 = (zFar - p1.pos(2)) / (p2.pos(2) - p1.pos(2));
            GetInsecValue(p1, p2, t1, zFar, g_globalRebder.triangleList[i].texV1,
                          g_globalRebder.triangleList[i].texV2, vL, colorL, texL);
            point1.color = colorL;
            point1.pos = vL;

            g_globalRebder.triangleList[i].texV1 = tex1;
            g_globalRebder.triangleList[i].texV2 = texL;
            g_globalRebder.triangleList[i].texV3 = tex3;

            t2 = (zFar - p3.pos(2)) / (p2.pos(2) - p3.pos(2));
            GetInsecValue(p3, p2, t2, zFar, tex3, tex2, vR, colorR, texR);

            point2.color = colorR;
            point2.pos = vR;
            tri = g_globalRebder.triangleList[i];
            tri.texV1 = texL;
            tri.texV2 = texR;
            tri.texV3 = tex3;
            tri.v1 = SearchPointInSet(g_globalRebder.pointList, point1);
            tri.v2 = SearchPointInSet(g_globalRebder.pointList, point2);
            tri.v3 = SearchPointInSet(g_globalRebder.pointList, p3);
            SearchEyeTriangleInSet(g_globalRebder.triangleList, tri);
            p2 = point1;
        }
        else if(!(inout2 & BACK) && !(inout3 & BACK) && inout1 & BACK)   //3
        {
            point1 = p1;
            point2 = p2;
            point3 = p3;
            tex1 = g_globalRebder.triangleList[i].texV1;
            tex2 = g_globalRebder.triangleList[i].texV2;
            tex3 = g_globalRebder.triangleList[i].texV3;
            t1 = (zFar - p3.pos(2)) / (p1.pos(2) - p3.pos(2));
            GetInsecValue(p3, p1, t1, zFar, g_globalRebder.triangleList[i].texV3,
                          g_globalRebder.triangleList[i].texV1, vL, colorL, texL);
            point1.color = colorL;
            point1.pos = vL;

            g_globalRebder.triangleList[i].texV1 = tex3;
            g_globalRebder.triangleList[i].texV2 = texL;
            g_globalRebder.triangleList[i].texV3 = tex2;

            t2 = (zFar - p2.pos(2)) / (p1.pos(2) - p2.pos(2));
            GetInsecValue(p2, p1, t2, zFar, tex2, tex1, vR, colorR, texR);

            point2.color = colorR;
            point2.pos = vR;
            tri = g_globalRebder.triangleList[i];
            tri.texV1 = texL;
            tri.texV2 = texR;
            tri.texV3 = tex2;
            tri.v1 = SearchPointInSet(g_globalRebder.pointList, point1);
            tri.v2 = SearchPointInSet(g_globalRebder.pointList, point2);
            tri.v3 = SearchPointInSet(g_globalRebder.pointList, p2);
            SearchEyeTriangleInSet(g_globalRebder.triangleList, tri);
            p1 = point1;
        }

        //==========================================
        tri = g_globalRebder.triangleList[i];
        tri.v1 = SearchPointInSet(tempPointList, p1);
        tri.v2 = SearchPointInSet(tempPointList, p2);
        tri.v3 = SearchPointInSet(tempPointList, p3);
        SearchEyeTriangleInSet(tempTriangleList, tri);
    } //end for

    g_globalRebder.triangleList = tempTriangleList;
    g_globalRebder.pointList = tempPointList;

} //end funcion


void PipeLine::GetInsecValue(Point& p1, Point& p2, float t, float z,
                             TextureCoord& tex1, TextureCoord& tex2,
                             Vector4f& v, Color4f& color, TextureCoord& tex)
{
    v(0) = p1.pos(0) + (p2.pos(0) - p1.pos(0)) * t;
    v(1) = p1.pos(1) + (p2.pos(1) - p1.pos(1)) * t;
    v(2) = z;
    v(3) = 1;
    tex.u = tex1.u + ( tex2.u - tex1.u ) * t;
    tex.v = tex1.v + ( tex2.v - tex1.v ) * t;
    color.r = p1.color.r + (p2.color.r - p1.color.r) * t;
    color.g = p1.color.g + (p2.color.g - p1.color.g) * t;
    color.b = p1.color.b + (p2.color.b - p1.color.b) * t;
    color.a = p1.color.a;
}

Texture PipeLine::CreateTexture(QString imgPath, float ka, float kd, float ks)
{
    Texture tex;
    tex.init();
    tex.picPath = imgPath;
    if(imgPath.size() > 0)
        tex.image = QImage(imgPath);
    tex.ka = ka;
    tex.kd = kd;
    tex.ks = ks;
    return tex;
}

Model PipeLine::CreatePlane(int width, int height, Texture tex, QString name)
{
    static int count = 0;
    count ++;
    Point p1, p2, p3, p4;
    p1.init(); p2.init(); p3.init(); p4.init();
    float halfW = width / 2, halfH = height / 2;
    Model model;
    model.init();
    model.id = count;
    model.name = name;
    model.avrRadius = sqrt(width * height / 4);
    model.maxRadius = model.avrRadius;

    Surface surf;
    surf.init();

    g_globalRebder.modelName.push_back(new QString(name));

    p1.color = { 180, 180, 180, 1 };
    p1.state = false;
    p1.normalCount = 0;
    p1.pos(0) = -halfW;
    p1.pos(1) = -200;
    p1.pos(2) = -halfH;
    p1.pos(3) = 1;

    p2.color = { 180, 180, 180, 1 };
    p2.state = false;
    p2.normalCount = 0;
    p2.pos(0) = -halfW;
    p2.pos(1) = -200;
    p2.pos(2) = halfH;
    p2.pos(3) = 1;

    p3.color = { 180, 180, 180, 1 };
    p3.state = false;
    p3.normalCount = 0;
    p3.pos(0) = halfW;
    p3.pos(1) = -200;
    p3.pos(2) = halfH;
    p3.pos(3) = 1;

    p4.color = { 180, 180, 180, 1 };
    p4.state = false;
    p4.normalCount = 0;
    p4.pos(0) = halfW;
    p4.pos(1) = -200;
    p4.pos(2) = -halfH;
    p4.pos(3) = 1;

    Triangle t1, t2;  t1.init(); t2.init();
    t1.textureIndex = -1;
    t2.textureIndex = -1;
    t1.modelName = SearchModelNameInSet(name);
    t2.modelName = SearchModelNameInSet(name);

    if(tex.picPath.size() > 0)
    {
        tex.name = QString("Plane Texture %1").arg(count);
        t1.textureIndex = InsertModelTexture(model.textureList, tex);
        t2.textureIndex = t1.textureIndex;

        p1.color = { 1, 1, 1, 1 };
        p2.color = { 1, 1, 1, 1 };
        p3.color = { 1, 1, 1, 1 };
        p4.color = { 1, 1, 1, 1 };
    }

    t1.localv1 = SearchPointInSet(model.localPointList, p1);
    t1.localv2 = SearchPointInSet(model.localPointList, p2);
    t1.localv3 = SearchPointInSet(model.localPointList, p4);
    t1.v1 = t1.localv1;
    t1.v2 = t1.localv2;
    t1.v3 = t1.localv3;
    t1.texV1 = {0, 1};
    t1.texV2 = {0, 0};
    t1.texV3 = {1, 1};

    t2.localv1 = SearchPointInSet(model.localPointList, p2);
    t2.localv2 = SearchPointInSet(model.localPointList, p3);
    t2.localv3 = SearchPointInSet(model.localPointList, p4);
    t2.v1 = t2.localv1;
    t2.v2 = t2.localv2;
    t2.v3 = t2.localv3;
    t2.texV1 = {0, 0};
    t2.texV2 = {1, 0};
    t2.texV3 = {1, 1};

    surf.triIndex_a = SearchTriangleInSet(model.triangleList, t1);
    surf.triIndex_b = SearchTriangleInSet(model.triangleList, t2);
    model.surfList.push_back(surf);
    model.pointList = model.localPointList;

    return model;
}


void PipeLine::CalculateLight()
{
    Color4f color_v1, color_v2, color_v3;
    color_v1.init(); color_v2.init(); color_v3.init();
    vector<Point> &pointList = g_globalRebder.pointList;
    vector<Triangle> &triangleList  = g_globalRebder.triangleList;
    Texture  texture;  texture.init();
    Vector3f direc, normal;
    float dotLN = 0;

    CreateLight();

    for(int i = 0; i < pointList.size(); i++)
    {
        pointList[i].state = false;
    }

    for(int i = 0; i < triangleList.size(); i++)
    {
        if(triangleList[i].state)
            continue;
        color_v1 = { 0, 0, 0, 1 };
        color_v2 = { 0, 0, 0, 1 };
        color_v3 = { 0, 0, 0, 1 };
        texture = g_globalRebder.textureList[triangleList[i].textureIndex];
        for(int j = 0; j < m_LightNums; j++)
        {
            switch(m_Lights[j].attribute)
            {
            case AMBIENT_LIGHT:
                color_v1.r += texture.ka * m_Lights[j].color.r;
                color_v1.g += texture.ka * m_Lights[j].color.g;
                color_v1.b += texture.ka * m_Lights[j].color.b;

                color_v2 = color_v1;
                color_v3 = color_v1;
                break;

            case DIRECTION_LIGHT:
                direc = -1 * Vector4fTo3f(m_Lights[j].worldDir);
                direc.normalize();
                normal = pointList[triangleList[i].v1].worldNormal;
                normal.normalize();
                dotLN = normal.dot(direc);
                if(dotLN > 0)
                {
                    color_v1.r += texture.kd * m_Lights[j].color.r * dotLN;
                    color_v1.g += texture.kd * m_Lights[j].color.g * dotLN;
                    color_v1.b += texture.kd * m_Lights[j].color.b * dotLN;
                }

                normal = pointList[triangleList[i].v2].worldNormal;
                normal.normalize();
                dotLN = normal.dot(direc);
                if(dotLN > 0)
                {
                    color_v2.r += texture.kd * m_Lights[j].color.r * dotLN;
                    color_v2.g += texture.kd * m_Lights[j].color.g * dotLN;
                    color_v2.b += texture.kd * m_Lights[j].color.b * dotLN;
                }

                normal = pointList[triangleList[i].v3].worldNormal;
                normal.normalize();
                dotLN = normal.dot(direc);
                if(dotLN > 0)
                {
                    color_v3.r += texture.kd * m_Lights[j].color.r * dotLN;
                    color_v3.g += texture.kd * m_Lights[j].color.g * dotLN;
                    color_v3.b += texture.kd * m_Lights[j].color.b * dotLN;
                }
                break;

            default:
                break;
            }
        }

        if(color_v1.r > 1) color_v1.r = 1;
        if(color_v1.g > 1) color_v1.g = 1;
        if(color_v1.b > 1) color_v1.b = 1;
        if(pointList[triangleList[i].v1].state == false)
        {
            pointList[triangleList[i].v1].state = true;
            pointList[triangleList[i].v1].color = color_v1;
        }

        if(color_v2.r > 1) color_v2.r = 1;
        if(color_v2.g > 1) color_v2.g = 1;
        if(color_v2.b > 1) color_v2.b = 1;
        if(pointList[triangleList[i].v2].state == false)
        {
            pointList[triangleList[i].v2].state = true;
            pointList[triangleList[i].v2].color = color_v2;
        }

        if(color_v3.r > 1) color_v3.r = 1;
        if(color_v3.g > 1) color_v3.g = 1;
        if(color_v3.b > 1) color_v3.b = 1;
        if(pointList[triangleList[i].v3].state == false)
        {
            pointList[triangleList[i].v3].state = true;
            pointList[triangleList[i].v3].color = color_v3;
        }
    }
}

void PipeLine::CreateLight()
{
    Light light;  light.init();
    light.color = { 0.3f, 0.3f, 0.3f, 1.0f };
    light.id = LIGHT0;
    light.state = LIGHT_ON;
    light.attribute = AMBIENT_LIGHT;
    AddLight(light);

    light.color = { 0.8f, 0.8f, 0.8f, 1.0f };
    light.id = LIGHT1;
    light.state = LIGHT_ON;
    light.attribute = DIRECTION_LIGHT;
    light.worldDir = Vector4f( -1, -1, -1, 1.0 );
    AddLight(light);
}

void PipeLine::AddLight(Light& light)
{
    if(light.id > LIGHT7)
        return;
    for(int i = 0; i < m_LightNums; i++)
    {
        if(m_Lights[m_LightNums].id == light.id)
        {
            m_Lights[m_LightNums] = light;
            return;
        }
    }
    light.id = LightAttribute(m_LightNums);
    m_Lights[m_LightNums] = light;

    if(m_LightNums <= LIGHT7)
        m_LightNums++;
}

void PipeLine::RotateSpereY(int angle)
{
    m_SphereRotateY += angle;
    m_SphereRotateY %= 360;
    ClearGloabalRender();
    CreateModels();
}

QString* PipeLine::SearchModelNameInSet(QString name)
{
    for(int i = 0; i < g_globalRebder.modelName.size(); i++)
    {
        if(*g_globalRebder.modelName[i] == name)
            return g_globalRebder.modelName[i];
    }
    return nullptr;
}

void PipeLine::CreateModels()
{
    Vector4f tempVec;
    float tempZ = 1;
    Model model;
    model.init();
    Texture tex;

    m_Matrix.PushMatrix();
    m_Matrix.MoveTo(Vector4f(0, -200, -500, 1));
    tex = CreateTexture(g_globalRebder.imgPath[0], 0.35f, 0.55f, 0.3f);
    model = CreatePlane(900, 600, tex, QString("Ground"));
    for(int j = 0; j < model.pointList.size(); j++)
    {
        tempVec = m_Matrix.LocaltoWorld(model.pointList[j].pos);
        model.pointList[j].pos = tempVec;
    }
    m_ModelList.push_back(model);
    m_Matrix.PopMatrix();

    m_Matrix.PushMatrix();
    m_Matrix.MoveTo(Vector4f(-200, -200 , -500, 1));
    //m_Matrix.RotateY(30);
    //m_Matrix.RotateAxis(Vector4f(1, 1, 1, 0), 50);
    tex = CreateTexture(g_globalRebder.imgPath[2], 0.15f, 0.85f, 0.3f);
    model = CreateCube(200, tex, QString("Container"));
    for(int j = 0; j < model.pointList.size(); j++)
    {
        tempVec = m_Matrix.LocaltoWorld(model.pointList[j].pos);
        model.pointList[j].pos = tempVec;
    }
    m_ModelList.push_back(model);
    m_Matrix.PopMatrix();

    m_Matrix.PushMatrix();
    m_Matrix.MoveTo(Vector4f(200, -200, -400, 1));
    //m_Matrix.RotateX(50);
    //m_Matrix.RotateY(50);
    //m_Matrix.RotateAxis(Vector4f(1, -1, 1, 1) ,50);
    tex = CreateTexture(g_globalRebder.imgPath[3], 0.5f, 0.35f, 0.6f);
    model = CreateCube(200, tex, QString("Wooden Crated"));
    for(int j = 0; j < model.pointList.size(); j++)
    {
        tempVec = m_Matrix.LocaltoWorld(model.pointList[j].pos);
        model.pointList[j].pos = tempVec;
    }
    m_ModelList.push_back(model);
    m_Matrix.PopMatrix();

    m_Matrix.PushMatrix();
    m_Matrix.MoveTo(Vector4f(0, 100, -300, 1));
    //m_Matrix.RotateAxis(Vector4f(1, 1, 1, 1) ,-70);
    m_Matrix.RotateX(30);
    m_Matrix.RotateY(m_SphereRotateY);
    tex = CreateTexture(g_globalRebder.imgPath[1], 0.15f, 0.75f, 0.3f);
    model = CreateSpere(20, 20, 150, tex, QString("Earth"));
    for(int j = 0; j < model.pointList.size(); j++)
    {
        tempVec = m_Matrix.LocaltoWorld(model.pointList[j].pos);
        model.pointList[j].pos = tempVec;
    }
    m_ModelList.push_back(model);
    m_Matrix.PopMatrix();

    for(int i = 0; i < m_ModelList.size(); i++)
    {
        CalculateWorldNormal(m_ModelList[i]);
    }

    BackfaceCulling();

    CalculateLight();

    for(int i = 0; i < g_globalRebder.pointList.size(); i++)
    {
        g_globalRebder.pointList[i].state = false;
    }
    for(int i = 0; i < g_globalRebder.triangleList.size(); i++)
    {
        g_globalRebder.triangleList[i].state = false;
    }
    for(int i = 0; i < g_globalRebder.triangleList.size(); i++)
    {
        if(g_globalRebder.pointList[g_globalRebder.triangleList[i].v1].state == false)
        {
            tempVec = m_Matrix.WorldtoEye(g_globalRebder.pointList[g_globalRebder.triangleList[i].v1].pos);
            g_globalRebder.pointList[g_globalRebder.triangleList[i].v1].pos = tempVec;
            g_globalRebder.pointList[g_globalRebder.triangleList[i].v1].state = true;
        }
        if(g_globalRebder.pointList[g_globalRebder.triangleList[i].v2].state == false)
        {
            tempVec = m_Matrix.WorldtoEye(g_globalRebder.pointList[g_globalRebder.triangleList[i].v2].pos);
            g_globalRebder.pointList[g_globalRebder.triangleList[i].v2].pos = tempVec;
            g_globalRebder.pointList[g_globalRebder.triangleList[i].v2].state = true;
        }
        if(g_globalRebder.pointList[g_globalRebder.triangleList[i].v3].state == false)
        {
            tempVec = m_Matrix.WorldtoEye(g_globalRebder.pointList[g_globalRebder.triangleList[i].v3].pos);
            g_globalRebder.pointList[g_globalRebder.triangleList[i].v3].pos = tempVec;
            g_globalRebder.pointList[g_globalRebder.triangleList[i].v3].state = true;
        }
    }

    m_Camera.camPos = Vector4f(0, 0, 0, 1);

    FrustumsCulling();

    for(int i = 0; i < g_globalRebder.pointList.size(); i++)
    {
        g_globalRebder.pointList[i].state = false;
    }
    for(int i = 0; i < g_globalRebder.triangleList.size(); i++)
    {
        if(g_globalRebder.pointList[g_globalRebder.triangleList[i].v1].state == false)
        {
            tempVec = g_globalRebder.pointList[g_globalRebder.triangleList[i].v1].pos;
            tempVec = m_Matrix.EyetoProjec(tempVec);
            tempVec(0) /= tempVec(3);
            tempVec(1) /= tempVec(3);
            tempVec(2) /= tempVec(3);
            tempZ = tempVec(3);
            tempVec(3)  = 1;
            tempVec = m_Matrix.ProjecttoScreen(tempVec);
            tempVec(3) = 1 / tempZ;

            tempVec(0) = (int) tempVec(0);
            tempVec(1) = (int) tempVec(1);
            g_globalRebder.pointList[g_globalRebder.triangleList[i].v1].pos = tempVec;
            g_globalRebder.pointList[g_globalRebder.triangleList[i].v1].state = true;
        }
        if(g_globalRebder.pointList[g_globalRebder.triangleList[i].v2].state == false)
        {
            tempVec = g_globalRebder.pointList[g_globalRebder.triangleList[i].v2].pos;
            tempVec = m_Matrix.EyetoProjec(tempVec);
            tempVec(0) /= tempVec(3);
            tempVec(1) /= tempVec(3);
            tempVec(2) /= tempVec(3);
            tempZ = tempVec(3);
            tempVec(3)  = 1;
            tempVec = m_Matrix.ProjecttoScreen(tempVec);
            tempVec(3) = 1 / tempZ;

            tempVec(0) = (int) tempVec(0);
            tempVec(1) = (int) tempVec(1);
            g_globalRebder.pointList[g_globalRebder.triangleList[i].v2].pos = tempVec;
            g_globalRebder.pointList[g_globalRebder.triangleList[i].v2].state = true;
        }
        if(g_globalRebder.pointList[g_globalRebder.triangleList[i].v3].state == false)
        {
            tempVec = g_globalRebder.pointList[g_globalRebder.triangleList[i].v3].pos;
            tempVec = m_Matrix.EyetoProjec(tempVec);
            tempVec(0) /= tempVec(3);
            tempVec(1) /= tempVec(3);
            tempVec(2) /= tempVec(3);
            tempZ = tempVec(3);
            tempVec(3)  = 1;
            tempVec = m_Matrix.ProjecttoScreen(tempVec);
            tempVec(3) = 1 / tempZ;

            tempVec(0) = (int) tempVec(0);
            tempVec(1) = (int) tempVec(1);
            g_globalRebder.pointList[g_globalRebder.triangleList[i].v3].pos = tempVec;
            g_globalRebder.pointList[g_globalRebder.triangleList[i].v3].state = true;
        }
    }
    m_pControler->RenderTriangles(g_globalRebder);
}

Model PipeLine::CreateCube(int length, Texture tex, QString name)
{
    Triangle tri;  tri.init();
    Surface surf;  surf.init();
    static int count = 0;  count ++;
    float halfLen = length / 2;
    Point p1, p2, p3, p4, p5, p6, p7, p8;
    p1.init(); p2.init(); p3.init(); p4.init();
    p5.init(); p6.init(); p7.init(); p8.init();
    Model model;  model.init();
    model.id = count;
    model.name = name;
    model.avrRadius = sqrt(length * halfLen + length * length);
    model.maxRadius = model.avrRadius;

    g_globalRebder.modelName.push_back(new QString(name));

    p1.color = { 180, 180, 180, 1 };
    p1.state = false;
    p1.normalCount = 0;
    p1.pos(0) = -halfLen;
    p1.pos(1) = halfLen;
    p1.pos(2) = -halfLen;
    p1.pos(3) = 1;

    p2.color = { 180, 180, 180, 1 };
    p2.state = false;
    p2.normalCount = 0;
    p2.pos(0) = -halfLen;
    p2.pos(1) = halfLen;
    p2.pos(2) = halfLen;
    p2.pos(3) = 1;

    p3.color = { 180, 180, 180, 1 };
    p3.state = false;
    p3.normalCount = 0;
    p3.pos(0) = halfLen;
    p3.pos(1) = halfLen;
    p3.pos(2) = halfLen;
    p3.pos(3) = 1;

    p4.color = { 180, 180, 180, 1 };
    p4.state = false;
    p4.normalCount = 0;
    p4.pos(0) = halfLen;
    p4.pos(1) = halfLen;
    p4.pos(2) = -halfLen;
    p4.pos(3) = 1;

    p5.color = { 180, 180, 180, 1 };
    p5.state = false;
    p5.normalCount = 0;
    p5.pos(0) = -halfLen;
    p5.pos(1) = -halfLen;
    p5.pos(2) = -halfLen;
    p5.pos(3) = 1;

    p6.color = { 180, 180, 180, 1 };
    p6.state = false;
    p6.normalCount = 0;
    p6.pos(0) = -halfLen;
    p6.pos(1) = -halfLen;
    p6.pos(2) = halfLen;
    p6.pos(3) = 1;

    p7.color = { 180, 180, 180, 1 };
    p7.state = false;
    p7.normalCount = 0;
    p7.pos(0) = halfLen;
    p7.pos(1) = -halfLen;
    p7.pos(2) = halfLen;
    p7.pos(3) = 1;

    p8.color = { 180, 180, 180, 1 };
    p8.state = false;
    p8.normalCount = 0;
    p8.pos(0) = halfLen;
    p8.pos(1) = -halfLen;
    p8.pos(2) = -halfLen;
    p8.pos(3) = 1;

    tri.textureIndex = -1;
    tri.modelName = SearchModelNameInSet(name);

    if(tex.picPath.size() > 0)
    {
        tex.name = QString("Cube Texture %1").arg(count);
        tri.textureIndex = InsertModelTexture(model.textureList, tex);

        p1.color = { 1, 1, 1, 1 };
        p2.color = { 1, 1, 1, 1 };
        p3.color = { 1, 1, 1, 1 };
        p4.color = { 1, 1, 1, 1 };
        p5.color = { 1, 1, 1, 1 };
        p6.color = { 1, 1, 1, 1 };
        p7.color = { 1, 1, 1, 1 };
        p8.color = { 1, 1, 1, 1 };
    }

    //设置left，t1,t2
    tri.localv1 = SearchPointInSet(model.localPointList, p1);
    tri.localv2 = SearchPointInSet(model.localPointList, p5);
    tri.localv3 = SearchPointInSet(model.localPointList, p2);
    tri.v1 = tri.localv1;
    tri.v2 = tri.localv2;
    tri.v3 = tri.localv3;
    tri.texV1 = { 0, 1 };
    tri.texV2 = { 0, 0 };
    tri.texV3 = { 1, 1 };
    surf.triIndex_a = SearchTriangleInSet(model.triangleList, tri);

    tri.localv1 = SearchPointInSet(model.localPointList, p5);
    tri.localv2 = SearchPointInSet(model.localPointList, p6);
    tri.localv3 = SearchPointInSet(model.localPointList, p2);
    tri.v1 = tri.localv1;
    tri.v2 = tri.localv2;
    tri.v3 = tri.localv3;
    tri.texV1 = { 0, 0 };
    tri.texV2 = { 1, 0 };
    tri.texV3 = { 1, 1 };
    surf.triIndex_b = SearchTriangleInSet(model.triangleList, tri);
    model.surfList.push_back(surf);

    //设置right，t1,t2
    tri.localv1 = SearchPointInSet(model.localPointList, p3);
    tri.localv2 = SearchPointInSet(model.localPointList, p7);
    tri.localv3 = SearchPointInSet(model.localPointList, p4);
    tri.v1 = tri.localv1;
    tri.v2 = tri.localv2;
    tri.v3 = tri.localv3;
    tri.texV1 = { 0, 1 };
    tri.texV2 = { 0, 0 };
    tri.texV3 = { 1, 1 };
    surf.triIndex_a = SearchTriangleInSet(model.triangleList, tri);

    tri.localv1 = SearchPointInSet(model.localPointList, p7);
    tri.localv2 = SearchPointInSet(model.localPointList, p8);
    tri.localv3 = SearchPointInSet(model.localPointList, p4);
    tri.v1 = tri.localv1;
    tri.v2 = tri.localv2;
    tri.v3 = tri.localv3;
    tri.texV1 = { 0, 0 };
    tri.texV2 = { 1, 0 };
    tri.texV3 = { 1, 1 };
    surf.triIndex_b = SearchTriangleInSet(model.triangleList, tri);
    model.surfList.push_back(surf);

    //设置down，t1,t2
    tri.localv1 = SearchPointInSet(model.localPointList, p6);
    tri.localv2 = SearchPointInSet(model.localPointList, p5);
    tri.localv3 = SearchPointInSet(model.localPointList, p8);
    tri.v1 = tri.localv1;
    tri.v2 = tri.localv2;
    tri.v3 = tri.localv3;
    tri.texV1 = { 0, 0 };
    tri.texV2 = { 0, 1 };
    tri.texV3 = { 1, 1 };
    surf.triIndex_a = SearchTriangleInSet(model.triangleList, tri);

    tri.localv1 = SearchPointInSet(model.localPointList, p6);
    tri.localv2 = SearchPointInSet(model.localPointList, p8);
    tri.localv3 = SearchPointInSet(model.localPointList, p7);
    tri.v1 = tri.localv1;
    tri.v2 = tri.localv2;
    tri.v3 = tri.localv3;
    tri.texV1 = { 0, 0 };
    tri.texV2 = { 1, 1 };
    tri.texV3 = { 1, 0 };
    surf.triIndex_b = SearchTriangleInSet(model.triangleList, tri);
    model.surfList.push_back(surf);

    //设置up，t1,t2
    tri.localv1 = SearchPointInSet(model.localPointList, p1);
    tri.localv2 = SearchPointInSet(model.localPointList, p2);
    tri.localv3 = SearchPointInSet(model.localPointList, p4);
    tri.v1 = tri.localv1;
    tri.v2 = tri.localv2;
    tri.v3 = tri.localv3;
    tri.texV1 = { 0, 1 };
    tri.texV2 = { 0, 0 };
    tri.texV3 = { 1, 1 };
    surf.triIndex_a = SearchTriangleInSet(model.triangleList, tri);

    tri.localv1 = SearchPointInSet(model.localPointList, p2);
    tri.localv2 = SearchPointInSet(model.localPointList, p3);
    tri.localv3 = SearchPointInSet(model.localPointList, p4);
    tri.v1 = tri.localv1;
    tri.v2 = tri.localv2;
    tri.v3 = tri.localv3;
    tri.texV1 = { 0, 0 };
    tri.texV2 = { 1, 0 };
    tri.texV3 = { 1, 1 };
    surf.triIndex_b = SearchTriangleInSet(model.triangleList, tri);
    model.surfList.push_back(surf);

    //设置front，t1,t2
    tri.localv1 = SearchPointInSet(model.localPointList, p2);
    tri.localv2 = SearchPointInSet(model.localPointList, p6);
    tri.localv3 = SearchPointInSet(model.localPointList, p3);
    tri.v1 = tri.localv1;
    tri.v2 = tri.localv2;
    tri.v3 = tri.localv3;
    tri.texV1 = { 0, 1 };
    tri.texV2 = { 0, 0 };
    tri.texV3 = { 1, 1 };
    surf.triIndex_a = SearchTriangleInSet(model.triangleList, tri);

    tri.localv1 = SearchPointInSet(model.localPointList, p6);
    tri.localv2 = SearchPointInSet(model.localPointList, p7);
    tri.localv3 = SearchPointInSet(model.localPointList, p3);
    tri.v1 = tri.localv1;
    tri.v2 = tri.localv2;
    tri.v3 = tri.localv3;
    tri.texV1 = { 0, 0 };
    tri.texV2 = { 1, 0 };
    tri.texV3 = { 1, 1 };
    surf.triIndex_b = SearchTriangleInSet(model.triangleList, tri);
    model.surfList.push_back(surf);

    //设置back，t1, t2
    tri.localv1 = SearchPointInSet(model.localPointList, p5);
    tri.localv2 = SearchPointInSet(model.localPointList, p1);
    tri.localv3 = SearchPointInSet(model.localPointList, p4);
    tri.v1 = tri.localv1;
    tri.v2 = tri.localv2;
    tri.v3 = tri.localv3;
    tri.texV1 = { 0, 0 };
    tri.texV2 = { 0, 1 };
    tri.texV3 = { 1, 1 };
    surf.triIndex_a = SearchTriangleInSet(model.triangleList, tri);

    tri.localv1 = SearchPointInSet(model.localPointList, p5);
    tri.localv2 = SearchPointInSet(model.localPointList, p4);
    tri.localv3 = SearchPointInSet(model.localPointList, p8);
    tri.v1 = tri.localv1;
    tri.v2 = tri.localv2;
    tri.v3 = tri.localv3;
    tri.texV1 = { 0, 0 };
    tri.texV2 = { 1, 1 };
    tri.texV3 = { 1, 0 };

    surf.triIndex_b = SearchTriangleInSet(model.triangleList, tri);
    model.surfList.push_back(surf);

    model.pointList = model.localPointList;
    return model;
}

Model PipeLine::CreateSpere(int nLatitude, int nParallel, float fRadius, Texture tex, QString name)
{
    static int count = 0;
    Model model;  model.init();
    float L_Step = 2 * PI / nLatitude;
    float P_Step = PI / nParallel;
    float tex_xstep = 1.0f / nLatitude, tex_ystep = 1.0f / nParallel;
    float tex_x = 0, tex_y = 0;
    float P_Y = 0, L_XZ = 0;
    Point p1, p2, p3, p4;  p2.init(); p2.init(); p3.init(); p4.init();
    Triangle tri1;  tri1.init();
    Surface surf;

    g_globalRebder.modelName.push_back(new QString(name));

    count ++;
    model.id = count;
    model.name = name;
    model.avrRadius = fRadius;
    model.maxRadius = fRadius;

    tri1.modelName = SearchModelNameInSet(name);

    p1.color = { 180, 180, 180, 1 };
    p2.color = { 180, 180, 180, 1 };
    p3.color = { 180, 180, 180, 1 };
    p4.color = { 180, 180, 180, 1 };

    if(tex.picPath.size() > 0)
    {
        tex.name = QString("Spere Texture %1").arg(count);
        tri1.textureIndex = InsertModelTexture(model.textureList, tex);

        p1.color = { 1, 1, 1, 1 };
        p2.color = { 1, 1, 1, 1 };
        p3.color = { 1, 1, 1, 1 };
        p4.color = { 1, 1, 1, 1 };
    }

    for(int i = 0; i < nParallel; i++)
    {
        P_Y = i * P_Step;
        tex_y = 1 -  i * tex_ystep;
        for(int j = 0; j < nLatitude; j++)
        {
            surf.init();
            L_XZ = j * L_Step;
            tex_x = j * tex_xstep;
            if(i == 0)
            {
                p1.state = false;
                p1.normalCount = 0;
                p1.pos(0) = 0;
                p1.pos(1) = fRadius;
                p1.pos(2) = 0;
                p1.pos(3) = 1;

                p2.state = false;
                p2.normalCount = 0;
                p2.pos(0) = fRadius * sin(P_Y + P_Step) * sin(L_XZ);
                p2.pos(1) = fRadius * cos(P_Y + P_Step);
                p2.pos(2) = fRadius * sin(P_Y + P_Step) * cos(L_XZ);
                p2.pos(3) = 1;

                p3.state = false;
                p3.normalCount = 0;
                p3.pos(0) = fRadius * sin(P_Y + P_Step) * sin(L_XZ + L_Step);
                p3.pos(1) = fRadius * cos(P_Y + P_Step);
                p3.pos(2) = fRadius * sin(P_Y + P_Step) * cos(L_XZ + L_Step);
                p3.pos(3) = 1;

                tri1.localv1 = SearchPointInSet(model.localPointList, p1);
                tri1.localv2 = SearchPointInSet(model.localPointList, p2);
                tri1.localv3 = SearchPointInSet(model.localPointList, p3);
                tri1.v1 = tri1.localv1;
                tri1.v2 = tri1.localv2;
                tri1.v3 = tri1.localv3;
                tri1.texV1 = {tex_x, tex_y};
                tri1.texV2 = {tex_x, tex_y - tex_ystep};
                tri1.texV3 = {tex_x + tex_xstep, tex_y - tex_ystep};
                surf.triIndex_a = SearchTriangleInSet(model.triangleList, tri1);
                model.surfList.push_back(surf);
            }
            else if(i < nParallel - 1)
            {
                p1.state = false;
                p1.normalCount = 0;
                p1.pos(0) = fRadius * sin(P_Y) * sin(L_XZ);
                p1.pos(1) = fRadius * cos(P_Y);
                p1.pos(2) = fRadius * sin(P_Y) * cos(L_XZ);
                p1.pos(3) = 1;

                p2.state = false;
                p2.normalCount = 0;
                p2.pos(0) = fRadius * sin(P_Y + P_Step) * sin(L_XZ);
                p2.pos(1) = fRadius * cos(P_Y + P_Step);
                p2.pos(2) = fRadius * sin(P_Y + P_Step) * cos(L_XZ);
                p2.pos(3) = 1;

                p3.state = false;
                p3.normalCount = 0;
                p3.pos(0) = fRadius * sin(P_Y + P_Step) * sin(L_XZ + L_Step);
                p3.pos(1) = fRadius * cos(P_Y + P_Step);
                p3.pos(2) = fRadius * sin(P_Y + P_Step) * cos(L_XZ + L_Step);
                p3.pos(3) = 1;

                p4.state = false;
                p4.normalCount = 0;
                p4.pos(0) = fRadius * sin(P_Y) * sin(L_XZ + L_Step);
                p4.pos(1) = fRadius * cos(P_Y);
                p4.pos(2) = fRadius * sin(P_Y) * cos(L_XZ + L_Step);
                p4.pos(3) = 1;

                tri1.localv1 = SearchPointInSet(model.localPointList, p1);
                tri1.localv2 = SearchPointInSet(model.localPointList, p2);
                tri1.localv3 = SearchPointInSet(model.localPointList, p4);
                tri1.v1 = tri1.localv1;
                tri1.v2 = tri1.localv2;
                tri1.v3 = tri1.localv3;
                tri1.texV1 = {tex_x, tex_y};
                tri1.texV2 = {tex_x, tex_y - tex_ystep};
                tri1.texV3 = {tex_x + tex_xstep, tex_y};
                surf.triIndex_a = SearchTriangleInSet(model.triangleList, tri1);

                tri1.localv1 = SearchPointInSet(model.localPointList, p2);
                tri1.localv2 = SearchPointInSet(model.localPointList, p3);
                tri1.localv3 = SearchPointInSet(model.localPointList, p4);
                tri1.v1 = tri1.localv1;
                tri1.v2 = tri1.localv2;
                tri1.v3 = tri1.localv3;
                tri1.texV1 = {tex_x, tex_y - tex_ystep};
                tri1.texV2 = {tex_x + tex_xstep, tex_y - tex_ystep};
                tri1.texV3 = {tex_x + tex_xstep, tex_y};
                surf.triIndex_b = SearchTriangleInSet(model.triangleList, tri1);
                model.surfList.push_back(surf);
            }
            else
            {
                p1.state = false;
                p1.normalCount = 0;
                p1.pos(0) = fRadius * sin(P_Y) * sin(L_XZ);
                p1.pos(1) = fRadius * cos(P_Y);
                p1.pos(2) = fRadius * sin(P_Y) * cos(L_XZ);
                p1.pos(3) = 1;

                p2.state = false;
                p2.normalCount = 0;
                p2.pos(0) = 0;
                p2.pos(1) = -fRadius;
                p2.pos(2) = 0;
                p2.pos(3) = 1;

                p3.state = false;
                p3.normalCount = 0;
                p3.pos(0) = fRadius * sin(P_Y) * sin(L_XZ + L_Step);
                p3.pos(1) = fRadius * cos(P_Y);
                p3.pos(2) = fRadius * sin(P_Y) * cos(L_XZ + L_Step);
                p3.pos(3) = 1;

                tri1.localv1 = SearchPointInSet(model.localPointList, p1);
                tri1.localv2 = SearchPointInSet(model.localPointList, p2);
                tri1.localv3 = SearchPointInSet(model.localPointList, p3);
                tri1.v1 = tri1.localv1;
                tri1.v2 = tri1.localv2;
                tri1.v3 = tri1.localv3;
                tri1.texV1 = {tex_x, tex_y};
                tri1.texV2 = {tex_x, tex_y - tex_ystep};
                tri1.texV3 = {tex_x + tex_xstep, tex_y};
                surf.triIndex_a = SearchTriangleInSet(model.triangleList, tri1);
                model.surfList.push_back(surf);
            }
        }
    }
     model.pointList = model.localPointList;
     return model;
}
