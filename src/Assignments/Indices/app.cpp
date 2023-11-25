//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    // A vector containing the x,y,z vertex coordinates for the square (base of the house) and the triangle (roof of the house).
    std::vector<GLfloat> vertices = {
        -0.5f, -0.25f, 0.0f, 1.0f, 0.0f, 0.0f,  // Bottom left corner of the square
         0.5f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f,  // Bottom right corner of the square
         0.5f,  0.25f, 0.0f, 0.0f, 0.0f, 1.0f,  // Top right corner of the square
        -0.5f,  0.25f, 0.0f, 1.0f, 0.0f, 0.0f,  // Top left corner of the square
         0.0f,  0.75f, 0.0f, 0.0f, 0.0f, 1.0f   // Top of the roof
    };

    // Creating indices buffer
    GLuint i_buffer_handle;
    std::vector<GLushort> indices_buffer = {0, 1, 2, 4, 3};
    glGenBuffers(1, &i_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_buffer.size() * sizeof(GLushort), indices_buffer.data(), GL_STATIC_DRAW);

    // Generating the buffer and loading the vertex data into it.
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);

    // This indicates that the data for attribute 0 (position) should be read from a vertex buffer.
    glEnableVertexAttribArray(0);
    // and this specifies how the position data is layout in the buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));

    // This indicates that the data for attribute 1 (color) should be read from a vertex buffer.
    glEnableVertexAttribArray(1);
    // and this specifies how the color data is layout in the buffer.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    auto[w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    // Binding the VAO will setup all the required vertex buffers.
    glBindVertexArray(vao_);
    // Draw the square (base of the house)
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, nullptr);
    // Draw the triangle (roof of the house)
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void*)(3 * sizeof(GLushort)));
    glBindVertexArray(0);
}
