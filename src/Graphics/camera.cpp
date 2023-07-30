#include "camera.h"
#include <math.h>

Mat4 Camera3D::lookat(Vec3f targetPos, Vec3f worldup){
    // imagine a camera normally looking towards -z with +x right and +y up
    // computing the new directions with similar orientations
    // the front is along +z
    front = normalize(pos - targetPos);
    right = normalize(crossProduct(worldup, front));
    up = normalize(crossProduct(front, right));

    // includes the rotation and translation of the camera 
    // here we need to convert a coordinate in world space to view space
    // a change in basis vectors as matrix
    //  [Rx Ux Fx 0]        [1 0 0 posx]
    //  [Ry Uy Fy 0] and    [0 1 0 posy]
    //  [Rz Uz Fz 0]        [0 0 1 posz]
    //  [0  0  0  1]        [0 0 0 1   ]
    // would change the coordinates in view space to world space
    //
    // so both matrices are inversed, ie, rotation matrix is transposed
    // and translation matrix is negated as
    //  [Rx Ry Rz 0]        [1 0 0 -posx]
    //  [Ux Uy Uz 0] and    [0 1 0 -posy]
    //  [Fx Fy Fz 0]        [0 0 1 -posz]
    //  [0  0  0  1]        [0 0 0  1   ]
    //  product of these matrices converts the world space coordinates to view space
    //  [Rx Ry Rz -R.pos]
    //  [Ux Uy Uz -U.pos]
    //  [Fx Fy Fz -F.pos]
    //  [0  0  0   1    ]

    Mat4 lookatmat = {
        right.x , right.y   , right.z   , -dotProduct(right, pos),
        up.x    , up.y      , up.z      , -dotProduct(up, pos),
        front.x , front.y   , front.z   , -dotProduct(front, pos),
        0       , 0         , 0         , 1
    };
    return(lookatmat);
}

void Camera3D::updatePos(uint32_t movementDirs, float deltatime){
    if (movementDirs & CameraMovement::FRONT){
        pos = pos - deltatime * (front * velFront);
    }
    else if (movementDirs & CameraMovement::BACK){
        pos = pos + deltatime * (front * velFront);
    }
    if (movementDirs & CameraMovement::LEFT){
        pos = pos - deltatime * (right * velSideways);
    }
    else if (movementDirs & CameraMovement::RIGHT){
        pos = pos + deltatime * (right * velSideways);
    }
}

void Camera3D::updateZoom(float multiplier){
    zoom *= multiplier;
}

void Camera3D::init(Vec3f front, Vec3f up, Vec3f right, float znear, float zfar, float screenW, float screenH){
    this->front = front;
    this->up = up;
    this->right = right;
    this->znear = znear;
    this->zfar = zfar;
    this->screenH = screenH;
    this->screenW = screenW;
    this->velFront = 0.01f;
    this->velSideways = 0.005f;
    this->zoom = 1.0f;
}


void Camera3D::updateOrientation(float xoffset, float yoffset){
    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw -= xoffset;
    pitch += yoffset;

    pitch = Clamp(-89,pitch,89);
    front.x = sinf(Radians(yaw)) ;
    front.y = sinf(Radians(pitch));
    front.z = cosf(Radians(yaw))* cosf(Radians(pitch));
    // front = normalize(front);
    // right = normalize(crossProduct(worldup, front));
    // up = normalize(crossProduct(front, right));
}


// projection matrix referenced from 
// http://www.songho.ca/opengl/gl_projectionmatrix.html
Mat4 projection(Camera3D * c){
    Mat4 p = {
        2.0f*c->znear/c->screenW, 0,0,0,
        0,2.0f*c->znear/c->screenH, 0,0,
        0,0,-(c->zfar+c->znear)/(c->zfar-c->znear), -2.0f*c->znear * c->zfar/(c->zfar-c->znear),
        0,0,-1,0
    };
    return p;
}




