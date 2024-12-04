#ifndef CAMERA_H
#define CAMERA_H

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
    glm::vec3 position() const
    {
        return pos;
    }
    glm::mat4 view() const;
    glm::mat4 projection(float aspect) const;
    void moveFront(float delta);
    void moveRight(float delta);
    void updateFront(glm::vec3 newFront);
    void updateFov(float newFov);
};

#endif // CAMERA_H
