#pragma once

#define Min(a, b)           ((a<b)?a:b)
#define Max(a, b)           ((a>b)?a:b)
#define Clamp(a, val, b)    (Max(a, Min(val, b)))
#define Abs(a)				((a<0)?-a:a)
#define Radians(x)          (2.0f*3.1415f*x/360.f)

template <typename T>
struct Vec3{
    T x,y,z;
};

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;


template <typename T>
struct Vec2{
    T x,y;
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

template <typename T>
struct Vec4{
    T x,y,z,w;
};

using Vec4f = Vec4<float>;
using Vec4i = Vec4<int>;





float Q_rsqrt(float number);
float fastSqRoot(float number);
bool nearlyEqual(float, float);


// vec2
Vec2f operator+(Vec2f, Vec2f);
Vec2f operator-(Vec2f, Vec2f);
Vec2f operator*(Vec2f, float);
Vec2f operator*(float, Vec2f);
Vec2f operator/(Vec2f, float);
Vec2f operator-(Vec2f);
void operator+=(Vec2f&, Vec2f);
void operator-=(Vec2f&, Vec2f);
void operator*=(Vec2f&, float);
void operator/=(Vec2f&, float);
bool operator==(Vec2f, Vec2f);
bool operator!=(Vec2f, Vec2f);

float lenSquared(Vec2f);
float len(Vec2f);

float dotProduct(Vec2f, Vec2f);
float crossMag(Vec2f, Vec2f);

Vec2f normalize(Vec2f);
float projection(Vec2f of, Vec2f on);


// vec3
Vec3f operator+(Vec3f, Vec3f);
Vec3f operator-(Vec3f, Vec3f);
Vec3f operator*(Vec3f, float);
Vec3f operator*(float, Vec3f);
Vec3f operator/(Vec3f, float);
Vec3f operator-(Vec3f);
void operator+=(Vec3f&, Vec3f);
void operator-=(Vec3f&, Vec3f);
void operator*=(Vec3f&, float);
void operator/=(Vec3f&, float);
bool operator==(Vec3f, Vec3f);
bool operator!=(Vec3f, Vec3f);

float lenSquared(Vec3f);
float len(Vec3f);

float dotProduct(Vec3f, Vec3f);
Vec3f crossProduct(Vec3f, Vec3f);

Vec3f normalize(Vec3f);
float projection(Vec3f of, Vec3f on);




template <typename T>
T lerp(T a,T b, float t){
    return a+(b-a)*t;
}



template <int m, int n>
struct Mat{
    float arr[m][n];
};

using Mat3 = Mat<3,3>;
using Mat4 = Mat<4,4>;



template <int p, int q, int r>
Mat<p,r> operator*(Mat<p,q> a, Mat <q,r> b){
    Mat<p,r> m;
    for(int i=0;i<p;i++){
        for(int j=0; j<r;j++){
            m.arr[i][j] = 0;
            for(int k=0; k<q;k++){
                m.arr[i][j] += a.arr[i][k] * b.arr[k][j];
            }
        }
    }
    return m;
}

template <int p, int q>
Mat<q,p> transpose(Mat<p,q> a){
    Mat<q,p> b;
    for(int i=0;i<p;i++){
        for(int j=0; j<q;j++){
            b.arr[j][i] = a.arr[i][j];
        }
    }
    return(b);
}

Vec3f operator *(Mat3 m, Vec3f a);
Vec4f operator *(Mat4 m, Vec4f a);

Mat4 scaleAboutOrigin(float sx, float sy, float sz);
Mat4 translate(float tx, float ty, float tz);

