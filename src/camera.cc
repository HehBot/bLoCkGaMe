#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 const Camera::globalUp(0.0f, 1.0f, 0.0f);

Camera::Camera(glm::vec3 _pos, glm::vec3 _front)
    : pos(_pos), front(glm::normalize(_front)), right(glm::normalize(glm::cross(_front, globalUp))), speed(5.0f), fov(45.0f)
{
}

glm::mat4 Camera::view() const
{
    return glm::lookAt(pos, pos + front, globalUp);
}

glm::mat4 Camera::projection(float aspect) const
{
    return glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
}

float Camera::step(float deltaTime) const
{
    return speed * deltaTime;
}

void Camera::moveFront(float step)
{
    pos += step * front;
}

void Camera::moveRight(float step)
{
    pos += step * right;
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
