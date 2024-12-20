#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

glm::vec3 calculateLightDirection(float lightYaw, float lightPitch);
float calculateRopeLengthInPixels(const glm::vec3 &ropeStart,
                                  const glm::vec3 &ropeEnd,
                                  const glm::mat4 &viewMatrix,
                                  const glm::mat4 &projMatrix,
                                  const glm::ivec2 &viewportSize);
float calculateAngularSpeedInPixels(float angularSpeedRadiansPerSec,
                                    const glm::mat4 &viewMatrix,
                                    const glm::mat4 &projMatrix,
                                    float thetaDegrees, float actualRopeLength,
                                    const glm::ivec2 &viewportSize);

#endif
