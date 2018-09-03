#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>
#include <stack>
#include "E:\\eigen-eigen-26667be4f70b\\Eigen\\Dense"

using namespace Eigen;
using namespace std;

#define PI 3.1415926f

class Mat4D
{

public:
    Mat4D();

    void     setWorldMatrix(Matrix4f& mat);
    Matrix4f getWorldMatrix();

    Matrix4f getEyeMatrix();
    void     setEyeMatrix(Matrix4f& mat);

    void     setProjectMatrix(Matrix4f& mat);
    Matrix4f getProjectMatrix();

    void     setSreenMatrix(Matrix4f& mat);
    Matrix4f getScreenMatrix();

    void     PushMatrix();
    Matrix4f PopMatrix();

    Vector4f     LocaltoWorld(Vector4f& vec);
    Vector4f     WorldtoEye(Vector4f& vec);
    Vector4f     EyetoProjec(Vector4f& vec);
    Vector4f     ProjecttoScreen(Vector4f& vec);

    Matrix4f Scale(float sx, float sy, float sz);
    Matrix4f MoveTo(Vector4f moveVec);
    Matrix4f RotateX(float angel);
    Matrix4f RotateY(float angel);
    Matrix4f RotateZ(float angel);
    Matrix4f RotateAxis(Vector4f& axisVector, float angel);
    Matrix4f LookAt(Vector4f& upVector, Vector4f& eye, Vector4f& pos);
    Matrix4f Ortho(Vector4f& bottomleftnear, Vector4f& toprightfar);
    Matrix4f Perspective(float fov, float aspect, float znear, float zfar);
    Matrix4f Viewport(Vector4f bottomleftnear, Vector4f toprightfar);
    float    Distance(Vector3f& pos);

private:
    Matrix4f m_worldMat;
    Matrix4f m_eyeMat;     //眼坐标矩阵
    Matrix4f m_projMat;    //投影矩阵
    Matrix4f m_screenMat;  //屏幕转换矩阵
    stack<Matrix4f> m_MatStack;
};

#endif // MATRIX_H
