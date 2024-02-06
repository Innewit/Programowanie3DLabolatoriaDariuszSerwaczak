//
// Created by Piotr Białas on 02/11/2021.
//

#include "PhongMaterial.h"

#include "Application/utils.h"

#include <glad/gl.h>

namespace xe {

    GLuint PhongMaterial::color_uniform_buffer_ = 0u;
    GLint  PhongMaterial::uniform_map_Kd_location_ = 0;
    GLuint PhongMaterial::shader_ = 0u;

    void PhongMaterial::bind() {
        glUseProgram(program());
        int use_map_Kd  = 0;
        if (texture_ > 0) {
            OGL_CALL(glUniform1i(uniform_map_Kd_location_, texture_unit_));
            OGL_CALL(glActiveTexture(GL_TEXTURE0 + texture_unit_));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, texture_));
            use_map_Kd  = 1;
        }
        OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, color_uniform_buffer_));
        glBindBuffer(GL_UNIFORM_BUFFER, color_uniform_buffer_);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), &Kd_[0]);
        glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), sizeof(GLint), &use_map_Kd);
        OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0u));
    }


    void PhongMaterial::init() {
        auto program = xe::utils::create_program(
                {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/phong_vs.glsl"},
                 {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/phong_fs.glsl"}});
        if (!program) {
            std::cerr << "Invalid program" << std::endl;
            exit(-1);
        }

        shader_ = program;

        glGenBuffers(1, &color_uniform_buffer_);

        glBindBuffer(GL_UNIFORM_BUFFER, color_uniform_buffer_);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) + sizeof(GLint), nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0u);
#if __APPLE__
        auto u_modifiers_index = glGetUniformBlockIndex(shader_, "Color");
        if (u_modifiers_index == -1) {
            std::cout << "Cannot find  {} uniform block in program" << "Color" << std::endl;
        } else {
            glUniformBlockBinding(program, u_modifiers_index, 0);
        }
#endif

#if __APPLE__
        auto u_transformations_index = glGetUniformBlockIndex(shader_, "Transformations");
        if (u_transformations_index == -1) {
            std::cout << "Cannot find  {} uniform block in program" << "Transformation" << std::endl;
        } else {
            glUniformBlockBinding(program, u_transformations_index, 1);
        }
#endif

        uniform_map_Kd_location_ = glGetUniformLocation(shader_, "map_Kd");
        if (uniform_map_Kd_location_ == -1) {
            std::cout << "Cannot get uniform {} location" << "map_Kd" << std::endl;
        }

    }

    void PhongMaterial::unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0u);
        glBindTexture(GL_TEXTURE_2D, 0u);
    }
}