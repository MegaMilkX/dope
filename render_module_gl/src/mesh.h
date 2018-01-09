#ifndef MESH_H
#define MESH_H

#include "gfxm.h"
#include "gl/glextutil.h"

class MeshBuffer
{
public:
    MeshBuffer()
        : indexCount(0), offset(0), vertexSize(20)
    {
        glGenVertexArrays(1, &vao);
        Bind();

        // =====
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        

        glGenBuffers(1, &uvBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        

        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    }

    void SetPositionData(std::vector<gfxm::vec3>& data)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(gfxm::vec3), (void*)data.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(gfxm::vec3), 0);
    }

    void SetUVData(std::vector<gfxm::vec2>& data)
    {
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(gfxm::vec2), (void*)data.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(gfxm::vec2), 0);
    }

    void SetIndexData(std::vector<unsigned short>& data)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(unsigned short), (void*)data.data(), GL_STATIC_DRAW);
        indexCount = data.size();
    }

    ~MeshBuffer()
    {
        glDeleteBuffers(1, &indexBuffer);
        glDeleteBuffers(1, &uvBuffer);
        glDeleteBuffers(1, &vertexBuffer);

        glDeleteVertexArrays(1, &vao);
    }

    void Bind()
    {
        glBindVertexArray(vao);
    }

    void Render()
    {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)offset);
    }
private:
    GLuint vao;
    GLuint vertexBuffer;
    GLuint uvBuffer;
    GLuint indexBuffer;

    int indexCount;
    int offset;

    int vertexSize;
};

#endif
