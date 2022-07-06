#include "Mesh.h"

// If indices pointer is nullptr, then glDrawArrays will be used, glDrawElements if not nullptr
Mesh::Mesh(Vertex* vertices, int _n_vertices, unsigned int* indices, int _n_indices, GLenum buffer_usage, GLenum draw_mode)
{
    n_indices = _n_indices;
    n_vertices = _n_vertices;
    this->buffer_usage = buffer_usage;
    this->draw_mode = draw_mode;

    // If buffer will be changed in the future, store the vertices
    if (buffer_usage == GL_DYNAMIC_DRAW) {
        this->vertices = vertices;
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, n_vertices * sizeof(Vertex), vertices, buffer_usage);

    if (indices != nullptr) {
        use_draw_elements = true;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indices * sizeof(unsigned int), indices, buffer_usage);
    }

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

void Mesh::Update() {
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
    if (use_draw_elements) {
        glDrawElements(draw_mode, n_indices, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(draw_mode, 0, n_vertices);
    }
}
