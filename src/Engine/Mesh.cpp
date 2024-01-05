//
// Created by Piotr Białas on 12/11/2021.
//

#include <iostream>

#include "Mesh.h"

#include "3rdParty/stb/stb_image.h"

#include "3rdParty/tinyobjloader/tiny_obj_loader.h"

void xe::Mesh::draw() const {
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_);
    for (auto i = 0; i < submeshes_.size(); i++) {
        materialas_[i]->bind();
        glDrawElements(GL_TRIANGLES, submeshes_[i].count(), GL_UNSIGNED_SHORT,
                       reinterpret_cast<void *>(sizeof(GLushort) * submeshes_[i].start));
        materialas_[i]->unbind();
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
    glBindVertexArray(0u);
}

void xe::Mesh::vertex_attrib_pointer(GLuint index, GLuint size, GLenum type, GLsizei stride, GLsizei offset) {
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, reinterpret_cast<void *>(offset));
    glBindBuffer(GL_ARRAY_BUFFER, 0u);
    glBindVertexArray(0u);
}


xe::Mesh::Mesh() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &v_buffer_);
    glGenBuffers(1, &i_buffer_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_);
    glBindVertexArray(0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
}

void xe::Mesh::allocate_index_buffer(size_t size, GLenum hint) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
}

void xe::Mesh::load_indices(size_t offset, size_t size, void *data) {

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
}


void xe::Mesh::allocate_vertex_buffer(size_t size, GLenum hint) {
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, hint);
    glBindBuffer(GL_ARRAY_BUFFER, 0u);
}

void xe::Mesh::
load_vertices(size_t offset, size_t size, void *data) {
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0u);
}

void *xe::Mesh::map_vertex_buffer() {
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_);
    return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void xe::Mesh::unmap_vertex_buffer() {
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void *xe::Mesh::map_index_buffer() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_);
    return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void xe::Mesh::unmap_index_buffer() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

GLuint xe::create_texture(const std::string &name) {

    stbi_set_flip_vertically_on_load(true);
    GLint width, height, channels;
    auto img = stbi_load(name.c_str(), &width, &height, &channels, 0);
    if (!img) {
        std::cout << "Could not read image from file `{}'" << name;
        return 0;
    }
    GLenum format;
    if (channels == 3)
        format = GL_RGB;
    else if (channels == 4) {
        format = GL_RGBA;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0u);

    return texture;
}