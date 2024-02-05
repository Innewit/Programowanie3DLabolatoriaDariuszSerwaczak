//
// Created by Dariusz Serwaczak on 22/12/2023.
//

#include "camera.h"

#ifndef GRAPHICS3DCODE_CAMERA_CONTROLER_H
#define GRAPHICS3DCODE_CAMERA_CONTROLER_H

class CameraControler {
public:
    CameraControler(): camera_(nullptr), scale_(0.1f) {}
    CameraControler(Camera* camera): camera_(camera), scale_(0.1f) {}
    CameraControler(Camera* camera, float scale): camera_(camera), scale_(scale) {}

    void set_camera(Camera *camera) { camera_ = camera; }
    void set_scale(float scale) { scale_ = scale; }

    void rotate_camera(float dx, float dy) {
        camera_->rotate_around_center(-scale_ * dy, camera_->x());
        camera_->rotate_around_center(-scale_ * dx, camera_->y());
    }

    void mouse_moved(float x, float y) {
        if (LMB_pressed_) {
            auto dx = x - x_;
            auto dy = y - y_;
            x_ = x;
            y_ = y;

            rotate_camera(dx, dy);
        }
    };

    void LMB_pressed(float x, float y) {
        LMB_pressed_ = true;
        x_ = x;
        y_ = y;
    };

    void LMB_released(float x, float y) {
        LMB_pressed_ = false;
        auto dx = x - x_;
        auto dy = y - y_;

        rotate_camera(dx, dy);
    };

private:
    Camera *camera_;
    float scale_;
    bool LMB_pressed_;
    float x_, y_;
};

#endif //GRAPHICS3DCODE_CAMERA_CONTROLER_H
