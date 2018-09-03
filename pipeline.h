#ifndef PIPELINE_H
#define PIPELINE_H


#include <vector>
#include "basicelement.h"

using namespace std;

class Controller;

class PipeLine
{
public:
    PipeLine();
    ~PipeLine();
    void  SetControler(Controller*);
    void  RebuildScene();
    void  CreateModels();
    Model CreateSpere(int nLatitude, int nParallel, float fRadius, Texture tex, QString name);
    Model CreateCube(int length, Texture tex, QString name);
    Model CreatePlane(int width, int height, Texture tex, QString name);

    int SearchPointInSet(vector<Point>& pointSet, Point& p);
    int SearchTriangleInSet(vector<Triangle>& triangleList, Triangle& tri);
    int SearchEyeTriangleInSet(vector<Triangle>& triangleList, Triangle& tri);
    QString* SearchModelNameInSet(QString name);

    void CalculateLight();

    inline GloabalRender& getGLobalRender() {return g_globalRebder;}
    void RotateSpereY(int angle = 15);
    int  GeSelectedScreenTri(QPoint pos);

private:
    Texture CreateTexture(QString imgPath, float ka, float kd, float ks);
    int  InsertModelTexture(vector<Texture>& triangleList, Texture& tex);
    void BackfaceCulling();
    void FrustumsCulling();
    void GetInsecValue(Point& p1, Point& p2,  float t, float z, TextureCoord& tex1, TextureCoord& tex2, Vector4f& v, Color4f& color, TextureCoord& tex);
    void CalculateWorldNormal(Model& model);
    void CalculateNormal(Model& model, Triangle& triangle, vector<Point>& pointList);
    Vector3f  Vector4fTo3f(const Vector4f& vec4f);
    Vector4f  Vector3fTo4f(const Vector3f& vec3f);
    void  CreateLight();
    void  AddLight(Light& light);
    int   FindSurfaceByTri(Model& model, int index);
    int   FindPointNotIntriSrc(Model& model, int triSrc, int triDst);
    inline int   GetAnotherSurfTri(Surface surf, int triIndex)
    {
        return surf.triIndex_a == triIndex ? surf.triIndex_b : surf.triIndex_a;
    }
    inline void  ClearGloabalRender()
    {
        g_globalRebder.pointList.clear();
        g_globalRebder.textureList.clear();
        g_globalRebder.triangleList.clear();
    }

private:
    int   m_SphereRotateY;
    Mat4D m_Matrix;
    Controller *m_pControler;
    vector<Model> m_ModelList;
    Camera m_Camera;
    static int m_LightNums;
    Light  m_Lights[8];
    GloabalRender g_globalRebder;
};

#endif // PIPELINE_H
