#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

glm::vec3 const Camera::globalUp(0.0f, 1.0f, 0.0f);

Camera::Camera(glm::vec3 _pos, glm::vec3 _front)
    : pos(_pos), front(glm::normalize(_front)), right(glm::normalize(glm::cross(_front, globalUp))), speed(20.0f), fov(60.0f)
{
}

glm::mat4 Camera::view() const
{
    return glm::lookAt(pos, pos + front, globalUp);
}

glm::mat4 Camera::projection(float aspect) const
{
    return glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
}

void Camera::moveFront(float delta)
{
    pos += speed * delta * front;
}

void Camera::moveRight(float delta)
{
    pos += speed * delta * right;
}

void Camera::updateFront(glm::vec3 newFront)
{
    front = newFront;
    right = glm::normalize(glm::cross(front, globalUp));
}

void Camera::updateFov(float newFov)
{
    fov = newFov;
}
