//
// Created by Piotr Białas on 02/11/2021.
//

#define STB_IMAGE_IMPLEMENTATION  1

#include "Material.h"

#include "Application/utils.h"

#include <glad/gl.h>

#include "3rdParty/stb/stb_image.h"

namespace xe {

    GLuint ColorMaterial::color_uniform_buffer_ = 0u;
    GLint  ColorMaterial::uniform_map_Kd_location_ = 0;
    GLuint ColorMaterial::shader_ = 0u;

    void ColorMaterial::bind() {
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


    void ColorMaterial::init() {
        auto program = xe::utils::create_program(
                {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/color_vs.glsl"},
                 {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/color_fs.glsl"}});
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

    void ColorMaterial::unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0u);
        glBindTexture(GL_TEXTURE_2D, 0u);
    }

    GLuint load_texture(std::string path) {
        stbi_set_flip_vertically_on_load(true);
        GLint width, height, channels;
        std::string texture_file = std::string(ROOT_DIR) + path;
        auto img = stbi_load(texture_file.c_str(), &width, &height, &channels, 0);
        if (!img) {
            std::cout << "Could not read image from file " << texture_file.c_str() << std::endl;
        }

        GLuint texture;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        stbi_image_free(img);

        return texture;
    }

    GLuint create_texture(const std::string &name) {
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
}