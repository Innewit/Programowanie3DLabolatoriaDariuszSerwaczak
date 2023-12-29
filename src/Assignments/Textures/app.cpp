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
            0, 1, -0.5,     1.0, 0.0,
            -0.5, 0, 0,     0.5, 0.809,
            0.5, 0, 0,      1.0, 1.0,
            0.5, 0, -1,     0.5, 0.809,
            -0.5, 0, -1,    0.1910, 0.5,
    }; //TODO Za malo wierzcholkow, piramida jest zdeformowana i przybiera losowo kolor niebieski
    //TODO Dodatkowo zmiany silnika uszkodziły MeshesMaterials, wiec trzeba naprawic bilblioteke w Engine lub
    //TODO wersjonowac wersje bliblioteki (jak sie wyrobie przed deadline-m to lepiej 1. podejscie)
    std::vector<GLushort> indices_buffer = {0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1, 1, 4, 2, 4, 3, 2};

    // Mesh setup
    auto pyramid = new Mesh;
    pyramid->allocate_vertex_buffer(vertices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    pyramid->load_vertices(0, vertices.size() * sizeof(GLfloat), vertices.data());
    pyramid->vertex_attrib_pointer(0, 3, GL_FLOAT, 3 * sizeof(GLfloat), 0);

    pyramid->allocate_index_buffer(indices_buffer.size() * sizeof(GLushort), GL_STATIC_DRAW);
    pyramid->load_indices(0, indices_buffer.size() * sizeof(GLushort), indices_buffer.data());

    ColorMaterial::init();

    ///////
    // Why? For this assignment, uniform color modifiers are not included in the shader, but we need to achieve the same color
    float strength = 0.8f;
    glm::vec4 color = {0.8f, 0.8f, 0.9f, 1.0f};
    ///////

    pyramid->add_submesh(0, 3, new ColorMaterial((glm::vec4){1.0f, 0.5f, 0.2f, 1.0f} * color * strength));
    pyramid->add_submesh(3, 6, new ColorMaterial((glm::vec4){0.6f, 0.4f, 0.7f, 1.0f} * color * strength));
    pyramid->add_submesh(6, 9, new ColorMaterial((glm::vec4){0.5f, 0.4f, 8.0f, 1.0f} * color * strength));
    pyramid->add_submesh(9, 12, new ColorMaterial((glm::vec4){0.1f, 0.8f, 0.2f, 1.0f} * color * strength));
    pyramid->add_submesh(12, 18, new ColorMaterial((glm::vec4){0.0f, 1.0f, 0.7f, 1.0f} * color * strength));
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

    load_predefined_texture();

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