#pragma once

#define y_ARRAY
#ifdef y_ARRAY
#include "../Allocator/array.h"
#define Array Array

#else
#include <vector>
#define Array std::vector 
#endif

#include "../math/vec.h"

struct Mesh{
    Array<Vec3f> vertices;
    Array<uint32_t> indices;

    Array<Vec3f> normals;

    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
};

#undef Array

int createMesh(Mesh *m, Vec3f *vertices, int nvertices, uint32_t *indices, int nindices);
