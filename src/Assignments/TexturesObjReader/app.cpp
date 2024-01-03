#include "app.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Application/utils.h"
#include "Engine/Mesh.cpp"
#include "Engine/Material.cpp"

using namespace xe;

void SimpleShapeApplication::init() {
    set_camera(new Camera);
    set_controler(new CameraControler(camera(), 0.01));

    auto program = utils::create_program({
        {GL_VERTEX_SHADER, std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
        {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}
    });

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    std::vector<GLfloat> vertices = {
            0, 1, -0.5,     1.0, 1.0,
            -0.5, 0, 0,     0.5, 0.809,
            0.5, 0, 0,      0.809, 0.5,
            0.5, 0, -1,     0.5, 0.1910,
            -0.5, 0, -1,    0.1910, 0.5,

            // Extra vertices for the top one (but with different UV)
            0, 1, -0.5,     0.0, 1.0,
            0, 1, -0.5,     1.0, 0.0,
            0, 1, -0.5,     0.0, 0.0,
    };
    std::vector<GLushort> indices_buffer = {0, 1, 2, 6, 2, 3, 7, 3, 4, 5, 4, 1, 1, 4, 2, 4, 3, 2};

    // Mesh setup
    auto pyramid = new Mesh;
    pyramid->allocate_vertex_buffer(vertices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    pyramid->load_vertices(0, vertices.size() * sizeof(GLfloat), vertices.data());
    pyramid->vertex_attrib_pointer(0, 3, GL_FLOAT, 5 * sizeof(GLfloat), 0);
    pyramid->vertex_attrib_pointer(1, 2, GL_FLOAT, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));

    pyramid->allocate_index_buffer(indices_buffer.size() * sizeof(GLushort), GL_STATIC_DRAW);
    pyramid->load_indices(0, indices_buffer.size() * sizeof(GLushort), indices_buffer.data());

    ColorMaterial::init();

    glm::vec4 base_color = {0.8, 0.8, 0.8, 1.0};
    GLuint texture = xe::load_texture((std::string) "/Models/multicolor.png");

    pyramid->add_submesh(0, 3, new ColorMaterial(base_color, texture, 0));
    pyramid->add_submesh(3, 6, new ColorMaterial(base_color, texture, 1));
    pyramid->add_submesh(6, 9, new ColorMaterial(base_color, texture, 2));
    pyramid->add_submesh(9, 12, new ColorMaterial(base_color, texture, 3));
    pyramid->add_submesh(12, 18, new ColorMaterial(base_color, texture, 4));
    add_submesh(pyramid);

    // PVM setup
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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glm::mat4 PVM = camera_->projection() * camera_->view();
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    for (auto m: meshes_)
        m->draw();
}