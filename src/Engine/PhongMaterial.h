//
// Created by Piotr Białas on 02/11/2021.
//

#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "Material.h"
#include "Application/utils.h"

namespace xe {

    class PhongMaterial : public Material {
    public:
        PhongMaterial(const glm::vec4 color) : Kd_(color), texture_(0) {
            set_texture(0);
            set_texture_unit(0);
        }
        PhongMaterial(const glm::vec4 color, GLuint texture, GLuint texture_unit) : Kd_(color) {
            set_texture(texture);
            set_texture_unit(texture_unit);
        }
        PhongMaterial(GLuint texture, GLuint texture_unit) : Kd_((glm::vec4){1.0, 1.0, 1.0, 1.0}){
            set_texture(texture);
            set_texture_unit(texture_unit);
        }

        void bind();
        void unbind();


        static void init();

        static GLuint program() { return shader_; }

        void set_texture(GLuint tex) { texture_ = tex; }
        [[nodiscard]] GLuint get_texture() const {
            return texture_;
        }

        void set_texture_unit(GLuint textureUnit) {
            texture_unit_ = textureUnit;
        }

        [[nodiscard]] GLuint get_texture_unit() const {
            return texture_unit_;
        }
    private:
        static GLuint shader_;
        static GLuint color_uniform_buffer_;

        static GLint uniform_map_Kd_location_;
        GLuint texture_;
        GLuint texture_unit_;
        glm::vec4 Kd_;

        glm::vec4 color_;
    };
}


