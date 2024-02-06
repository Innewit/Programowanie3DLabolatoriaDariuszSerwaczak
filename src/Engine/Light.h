#include <glm/glm.hpp>

namespace xe {
    struct PointLight {
        PointLight() = default;
        PointLight(const glm::vec3 &pos, const glm::vec3 &color, const glm::vec3 &intensity) : position_in_ws(pos),
                                                                                         color(color), intensity(intensity) {}
        alignas(16) glm::vec3 position_in_ws;
        alignas(16) glm::vec3 position_in_vs;
        alignas(16) glm::vec3 color;
        alignas(16) glm::vec3 intensity;
    };
}