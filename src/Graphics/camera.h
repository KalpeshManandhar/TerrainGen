#pragma once

#include <stdio.h>
#include <stdint.h>
#include "../math/vec.h"

enum CameraMovement{
    NONE = 0x0, 
    FRONT = 0x1,
    BACK = 0x2,
    LEFT = 0x4,
    RIGHT = 0x8,
};


struct Camera3D{
    Vec3f front, right, up;
    float yaw, pitch;
    
    Vec3f pos;
    float velSideways;
    float velFront;

    float znear, zfar;
    float screenW, screenH;
    float zoom;

    void init(Vec3f front, Vec3f up, Vec3f right, float znear, float zfar, float screenW, float screenH);
    Mat4 lookat(Vec3f targetPos, Vec3f worldup);
    void updatePos(uint32_t movementDirs, float deltatime);
    void updateOrientation(float xoffset, float yoffset);
    void updateZoom(float multiplier);


};

Mat4 projection(Camera3D *camera);


