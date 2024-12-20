#include "physics.hpp"

glm::vec3 calculateLightDirection(float lightYaw, float lightPitch) {
  float yawRad = glm::radians(lightYaw);
  float pitchRad = glm::radians(lightPitch);

  glm::vec3 dir;
  dir.x = std::cos(yawRad) * std::cos(pitchRad);
  dir.y = std::sin(pitchRad);
  dir.z = std::sin(yawRad) * std::cos(pitchRad);

  return glm::normalize(dir);
}

float calculateRopeLengthInPixels(const glm::vec3 &ropeStart,
                                  const glm::vec3 &ropeEnd,
                                  const glm::mat4 &viewMatrix,
                                  const glm::mat4 &projMatrix,
                                  const glm::ivec2 &viewportSize) {
  glm::vec4 startNDC = projMatrix * viewMatrix * glm::vec4(ropeStart, 1.0f);
  glm::vec4 endNDC = projMatrix * viewMatrix * glm::vec4(ropeEnd, 1.0f);

  startNDC /= startNDC.w;
  endNDC /= endNDC.w;

  glm::vec2 startScreen{(startNDC.x * 0.5f + 0.5f) * viewportSize.x,
                        (1.0f - (startNDC.y * 0.5f + 0.5f)) * viewportSize.y};
  glm::vec2 endScreen{(endNDC.x * 0.5f + 0.5f) * viewportSize.x,
                      (1.0f - (endNDC.y * 0.5f + 0.5f)) * viewportSize.y};

  return glm::length(endScreen - startScreen);
}

float calculateAngularSpeedInPixels(float angularSpeedRadiansPerSec,
                                    const glm::mat4 &viewMatrix,
                                    const glm::mat4 &projMatrix,
                                    float thetaDegrees, 
                                    float actualRopeLength,
                                    const glm::ivec2 &viewportSize) {
  float theta = glm::radians(thetaDegrees);
  float r = actualRopeLength * std::sin(theta);
  glm::vec3 center(0.0f, 2.0f, 0.0f);
  glm::vec3 ballPosition(
      center.x + r, center.y - actualRopeLength * std::cos(theta), center.z);

  float screenRadius = calculateRopeLengthInPixels(
      center, ballPosition, viewMatrix, projMatrix, viewportSize);

  return screenRadius * angularSpeedRadiansPerSec;
}
