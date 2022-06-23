#include "Mesh.h"

Mesh::Mesh(Vertex* vertices, int _n_vertices, unsigned int* indices, int _n_indices, GLenum usage)
{
    buffer_usage = usage;
    n_indices = _n_indices;
    n_vertices = _n_vertices;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, n_vertices * sizeof(Vertex), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indices * sizeof(unsigned int), indices, usage);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coordinate));
    glEnableVertexAttribArray(1);

    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void Mesh::Update(Vertex* vertices) {
    if (buffer_usage == GL_DYNAMIC_DRAW) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        void* vbo_buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(vbo_buffer, vertices, n_vertices * sizeof(Vertex));
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
}

void Mesh::Draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, n_indices, GL_UNSIGNED_INT, 0);
}
