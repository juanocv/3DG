// window.hpp
#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcg.hpp"       // Include ABCg framework base classes
#include "abcgOpenGL.hpp" // Include OpenGL definitions

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "line.hpp"
#include "sphere.hpp"
#include "physics.hpp"    // Inclui as funções relacionadas à física

const float gravity{9.81f};
const float pivotHeight{2.0f};

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onUpdate() override;
  void onDestroy() override;
  void onEvent(SDL_Event const &event) override;
  void onResize(glm::ivec2 const &size) override;

private:
  // Pendulum parameters
  int ropeLength{100};
  int animationSpeed{100};
  int thetaDegrees{30}; // Inclination angle in degrees

  // Simulation variables
  float angle{0.0f};
  float deltaTime{0.0f};
  float angularVelocity{0.0f};
  float actualRopeLength{};

  // Camera control
  glm::vec3 cameraPosition{5.0f, 2.0f,
                           0.0f}; // Position the camera along the X-axis
  glm::vec3 cameraTarget{0.0f, 0.0f, 0.0f}; // Looking towards the origin
  float cameraYaw{-90.0f};                  // Adjust as necessary
  float cameraPitch{0.0f};

  // Light control
  float lightYaw{0.0f};   // Horizontal angle of the light (in degrees)
  float lightPitch{45.0f}; // Vertical angle of the light (in degrees)

  // Matrices as member variables
  glm::mat4 m_viewMatrix;
  glm::mat4 m_projMatrix;

  // OpenGL variables
  GLuint program{};
  GLint modelMatrixLoc{};
  GLint viewMatrixLoc{};
  GLint projMatrixLoc{};
  GLint colorLoc{};

  // Texture
  GLuint m_groundTexture;
  GLuint m_sphereTexture{};

  // Ground plane variables
  GLuint groundVAO{};
  GLuint groundVBO{};
  GLuint groundEBO{};

  // Sphere & Line
  Sphere m_sphere;
  Line m_line;

  // Ground plane color
  glm::vec3 groundColor{1.0f, 1.0f, 1.0f}; // Brown color

  // Color variable for the ball
  glm::vec3 ballColor{1.0f, 1.0f, 1.0f}; // Default to white color

  // Input handling
  bool m_forward{false};
  bool m_backward{false};
  bool m_left{false};
  bool m_right{false};
  bool m_mouseCaptured{true}; // Start with the mouse captured

  float m_lastX{};
  float m_lastY{};
  bool m_firstMouse{true};
  float m_sensitivity{0.1f};
  float m_ropeLengthInPixels{};
  float m_angularSpeedInPixels{};

  // Helper methods
  void handleInput();
  void renderPendulum();
  void renderGround();

  // Viewport size
  glm::ivec2 m_viewportSize{getWindowSettings().width,
                            getWindowSettings().height};
};

#endif
