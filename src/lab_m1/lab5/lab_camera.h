#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"

#include <iostream>
namespace implemented
{
    class Camera
    {
     public:
         Camera()
         {
             reversed = 1;
             angleX = 0;
         }
    
        Camera(const bool firstPerson, const float thirdPerson, bool reversedCamera)
        {
            position    = glm::vec3(0, 2, 5);
            forward     = glm::vec3(0, 0, -1);
            up          = glm::vec3(0, 1, 0);
            right       = glm::vec3(1, 0, 0);
            distanceToTarget = 2;
            globalUp = glm::vec3(0, 1, 0);
            this->firstPerson = firstPerson;
            this->thirdPerson = thirdPerson;
            this->reversedCamera = reversedCamera;

            if (reversedCamera)
                reversed = -1;
            else
                reversed = 1;

            angleX = 0;
        }

        Camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            Set(position, center, up);
        }

        ~Camera()
        { }

        // Update camera
        void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            this->position = position;
            forward     = glm::normalize(center - position);
            right       = glm::cross(forward, up);
            this->up    = glm::cross(right, forward);
        }

        float getCameraAngleX() {
            if (fabs(glm::dot(glm::normalize(glm::vec3(forward.x, 0, forward.z)), forward) - 1) < 0.00001)
                return 0;

            float angle = acos(glm::dot(glm::normalize(glm::vec3(forward.x, 0, forward.z)), forward));

            if (forward.y < 0)
                return -angle;

            return angle;
        }

        float getCameraAngleY() {
            float angle = acos(glm::dot(glm::normalize(glm::vec3(forward.x, 0, forward.z)), {0, 0, -1}));

            if (forward.x > 0)
                angle = 2 * glm::pi<float>() - angle;

            if (reversed == -1)
                return angle + glm::pi<float>();

            return angle;
        }

        void MoveForward(float distance)
        {
            // Translates the camera using the `dir` vector computed from
            // `forward`. Movement will always keep the camera at the same
            // height. For example, if you rotate your head up/down, and then
            // walk forward, then you will still keep the same relative
            // distance (height) to the ground!
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += reversed * dir * distance;
        }

        void MoveRight(float distance)
        {
            glm::vec3 dir = glm::normalize(glm::vec3(right.x, 0, right.z));
            position += reversed * dir * distance;
        }

        void TranslateForward(float distance)
        {
            // TODO(student): Translate the camera using the `forward` vector.
            // What's the difference between `TranslateForward()` and
            // `MoveForward()`?
            position += forward * distance;
        }

        void TranslateUpward(float distance)
        {
            // TODO(student): Translate the camera using the `up` vector.
            position += up * distance;
        }

        void TranslateUpwardGlobal(float distance)
        {
            // TODO(student): Translate the camera using the `up` vector.
            position += globalUp * distance;
        }

        void TranslateRight(float distance)
        {
            // TODO(student): See instructions below. Read the entire thing!
            // You need to translate the camera using the `right` vector.
            // Usually, however, translation using camera's `right` vector
            // is not very useful, because if the camera is rotated around the
            // `forward` vector, then the translation on the `right` direction
            // will have an undesired effect, more precisely, the camera will
            // get closer or farther from the ground. The solution is to
            // actually use the projected `right` vector (projected onto the
            // ground plane), which makes more sense because we will keep the
            // same distance from the ground plane.
            position += right * distance;
        }

        void RotateFirstPerson_OX(float angle)
        {
            // TODO(student): Compute the new `forward` and `up` vectors.
            // Don't forget to normalize the vectors! Use `glm::rotate()`.
            forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, right) * glm::vec4(forward, 1)));
            up = glm::normalize(glm::cross(right, forward));
            angleX += angle;
        }

        void RotateFirstPerson_OY(float angle)
        {
            // TODO(student): Compute the new `forward`, `up` and `right`
            // vectors. Use `glm::rotate()`. Don't forget to normalize the
            // vectors!
            forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, globalUp) * glm::vec4(forward, 1)));
            right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, globalUp) * glm::vec4(right, 1)));
            up = glm::cross(right, forward);
        }

        void RotateFirstPerson_OZ(float angle)
        {
            // TODO(student): Compute the new `right` and `up`. This time,
            // `forward` stays the same. Use `glm::rotate()`. Don't forget
            // to normalize the vectors!
            right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, forward) * glm::vec4(right, 1)));
            up = glm::normalize(glm::cross(right, forward));
        }

        void RotateThirdPerson_OX(float angle)
        {
            // TODO(student): Rotate the camera in third-person mode around
            // the OX axis. Use `distanceToTarget` as translation distance.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OX(angle);
            TranslateForward(-distanceToTarget);
        }

        void RotateThirdPerson_OY(float angle)
        {
            // TODO(student): Rotate the camera in third-person mode around
            // the OY axis.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OY(angle);
            TranslateForward(-distanceToTarget);
        }

        void RotateThirdPerson_OZ(float angle)
        {
            // TODO(student): Rotate the camera in third-person mode around
            // the OZ axis.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OZ(angle);
            TranslateForward(-distanceToTarget);
        }

        void rotateOX(float angle) {
            if (firstPerson)
                RotateFirstPerson_OX(angle);
            else
                RotateThirdPerson_OX(angle);
        }

        void rotateOY(float angle) {
            if (firstPerson)
                RotateFirstPerson_OY(angle);
            else
                RotateThirdPerson_OY(angle);
        }

        void rotateOZ(float angle) {
            if (firstPerson)
                RotateFirstPerson_OZ(angle);
            else
                RotateThirdPerson_OZ(angle);
        }

        glm::mat4 GetViewMatrix()
        {
            // Returns the view matrix
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position + forward * distanceToTarget;
        }

     public:
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 globalUp;
        float reversed;
        bool firstPerson, thirdPerson;
        bool reversedCamera;
        float angleX;
    };
}   // namespace implemented
