#include "app.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Application/utils.h"

void SimpleShapeApplication::init() {
    set_camera(new Camera);
    set_controler(new CameraControler(camera(), 0.01));

    auto program = xe::utils::create_program({
        {GL_VERTEX_SHADER, std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
        {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}
    });

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    std::vector<GLfloat> vertices = {
            0, 1, -0.5, 0.0f, 1.0f, 0.7f,
            -0.5, 0, 0, 0.6f, 0.4f, 0.7f,
            0.5, 0, 0, 0.5f, 0.4f, 8.0f,
            0.5, 0, -1, 0.1f, 0.8f, 0.2f,
            -0.5, 0, -1, 1.0f, 0.5f, 0.2f,
    };

    GLuint u_buffer_handle;
    glGenBuffers(1, &u_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, u_buffer_handle);

    float strength = 0.8f;
    float color[4] = {0.8f, 0.8f, 0.9f, 1.0f};
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 4 * sizeof(float), color);

    glGenBuffers(1, &u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_pvm_buffer_);

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.5f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -0.5f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    camera_->look_at(cameraPos, cameraTarget, upVector);

    int w, h;
    std::tie(w, h) = frame_buffer_size();
    float aspect = static_cast<float>(w) / h;
    float fov = glm::pi<float>() / 4.0;
    float near = 0.1f;
    float far = 100.0f;
    camera_->perspective(fov, aspect, near, far);

    glm::mat4 PVM = camera_->projection() * camera_->view();
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(PVM));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint bindingPoint = 1;
    GLuint blockIndex = glGetUniformBlockIndex(program, "Transformations");
    glUniformBlockBinding(program, blockIndex, bindingPoint);

    GLuint i_buffer_handle;
    std::vector<GLushort> indices_buffer = {0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1, 1, 4, 2, 4, 3, 2};
    glGenBuffers(1, &i_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices_buffer.size() * sizeof(GLushort),
            indices_buffer.data(),
            GL_STATIC_DRAW
            );

    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * sizeof(GLfloat),
            reinterpret_cast<GLvoid*>(0)
            );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * sizeof(GLfloat),
            reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat))
            );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    glViewport(0, 0, w, h);
    glUseProgram(program);
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    camera_->set_aspect((float) w / h);
}

void SimpleShapeApplication::frame() {
    glBindVertexArray(vao_);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glm::mat4 PVM = camera_->projection() * camera_->view();
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, nullptr);
    glBindVertexArray(0);
}