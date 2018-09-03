#ifndef BASICELEMENT_H
#define BASICELEMENT_H

#include <QImage>
#include <vector>
#include "Matrix.h"
#include "E:\\eigen-eigen-26667be4f70b\\Eigen\\Dense"


using namespace std;
using namespace Eigen;


#define PI 3.1415926f

//多边形环绕方向
enum Surround { CCW, CW  };

enum LightAttribute {
    LIGHT0 = 0,
    LIGHT1,
    LIGHT2,
    LIGHT3,
    LIGHT4,           //光源序号
    LIGHT5,
    LIGHT6,
    LIGHT7,

    AMBIENT_LIGHT,
    POINT_LIGHT,      //光源类型
    DIRECTION_LIGHT,
    SPOT_LIGHT,

    LIGHT_ON,         //光源状态
    LIGHT_OFF
};


struct Color4f
{
    float r, g, b, a;  //颜色
    void init() {
        r = 1;
        g = 1;
        b = 1;
        a = 1;
    }
};

struct TextureCoord
{
    float u, v;  //纹理坐标
    void init() {
        u = 0;
        v = 0;
    }
};

struct Point
{
    bool state;      //状态
    int nNum;        //序数
    int normalCount; //法线总数
    Color4f color;   //颜色
    Vector3f worldNormal;   //法线世界坐标
    Vector3f eyeNormal;     //法线眼坐标
    Vector4f pos;           //坐标

    void init() {
        state = 0;
        nNum = 0;
        normalCount = 0;
        worldNormal = Vector3f(0, 0, 0);
        eyeNormal  = Vector3f(0, 0, 0);
    }
};

struct Camera
{
    Vector4f camPos;    //摄像机位置
    Vector4f camDir;    //相机方向

    Vector4f right;    //右方
    Vector4f forward;  //前方
    Vector4f up;       //上方

    Vector4f nearBottomLeft;  // 近左下角
    Vector4f farTopRight;     //远右上角

    float zNear;        //近裁剪面
    float zFar;         //远裁剪面

    float camDist;      //视距

    float theta;        //视场垂直角

    float aspect;       //宽高比

    float viewWidth;    //视口宽度
    float viewHeight;   //视口高度

    void init() {
        zNear = -100;
        zFar = -900;
        camDist = 100;
        theta = 90;
        aspect = 0.75;
        viewWidth = 800;
        viewHeight = 600;
    }
};


struct Edge
{
    int state;        //状态
    int nNum;         //序数
    Color4f   color;  //颜色
    int localv1, localv2;     //初始顶点索引
    int v1, v2;       //变换顶点索引
    int tri1, tri2;   //三角形索引

    void init() {
        state = 0;
        nNum = 0;
        localv1 = 0;
        localv2 = 0;
        v1 = 0;
        v2 = 0;
        tri1 = 0;
        tri2 = 0;
    }
};


struct Triangle
{
    int state;     //状态
    int nNum;      //序数
    QString* modelName;
    TextureCoord texV1, texV2, texV3;  //纹理坐标
    Vector3f worldNormal;              //法线坐标
    Vector3f eyeNormal;
    int  textureIndex;                 //三角形纹理索引
    int  localv1, localv2, localv3;    //初始顶点索引
    int  v1, v2, v3;                   //变换顶点索引
    int locale1, locale2, locale3;     //初始边索引
    int e1, e2, e3;      //变换边索引
    Surround surround;   //环绕方式

    void init() {
        modelName = nullptr;
        state = 0;
        nNum = 0;
        textureIndex = 0;
        localv1 = 0;
        localv2 = 0;
        localv3 = 0;
        v1 = 0;
        v2 = 0;
        v3 = 0;
        locale1 = 0;
        locale2 = 0;
        locale3 = 0;
        e1 = 0;
        e2 = 0;
        e3 = 0;
        surround = CCW;
        worldNormal = Vector3f(0, 0, 0);
        eyeNormal = Vector3f(0, 0, 0);
    }
};


struct  Light
{
    LightAttribute state;     //光源状态
    LightAttribute id;        //光源id
    LightAttribute attribute; //光源类型
    Color4f color;            //环境光强度
    Vector4f worldPos;   //世界坐标光源位置
    Vector4f eyePos;     //眼坐标光源位置
    Vector4f worldDir;   //世界坐标光源方向
    Vector4f eyeDir;     //眼坐标光源方向
    float kc, kl, kq;  //衰减因子
    float spotInner;   //聚光灯内锥角
    float spotOuter;   //聚光灯外锥角
    float pf;          //聚光灯指数因子

    void init() {
        kc = 0;
        kl = 0;
        kq = 0;
        spotInner = 45;
        spotOuter = 60;
        pf = 1.0;
    }
};


struct Texture
{
    int state;           //材质的状态
    int id;              //材质的id
    QString name;        //材质名称
    QString picPath;     //纹理路径
    int attr;            //属性
    float ka, kd, ks;    //光源反射系数
    QImage image;        //纹理

    void init() {
        state = 0;
        id = 0;
        attr = 0;
        ka = 0.5;
        kd = 0.75;
        ks = 0.5;
    }
};

struct Surface {
    bool bHandle;
    bool bSelected;
    int  triIndex_a, triIndex_b;
    void init() {
        bHandle = false;
        bSelected = false;
        triIndex_a = -1;
        triIndex_b = -1;
    }
};

struct Model
{
    int id;         //物体id
    QString name;   //名称
    bool state;     //物体状态
    float avrRadius;   //物体平均半径
    float maxRadius;   //物体最大半径
    Matrix4f modelMatrix, viewMatrix; //存储模型视图矩阵
    vector<Point> localPointList;     //局部坐标顶点列表
    vector<Point> pointList;          //用于顶点坐标列表
    vector<Triangle> triangleList;    //多边形列表
    vector<Surface> surfList;         //表面列表
    vector<Texture> textureList;      //材质列表

    void init() {
        id = 0;
        state = 0;
        avrRadius = 0;
        maxRadius = 0;
    }
};

struct GloabalRender
{
    vector<Point>    pointList;     //全局顶点渲染队列
    vector<Triangle> triangleList;  //全局三角形渲染队列
    vector<Texture>  textureList;   //全局纹理渲染队列
    vector<QString>  imgPath;       //全局纹理资源路径
    vector<QString*>  modelName;    //模型名字
};

#endif // BASICELEMENT_H
