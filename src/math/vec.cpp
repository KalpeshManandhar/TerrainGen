#include "vec.h"
#include <math.h>

bool nearlyEqual(float a, float b) {
    return(Abs(a - b) < 0.00005);
}

Vec2f operator+(Vec2f a, Vec2f b){
    return(Vec2f{a.x + b.x, a.y + b.y});
}
Vec2f operator-(Vec2f a, Vec2f b){
    return(Vec2f{a.x - b.x, a.y - b.y});
}
Vec2f operator*(Vec2f a, float b){
    return(Vec2f{a.x * b, a.y *b});
}
Vec2f operator*(float b, Vec2f a){
    return(Vec2f{a.x * b, a.y *b});
}
Vec2f operator/(Vec2f a, float b){
    return(Vec2f{a.x/b, a.y/b});
}
Vec2f operator-(Vec2f a){
    return(Vec2f{-a.x, -a.y});
}
void operator+=(Vec2f& a, Vec2f b){
    a = a+b;
}
void operator-=(Vec2f& a, Vec2f b){
    a = a-b;
}
void operator*=(Vec2f& a, float b){
    a = a*b;
}
void operator/=(Vec2f& a, float b){
    a = a/b;
}
bool operator==(Vec2f a, Vec2f b) {
    return(nearlyEqual(a.x,b.x) && nearlyEqual(a.y,b.y));
}
bool operator!=(Vec2f a, Vec2f b) {
    return(!nearlyEqual(a.x, b.x) || !nearlyEqual(a.y, b.y));
}

float lenSquared(Vec2f a){
    return(a.x * a.x + a.y * a.y);
}
float len(Vec2f a){
    return(sqrt(lenSquared(a)));
}

float dotProduct(Vec2f a, Vec2f b){
    return(a.x * b.x + a.y * b.y);
}

// clockwise = +ve; and anticlockwise = -ve
float crossMag(Vec2f a, Vec2f b){
    return(a.x * b.y - b.x * a.y);
}

Vec2f normalize(Vec2f a){
    return(a*Q_rsqrt(lenSquared(a)));
}
float projection(Vec2f of, Vec2f on){
    return(dotProduct(of, normalize(on)));
}




// vec3f
Vec3f operator+(Vec3f a, Vec3f b){
    return(Vec3f{a.x + b.x, a.y + b.y,a.z + b.z});
}
Vec3f operator-(Vec3f a, Vec3f b){
    return(Vec3f{a.x - b.x, a.y - b.y, a.z-b.z});
}
Vec3f operator*(Vec3f a, float b){
    return(Vec3f{a.x * b, a.y *b,a.z*b});
}
Vec3f operator*(float b, Vec3f a){
    return(Vec3f{a.x * b, a.y *b, a.z*b});
}
Vec3f operator/(Vec3f a, float b){
    return(Vec3f{a.x/b, a.y/b, a.z/b});
}
Vec3f operator-(Vec3f a){
    return(Vec3f{-a.x, -a.y, -a.z});
}
void operator+=(Vec3f& a, Vec3f b){
    a = a+b;
}
void operator-=(Vec3f& a, Vec3f b){
    a = a-b;
}
void operator*=(Vec3f& a, float b){
    a = a*b;
}
void operator/=(Vec3f& a, float b){
    a = a/b;
}
bool operator==(Vec3f a, Vec3f b) {
    return(nearlyEqual(a.x,b.x) && nearlyEqual(a.y,b.y)&& nearlyEqual(a.z,b.z));
}
bool operator!=(Vec3f a, Vec3f b) {
    return(!nearlyEqual(a.x, b.x) || !nearlyEqual(a.y, b.y) || !nearlyEqual(a.z,b.z));
}

float lenSquared(Vec3f a){
    return(a.x * a.x + a.y * a.y+a.z * a.z);
}
float len(Vec3f a){
    return(sqrt(lenSquared(a)));
}

float dotProduct(Vec3f a, Vec3f b){
    return(a.x * b.x + a.y * b.y + a.z *b.z);
}

// clockwise = +ve; and anticlockwise = -ve
Vec3f crossProduct(Vec3f a, Vec3f b){
    return(Vec3f{a.y*b.z-a.z*b.y, a.z*b.x-b.z*a.x, a.x * b.y - b.x * a.y});
}

Vec3f normalize(Vec3f a){
    return(a * Q_rsqrt(lenSquared(a)));
}
float projection(Vec3f of, Vec3f on){
    return(dotProduct(of, normalize(on)));
}





float smoothstep(float x){
    x = Clamp(0.0f,x,1.0f);
    return x * x * (3.0f - 2.0f * x);
}





// quick inverse square root from quake III 
float Q_rsqrt(float number)              
{
    int i;
    float x2, y;
    const float threehalfs = 1.5f;
    x2 = number * 0.5f;
    y  = number;
    i  = *(int*) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = *(float*)&i;
    y  = y * (threehalfs - (x2 * y * y));   // 1st iteration
	//  y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
    return y;
}

float fastSqRoot(float number){
    return(Q_rsqrt(number) * number);
}



Vec3f operator *(Mat3 m, Vec3f a){
    Vec3f b;
    b.x = m.arr[0][0] * a.x + m.arr[0][1] * a.y + m.arr[0][2] * a.z;
    b.y = m.arr[1][0] * a.x + m.arr[1][1] * a.y + m.arr[1][2] * a.z;
    b.z = m.arr[2][0] * a.x + m.arr[2][1] * a.y + m.arr[2][2] * a.z;
    return b;
}

Vec4f operator *(Mat4 m, Vec4f a){
    Vec4f b;
    b.x = m.arr[0][0] * a.x + m.arr[0][1] * a.y + m.arr[0][2] * a.z + m.arr[0][3] * a.w;
    b.y = m.arr[1][0] * a.x + m.arr[1][1] * a.y + m.arr[1][2] * a.z + m.arr[1][3] * a.w;
    b.z = m.arr[2][0] * a.x + m.arr[2][1] * a.y + m.arr[2][2] * a.z + m.arr[2][3] * a.w;
    b.w = m.arr[3][0] * a.x + m.arr[3][1] * a.y + m.arr[3][2] * a.z + m.arr[3][3] * a.w;
    return b;
}



Mat4 scaleAboutOrigin(float sx, float sy, float sz){
    return Mat4{
        sx,0,0,0,
        0,sy,0,0,
        0,0,sz,0,
        0,0,0,1
    };
}

Mat4 translate(float tx, float ty, float tz){
    return Mat4{
        1,0,0,tx,
        0,1,0,ty,
        0,0,1,tz,
        0,0,0,1
    };
}
