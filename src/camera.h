#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera {
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 right;
    static glm::vec3 const globalUp;
    float speed;
    float fov;

public:
    Camera(glm::vec3 _pos, glm::vec3 _front);
    glm::mat4 view() const;
    glm::mat4 projection(float aspect) const;
    float step(float deltaTime) const;
    void moveFront(float step);
    void moveRight(float step);
    void updateFront(glm::vec3 newFront);
    void updateFov(float newFov);
};

#endif // CAMERA_HPP
