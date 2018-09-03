#include "Matrix.h"

Mat4D::Mat4D()
{
    m_worldMat = m_worldMat.Identity(4, 4);
    m_eyeMat = m_eyeMat.Identity(4, 4);
    m_projMat = m_projMat.Identity(4, 4);
    m_screenMat = m_screenMat.Identity(4, 4);
}

float  Mat4D::Distance(Vector3f& pos)
{
    return sqrt(pos(0) * pos(0) + pos(1) * pos(1) + pos(2) * pos(2));
}

Matrix4f Mat4D::getWorldMatrix()
{
    return m_worldMat;
}

void Mat4D::setWorldMatrix(Matrix4f& mat)
{
    m_worldMat = mat;
}

Matrix4f Mat4D::getEyeMatrix()
{
    return m_eyeMat;
}

void  Mat4D::setEyeMatrix(Matrix4f& mat)
{
    m_eyeMat = mat;
}

void Mat4D::setProjectMatrix(Matrix4f& mat)
{
    m_projMat = mat;
}

Matrix4f Mat4D::getProjectMatrix()
{
    return m_projMat;
}


void Mat4D::setSreenMatrix(Matrix4f& mat)
{
    m_screenMat = mat;
}

Matrix4f Mat4D::getScreenMatrix()
{
    return m_screenMat;
}

void Mat4D::PushMatrix()
{
    m_MatStack.push(m_worldMat);
}

Matrix4f Mat4D::PopMatrix()
{
    if(m_MatStack.empty())
    {
        return m_worldMat.Identity(4, 4);
    }
    m_worldMat = m_MatStack.top();
    m_MatStack.pop();
    return m_worldMat;
}

Vector4f  Mat4D::LocaltoWorld(Vector4f& vec)
{
    return m_worldMat * vec;
}

Vector4f Mat4D::WorldtoEye(Vector4f& vec)
{
    return m_eyeMat * vec;
}


Vector4f Mat4D::EyetoProjec(Vector4f& vec)
{
    return m_projMat * vec;
}

Vector4f Mat4D::ProjecttoScreen(Vector4f& vec)
{
    return m_screenMat * vec;
}


Matrix4f Mat4D::Scale(float sx =1 , float sy = 1, float sz = 1)
{
    Matrix4f temp = Matrix4f::Identity(4, 4);
    temp(0, 0) = sx;
    temp(1, 1) = sy;
    temp(2, 2) = sz;

    m_worldMat *= temp;

    return m_worldMat;
}

Matrix4f Mat4D::MoveTo(Vector4f moveVec)
{
    Matrix4f temp = Matrix4f::Identity(4, 4);
    temp(0, 3) = moveVec(0);
    temp(1, 3) = moveVec(1);
    temp(2, 3) = moveVec(2);

    m_worldMat *= temp;

    return m_worldMat;
}

Matrix4f Mat4D::RotateX(float angel)
{
    float radian = angel / 180.0f * PI;
    Matrix4f temp = Matrix4f::Identity(4, 4);
    temp(1, 1) = cos(radian);
    temp(1, 2) = -sin(radian);
    temp(2, 1) = sin(radian);
    temp(2, 2) = cos(radian);

    m_worldMat *= temp;

    return m_worldMat;
}

Matrix4f Mat4D::RotateY(float angel)
{
    float radian = angel / 180.0f * PI;
    Matrix4f temp = Matrix4f::Identity(4, 4);
    temp(0, 0) = cos(radian);
    temp(0, 2) = sin(radian);
    temp(2, 0) = -sin(radian);
    temp(2, 2) = cos(radian);

    m_worldMat *= temp;

    return m_worldMat;
}


Matrix4f Mat4D::RotateZ(float angel)
{
    float radian = angel / 180.0f * PI;
    Matrix4f temp = Matrix4f::Identity(4, 4);
    temp(0, 0) = cos(radian);
    temp(0, 1) = -sin(radian);
    temp(1, 0) = sin(radian);
    temp(1, 1) = cos(radian);

    m_worldMat *= temp;

    return m_worldMat;
}


Matrix4f Mat4D::RotateAxis(Vector4f & axisVector, float angel)
{
    float radian = angel / 180.0f * PI;
    Matrix4f temp = Matrix4f::Identity(4, 4);
    float cosA = 1 - cos(radian);
    float cosB = cos(radian);
    float sinB = sin(radian);
    float mul01 = axisVector(0) * axisVector(1);
    float mul02 = axisVector(0) * axisVector(2);
    float mul12 = axisVector(1) * axisVector(2);

    temp(0, 0) = axisVector(0) * axisVector(0) * cosA + cosB;
    temp(0, 1) = mul01 * cosA - axisVector(2) * sinB;
    temp(0, 2) = mul02 * cosA + axisVector(1) * sinB;

    temp(1, 0) = mul01 * cosA + axisVector(2) * sinB;
    temp(1, 1) = axisVector(1) * axisVector(1) * cosA + cosB;
    temp(1, 2) = mul12 * cosA - axisVector(0) * sinB;

    temp(2, 0) = mul02 * cosA - axisVector(1) * sinB;
    temp(2, 1) = mul12 * cosA + axisVector(0) * sinB;
    temp(2, 2) = axisVector(2) * axisVector(2) * cosA + cosB;

    m_worldMat *= temp;

    return m_worldMat;
}

Matrix4f Mat4D::LookAt(Vector4f & upVector, Vector4f & eye, Vector4f & pos)
{
    Matrix4f temp = Matrix4f::Identity(4, 4), trans = Matrix4f::Identity(4, 4);

    upVector.normalize();

    Vector3f direct(eye(0) - pos(0), eye(1) - pos(1), eye(2) - pos(2));
    direct.normalize();

    Vector3f tempUp;
    tempUp(0) = upVector(0);
    tempUp(1) = upVector(1);
    tempUp(2) = upVector(2);

    Vector3f right = tempUp.cross(direct);
    right.normalize();

    Vector3f normal = direct.cross(right);
    normal.normalize();

    temp(0, 0) = right(0);  temp(0, 1) = right(1);  temp(0, 2) = right(2);
    temp(1, 0) = normal(0); temp(1, 1) = normal(1); temp(1, 2) = normal(2);
    temp(2, 0) = direct(0); temp(2, 1) = direct(1); temp(2, 2) = direct(2);


    /*temp(0, 0) = right(0);  temp(0, 1) = normal(0);  temp(0, 2) = direct(0);
    temp(1, 0) = right(1);  temp(1, 1) = normal(1);  temp(1, 2) = direct(1);
    temp(2, 0) = right(2);  temp(2, 1) = normal(2);  temp(2, 2) = direct(2);*/


    trans(0, 3) = -eye(0);
    trans(1, 3) = -eye(1);
    trans(2, 3) = -eye(2);

    m_eyeMat =  temp * trans;

    return m_eyeMat;
}

Matrix4f Mat4D::Ortho(Vector4f & bottomleftnear, Vector4f & toprightfar)
{
    Matrix4f temp = Matrix4f::Identity(4, 4);

    temp(0, 0) = 2 / (toprightfar(0) - bottomleftnear(0));
    temp(0, 3) = -(toprightfar(0) + bottomleftnear(0)) / (toprightfar(0) - bottomleftnear(0));
    temp(1, 1) = 2 / (toprightfar(1) - bottomleftnear(1));
    temp(1, 3) = -(toprightfar(1) + bottomleftnear(1)) / (toprightfar(1) - bottomleftnear(1));
    temp(2, 2) = -2 / (toprightfar(2) - bottomleftnear(2));
    temp(2, 3) = (toprightfar(2) + bottomleftnear(2)) / (toprightfar(2) - bottomleftnear(2));

    m_projMat = temp;

    return temp;
}

Matrix4f Mat4D::Perspective(float fov, float aspect, float znear, float zfar)
{
    float radian = (fov / 180.0f * PI) / 2;
    Matrix4f temp = Matrix4f::Identity(4, 4);
    temp(0, 0) = 1/ (tan(radian) * aspect);
    temp(1, 1) = 1 / tan(radian);
    temp(2, 2) = (znear + zfar) / (znear - zfar);
    temp(2, 3) = -(2 * znear * zfar) / (znear - zfar);
    temp(3, 2) = -1;
    temp(3, 3) = 0;

    m_projMat = temp;

    return temp;
}

Matrix4f Mat4D::Viewport(Vector4f bottomleftnear, Vector4f toprightfar)
{
    Matrix4f temp = Matrix4f::Identity(4, 4);
    temp(0, 0) = (toprightfar(0) - bottomleftnear(0)) / 2;
    temp(0, 3) = (toprightfar(0) + bottomleftnear(0)) / 2;
    temp(1, 1) = (toprightfar(1) - bottomleftnear(1)) / 2;
    temp(1, 3) = (toprightfar(1) + bottomleftnear(1)) / 2;
    temp(2, 2) = 0.5;
    temp(2, 3) = 0.5;

    m_screenMat = temp;

    return temp;
}
