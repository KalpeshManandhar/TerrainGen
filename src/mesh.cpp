#include "mesh.h"
#include <glad/glad.h>

static int setupMesh(Mesh *m){
    glGenVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao);

    glGenBuffers(1,&m->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    size_t vertexSize = m->vertices.size() * sizeof(m->vertices[0]);
    size_t normalSize = m->normals.size() * sizeof(m->normals[0]);

    size_t totalSize = vertexSize + normalSize;
    glBufferData(GL_ARRAY_BUFFER, totalSize, NULL, GL_STATIC_DRAW);
    
    glBufferSubData(GL_ARRAY_BUFFER, NULL, vertexSize, &m->vertices[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) *3, 0);
    glEnableVertexAttribArray(0);


    if (normalSize != 0){
        glBufferSubData(GL_ARRAY_BUFFER, vertexSize, normalSize, &m->normals[0]);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) *3, (void *)vertexSize);
        glEnableVertexAttribArray(1);
    } 

    glGenBuffers(1,&m->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
    size_t indexSize = m->indices.size() * sizeof(m->indices[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, &m->indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    return 0;
}




int createMesh(Mesh *m, Vec3f *vertices, int nvertices, uint32_t *indices, int nindices){
    for (int i = 0; i < nvertices; i++){
        m->vertices.push_back(vertices[i]);
    }

    for (int i = 0; i < nindices; i++){
        m->indices.push_back(indices[i]);
    }
    setupMesh(m);    
    return 0;
}



