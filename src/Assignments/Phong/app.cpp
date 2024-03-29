#define GLM_ENABLE_EXPERIMENTAL

#include "app.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Application/utils.h"
#include "Engine/mesh_loader.cpp"
#include "Engine/Mesh.cpp"
#include "Engine/texture.cpp"
#include "ObjectReader/obj_reader.cpp"

struct SceneLights {
    PointLight light[24];
    int num_lights;
};

void SimpleShapeApplication::init() {
    set_camera(new Camera);
    set_controler(new CameraControler(camera(), 0.01));

    auto loaded_mesh = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/square.obj",
                                            std::string(ROOT_DIR) + "/Models");

    auto program = utils::create_program({
        {GL_VERTEX_SHADER, std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
        {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}
    });

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    // Mesh setup
    PhongMaterial::init();;
    add_submesh(loaded_mesh);

    // PVM setup
    glGenBuffers(1, &u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_pvm_buffer_);

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
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

    // Phong view space transformations
    glGenBuffers(1, &phong_vs_transformations);
    glBindBuffer(GL_UNIFORM_BUFFER, phong_vs_transformations);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) + sizeof(glm::mat3), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, phong_vs_transformations);

    // Point Lights
    add_light(*new PointLight({ 0.0, 0.2, 0.4 }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 }));
    add_light(*new PointLight({ -0.2, -0.2, 0.4 }, { 0.0, 1.0, 0.0 }, { 1.0, 1.0, 1.0 }));
    add_light(*new PointLight({ 0.2, -0.2, 0.4 }, { 0.0, 0.0, 1.0 }, { 1.0, 1.0, 1.0 }));
    xe::PhongMaterial::set_ambient({ 0.2f, 0.2f, 0.2f });

    glGenBuffers(1, &phong_lights_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, phong_lights_buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneLights), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 3, phong_lights_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, phong_lights_buffer);
    ////

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
    // Phong - View space transformations
    auto VM = camera_->view() * glm::mat4(1.0f);
    auto R = glm::mat3(VM);
    auto N = glm::mat3(glm::cross(R[1], R[2]), glm::cross(R[2], R[0]), glm::cross(R[0], R[1]));

    glBindBuffer(GL_UNIFORM_BUFFER, phong_vs_transformations);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(VM), &VM);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(VM), sizeof(N), &N);
    ////

    // Lights
    glBindBuffer(GL_UNIFORM_BUFFER, phong_lights_buffer);
    SceneLights scene_lights{};
    scene_lights.num_lights = p_lights_.size();

    for(int i = 0; i < scene_lights.num_lights; i++) scene_lights.light[i] = p_lights_[i];

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SceneLights), &scene_lights);
    ////

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glm::mat4 PVM = camera_->projection() * camera_->view();
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    for (auto m: meshes_)
        m->draw();
}