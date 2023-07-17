#pragma once

#include <vector>
#define Array std::vector 

#include "math/vec.h"

struct Mesh{
    Array<Vec3f> vertices;
    Array<uint32_t> indices;

    Array<Vec3f> normals;

    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
};

int createMesh(Mesh *m, Vec3f *vertices, int nvertices, uint32_t *indices, int nindices);
