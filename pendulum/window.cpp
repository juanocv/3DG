// window.cpp
#include "window.hpp"
#include "physics.hpp"
#include "abcg.hpp"

/* glm::vec3 Window::calculateLightDirection() const {
  // Convert angles to radians
  float yawRad = glm::radians(lightYaw);
  float pitchRad = glm::radians(lightPitch);

  // Compute direction vector based on yaw and pitch
  glm::vec3 dir;
  dir.x = std::cos(yawRad) * std::cos(pitchRad);
  dir.y = std::sin(pitchRad);
  dir.z = std::sin(yawRad) * std::cos(pitchRad);

  return glm::normalize(dir);
} */

/* float Window::calculateRopeLengthInPixels(const glm::vec3 &ropeStart,
                                          const glm::vec3 &ropeEnd,
                                          const glm::mat4 &viewMatrix,
                                          const glm::mat4 &projMatrix) {
  // Transform ropeStart and ropeEnd to NDC coordinates
  glm::vec4 startNDC = projMatrix * viewMatrix * glm::vec4(ropeStart, 1.0f);
  glm::vec4 endNDC = projMatrix * viewMatrix * glm::vec4(ropeEnd, 1.0f);

  // Perform perspective division
  startNDC /= startNDC.w;
  endNDC /= endNDC.w;

  // Convert NDC coordinates to screen coordinates
  glm::vec2 startScreen{(startNDC.x * 0.5f + 0.5f) * m_viewportSize.x,
                        (1.0f - (startNDC.y * 0.5f + 0.5f)) * m_viewportSize.y};
  glm::vec2 endScreen{(endNDC.x * 0.5f + 0.5f) * m_viewportSize.x,
                      (1.0f - (endNDC.y * 0.5f + 0.5f)) * m_viewportSize.y};

  // Calculate and return the distance in pixels
  return glm::length(endScreen - startScreen);
} */

/* float Window::calculateAngularSpeedInPixels(float angularSpeedRadiansPerSec,
                                            const glm::mat4 &viewMatrix,
                                            const glm::mat4 &projMatrix) {
  // Use the current inclination angle
  float theta = glm::radians(static_cast<float>(thetaDegrees));

  // Horizontal radius in world space
  float r = actualRopeLength * std::sin(theta);

  // Define the center point (pivot point)
  glm::vec3 center(0.0f, pivotHeight, 0.0f);

  // Position of the pendulum bob at a fixed angle (e.g., angle = 0)
  float fixedAngle = 0.0f;
  float x = center.x + r * std::cos(fixedAngle);
  float z = center.z + r * std::sin(fixedAngle);
  float y = center.y - actualRopeLength * std::cos(theta);

  glm::vec3 ballPosition(x, y, z);

  // Calculate the screen-space radius
  float screenRadius =
      calculateRopeLengthInPixels(center, ballPosition, viewMatrix, projMatrix);

  // Angular speed in pixels/sec = radius (in pixels) * angular speed (in
  // radians/sec)
  return screenRadius * angularSpeedRadiansPerSec;
} */

void Window::onCreate() {
  // Load shaders

  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::ShaderSource vertexShader;
  vertexShader.source = assetsPath + "vertex_shader.glsl";
  vertexShader.stage = abcg::ShaderStage::Vertex;

  abcg::ShaderSource fragmentShader;
  fragmentShader.source = assetsPath + "fragment_shader.glsl";
  fragmentShader.stage = abcg::ShaderStage::Fragment;

  // Create the OpenGL program
  program = abcg::createOpenGLProgram({vertexShader, fragmentShader});

  // Get location of uniform variables
  modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
  viewMatrixLoc = glGetUniformLocation(program, "viewMatrix");
  projMatrixLoc = glGetUniformLocation(program, "projMatrix");
  colorLoc = glGetUniformLocation(program, "color");

  // Load ground texture
  abcg::OpenGLTextureCreateInfo groundTextureCreateInfo;
  groundTextureCreateInfo.path =
      assetsPath + "snow_02_diff_4k.jpg"; // Specify your texture
  m_groundTexture = abcg::loadOpenGLTexture(groundTextureCreateInfo);

  // Ensure that texture loading succeeded
  if (m_groundTexture == 0) {
    throw abcg::Exception("Failed to load ground texture");
  }

  // Load sphere texture
  abcg::OpenGLTextureCreateInfo sphereTextureInfo;
  sphereTextureInfo.path = assetsPath + "majoras_mask_moon.png";
  m_sphereTexture = abcg::loadOpenGLTexture(sphereTextureInfo);

  if (m_sphereTexture == 0) {
    throw abcg::Exception("Failed to load sphere texture");
  }

  // Set texture parameters
  glBindTexture(GL_TEXTURE_2D, m_groundTexture);
  glBindTexture(GL_TEXTURE_2D, m_sphereTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);

  // Create the sphere
  m_sphere.create(program);

  // Create the line
  m_line.create(program);

  // Set the mouse capture state
  SDL_SetRelativeMouseMode(m_mouseCaptured ? SDL_TRUE : SDL_FALSE);

  // Initialize camera orientation based on position and target
  glm::vec3 direction = glm::normalize(cameraTarget - cameraPosition);
  cameraYaw = glm::degrees(std::atan2(direction.z, direction.x));
  cameraPitch = glm::degrees(std::asin(direction.y));

  // Update cameraTarget based on yaw and pitch
  glm::vec3 front;
  front.x =
      std::cos(glm::radians(cameraYaw)) * std::cos(glm::radians(cameraPitch));
  front.y = std::sin(glm::radians(cameraPitch));
  front.z =
      std::sin(glm::radians(cameraYaw)) * std::cos(glm::radians(cameraPitch));
  cameraTarget = glm::normalize(front);

  // Create ground plane
  // Define the vertices of the ground plane
  std::array<glm::vec3, 4> groundVertices = {
      glm::vec3{-10.0f, 0.0f, -10.0f}, // Bottom-left
      glm::vec3{10.0f, 0.0f, -10.0f},  // Bottom-right
      glm::vec3{10.0f, 0.0f, 10.0f},   // Top-right
      glm::vec3{-10.0f, 0.0f, 10.0f}   // Top-left
  };

  // Define the normals for the ground plane
  std::array<glm::vec3, 4> groundNormals = {
      glm::vec3{0.0f, 1.0f, 0.0f}, // All normals pointing up
      glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f},
      glm::vec3{0.0f, 1.0f, 0.0f}};

  // Define the indices for two triangles
  std::array<GLuint, 6> groundIndices = {
      0, 1, 2, // First triangle
      2, 3, 0  // Second triangle
  };

  // Generate and bind the VAO for ground
  glGenVertexArrays(1, &groundVAO);
  glBindVertexArray(groundVAO);

  // Create VBO for ground vertices
  glGenBuffers(1, &groundVBO);
  glBindBuffer(GL_ARRAY_BUFFER, groundVBO);

  // Interleave position and normal data
  std::vector<Vertex> groundData;
  for (size_t i = 0; i < groundVertices.size(); ++i) {
    // Compute u,v based on vertexX, vertexZ
    // Suppose ground extends -10 to 10 in X and Z:
    float vertexX = groundVertices[i].x;
    float vertexZ = groundVertices[i].z;
    float u = (vertexX + 10.0f) / 20.0f;
    float v = (vertexZ + 10.0f) / 20.0f;

    groundData.push_back(Vertex{
        groundVertices[i], groundNormals[i],
        glm::vec2(u, v) // Assign texture coordinates
    });
  }
  glBufferData(GL_ARRAY_BUFFER, groundData.size() * sizeof(Vertex),
               groundData.data(), GL_STATIC_DRAW);

  // Create EBO for ground indices
  glGenBuffers(1, &groundEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices),
               groundIndices.data(), GL_STATIC_DRAW);

  // Retrieve attribute locations
  GLint groundPositionLoc = glGetAttribLocation(program, "inPosition");
  GLint groundNormalLoc = glGetAttribLocation(program, "inNormal");

  // Set attribute pointers for ground
  // Position attribute
  glEnableVertexAttribArray(groundPositionLoc);
  glVertexAttribPointer(
      groundPositionLoc, // Attribute location
      3,                 // Number of components (x, y, z)
      GL_FLOAT,          // Data type
      GL_FALSE,          // Normalized?
      sizeof(Vertex),    // Stride (size of Vertex struct)
      reinterpret_cast<void *>(offsetof(Vertex, position)) // Offset to position
  );

  // Normal attribute
  glEnableVertexAttribArray(groundNormalLoc);
  glVertexAttribPointer(
      groundNormalLoc, // Attribute location
      3,               // Number of components (x, y, z)
      GL_FLOAT,        // Data type
      GL_FALSE,        // Normalized?
      sizeof(Vertex),  // Stride (size of Vertex struct)
      reinterpret_cast<void *>(offsetof(Vertex, normal)) // Offset to normal
  );

  // After setting position and normal pointers, add:
  GLint groundTexCoordLoc = glGetAttribLocation(program, "inTexCoord");
  glEnableVertexAttribArray(groundTexCoordLoc);
  glVertexAttribPointer(
      groundTexCoordLoc,
      2, // x and y for texture coordinates
      GL_FLOAT, GL_FALSE, sizeof(Vertex),
      reinterpret_cast<void *>(offsetof(Vertex, texCoord)) // Offset to texCoord
  );

  // Unbind VAO (optional)
  glBindVertexArray(0);

  // Initialize viewport size
  m_viewportSize =
      glm::ivec2(getWindowSettings().width, getWindowSettings().height);

  // Set the initial viewport
  glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  // Initial calculation of rope length in pixels using fixed camera parameters
  glm::vec3 fixedCameraPosition{0.0f, 2.5f, 5.0f};
  glm::vec3 fixedCameraTarget{0.0f, 1.5f, 0.0f};
  glm::mat4 fixedViewMatrix = glm::lookAt(
      fixedCameraPosition, fixedCameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));

  float aspect = static_cast<float>(m_viewportSize.x) / m_viewportSize.y;
  glm::mat4 fixedProjMatrix =
      glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

  // Ensure 'actualRopeLength' is correctly set
  actualRopeLength =
      static_cast<float>(ropeLength) / 100.0f; // Converts percentage to meters

  // Calculate initial angular velocity
  float theta = glm::radians(static_cast<float>(thetaDegrees));
  float tanTheta = std::tan(theta);
  angularVelocity = std::sqrt((gravity * tanTheta) /
                              actualRopeLength); // ω in radians per second

  // Calculate initial rope length in pixels
  float fixedAngle = 0.0f;

  float r = actualRopeLength * std::sin(theta);
  float x = r * std::cos(fixedAngle);
  float z = r * std::sin(fixedAngle);
  float y = pivotHeight - actualRopeLength * std::cos(theta);

  glm::vec3 ropeStart(0.0f, pivotHeight, 0.0f);
  glm::vec3 ropeEnd(x, y, z);

  m_ropeLengthInPixels = calculateRopeLengthInPixels(
      ropeStart, ropeEnd, fixedViewMatrix, fixedProjMatrix, m_viewportSize);

  m_angularSpeedInPixels = calculateAngularSpeedInPixels(
      angularVelocity, fixedViewMatrix, fixedProjMatrix, theta, actualRopeLength, m_viewportSize);

  m_angularSpeedInPixels *= (static_cast<float>(animationSpeed) / 100.0f);
}

void Window::onUpdate() {
  // Update deltaTime
  deltaTime = static_cast<float>(getDeltaTime());

  // Convert theta to radians
  float theta = glm::radians(static_cast<float>(thetaDegrees));

  // Define actualRopeLength
  actualRopeLength =
      static_cast<float>(ropeLength) / 100.0f; // Converts percentage to meters

  // Calculate angular velocity based on rope length and theta
  float tanTheta = std::tan(theta);
  angularVelocity = std::sqrt((gravity * tanTheta) /
                              actualRopeLength); // ω in radians per second

  // Update the angle based on calculated angular velocity and animation speed
  angle += angularVelocity * (static_cast<float>(animationSpeed) / 100.0f) *
           deltaTime;

  // Keep angle within [0, 2π]
  angle = std::fmod(angle, 2.0f * glm::pi<float>());

  // Handle camera input
  handleInput();
}

void Window::onPaint() {
  // Set the clear color to dark gray
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program);

  // Set up view and projection matrices
  m_viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraTarget,
                             glm::vec3(0.0f, 1.0f, 0.0f));

  // Use m_viewportSize to calculate the aspect ratio
  float aspect = static_cast<float>(m_viewportSize.x) / m_viewportSize.y;
  m_projMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

  // Pass matrices to the shader
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);

  // Calculate and pass the dynamic light direction
  glm::vec3 lightDirection = calculateLightDirection(lightPitch, lightPitch);
  GLint lightDirLoc = glGetUniformLocation(program, "lightDir");
  glUniform3fv(lightDirLoc, 1, &lightDirection.x);

  // Render the sphere with its own texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_sphereTexture);
  GLint samplerLoc = glGetUniformLocation(program, "textureSampler");
  glUniform1i(samplerLoc, 0);
  renderPendulum(); // Draw sphere object with m_sphereTexture

  // Render the ground with the other texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_groundTexture);
  glUniform1i(samplerLoc, 0);
  renderGround(); // Draw ground with m_texture

  glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  // Definir a próxima janela para começar minimizada
  ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);

  ImGui::Begin("Controles do Pêndulo", nullptr,
               ImGuiWindowFlags_AlwaysAutoResize);

  // Existing controls
  bool thetaChanged =
      ImGui::SliderInt("Ângulo de Inclinação (°)", &thetaDegrees, 20, 85);
  bool ropeLengthChanged =
      ImGui::SliderInt("Comprimento da Corda (%)", &ropeLength, 1, 200);
  bool lightYawChanged =
      ImGui::SliderFloat("Luz Yaw (°)", &lightYaw, -180.0f, 180.0f);
  bool lightPitchChanged =
      ImGui::SliderFloat("Luz Pitch (°)", &lightPitch, -89.0f, 89.0f);
  bool animationChanged = ImGui::SliderInt("Velocidade da Animação (%)",
                                           &animationSpeed, 100, 1000);

  // Add color picker for the ball
  ImGui::ColorEdit3("Cor da Esfera", &ballColor[0]);

  // Update actualRopeLength
  actualRopeLength =
      static_cast<float>(ropeLength) / 100.0f; // Converts percentage to meters

  if (ropeLengthChanged || thetaChanged || animationChanged ||
      lightYawChanged || lightPitchChanged) {
    // Recalculate angular velocity
    float theta = glm::radians(static_cast<float>(thetaDegrees));
    float tanTheta = std::tan(theta);

    angularVelocity = std::sqrt((gravity * tanTheta) /
                                actualRopeLength); // ω in radians per second

    // Recalculate rope length and angular speed in pixels using fixed camera
    // parameters
    glm::vec3 fixedCameraPosition{0.0f, 2.5f, 5.0f};
    glm::vec3 fixedCameraTarget{0.0f, 1.5f, 0.0f};
    glm::mat4 fixedViewMatrix = glm::lookAt(
        fixedCameraPosition, fixedCameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));

    float aspect = static_cast<float>(m_viewportSize.x) / m_viewportSize.y;
    glm::mat4 fixedProjMatrix =
        glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    // **Set theta to 0 for m_ropeLengthInPixels calculation**
    float thetaForLength = 0.0f; // Rope hanging straight down
    // float fixedAngle = 0.0f;

    // Positions for m_ropeLengthInPixels
    // float r_length = actualRopeLength * std::sin(thetaForLength); // r_length
    // = 0 float x_length = r_length * std::cos(fixedAngle); // x_length = 0
    // float z_length = r_length * std::sin(fixedAngle); // z_length = 0
    float y_length =
        pivotHeight -
        actualRopeLength *
            std::cos(
                thetaForLength); // y_length = pivotHeight - actualRopeLength

    glm::vec3 ropeStart(0.0f, pivotHeight, 0.0f);
    glm::vec3 ropeEnd(0.0f, y_length, 0.0f); // Rope is vertical

    // Recalculate rope length in pixels
    m_ropeLengthInPixels = calculateRopeLengthInPixels(
        ropeStart, ropeEnd, fixedViewMatrix, fixedProjMatrix, m_viewportSize);

    // Calculate m_angularSpeedInPixels using actual theta
    m_angularSpeedInPixels = calculateAngularSpeedInPixels(
        angularVelocity, fixedViewMatrix, fixedProjMatrix, theta, actualRopeLength,  m_viewportSize);

    // Adjust for animation speed
    m_angularSpeedInPixels *= (animationSpeed / 100.0f);
  }

  // Display the rope length in pixels
  ImGui::Text("Comprimento da Corda: %.2f pixels", m_ropeLengthInPixels);

  // Display the calculated angular speed in pixels/sec
  ImGui::Text("Velocidade Angular: %.2f pixels/s", m_angularSpeedInPixels);

  ImGui::End();
}

void Window::onDestroy() {
  m_sphere.destroy();
  m_line.destroy();

  // Delete ground plane buffers
  glDeleteBuffers(1, &groundVBO);
  glDeleteBuffers(1, &groundEBO);
  glDeleteVertexArrays(1, &groundVAO);

  // Release the mouse cursor
  SDL_SetRelativeMouseMode(SDL_FALSE);

  glDeleteProgram(program);
}

void Window::handleInput() {
  float cameraSpeed =
      2.5f * deltaTime * (static_cast<float>(animationSpeed) / 100.0f);

  glm::vec3 cameraRight =
      glm::normalize(glm::cross(cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f)));

  if (m_forward)
    cameraPosition += cameraSpeed * cameraTarget;
  if (m_backward)
    cameraPosition -= cameraSpeed * cameraTarget;
  if (m_left)
    cameraPosition -= cameraRight * cameraSpeed;
  if (m_right)
    cameraPosition += cameraRight * cameraSpeed;
}

void Window::onEvent(SDL_Event const &event) {
  // Handle key events for toggling mouse capture
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_CAPSLOCK) {
      m_mouseCaptured = !m_mouseCaptured;
      SDL_SetRelativeMouseMode(m_mouseCaptured ? SDL_TRUE : SDL_FALSE);
    }

    // Handle movement keys
    if (event.key.keysym.sym == SDLK_w)
      m_forward = true;
    if (event.key.keysym.sym == SDLK_s)
      m_backward = true;
    if (event.key.keysym.sym == SDLK_a)
      m_left = true;
    if (event.key.keysym.sym == SDLK_d)
      m_right = true;
  }

  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_w)
      m_forward = false;
    if (event.key.keysym.sym == SDLK_s)
      m_backward = false;
    if (event.key.keysym.sym == SDLK_a)
      m_left = false;
    if (event.key.keysym.sym == SDLK_d)
      m_right = false;
  }

  // Handle mouse motion events
  if (event.type == SDL_MOUSEMOTION && m_mouseCaptured) {
    float xoffset = static_cast<float>(event.motion.xrel) * m_sensitivity;
    float yoffset =
        -static_cast<float>(event.motion.yrel) * m_sensitivity; // Invert y-axis

    cameraYaw += xoffset;
    cameraPitch += yoffset;

    // Constrain the pitch
    if (cameraPitch > 89.0f)
      cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
      cameraPitch = -89.0f;

    // Update camera target vector
    glm::vec3 front;
    front.x =
        std::cos(glm::radians(cameraYaw)) * std::cos(glm::radians(cameraPitch));
    front.y = std::sin(glm::radians(cameraPitch));
    front.z =
        std::sin(glm::radians(cameraYaw)) * std::cos(glm::radians(cameraPitch));
    cameraTarget = glm::normalize(front);
  }
}

void Window::onResize(glm::ivec2 const &size) {
  // Update the viewport size
  m_viewportSize = size;

  // Set the OpenGL viewport to cover the new window size
  glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
}

void Window::renderGround() {
  // Set the model matrix for the ground plane
  glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  // Set the color to the ground color
  glUniform4f(colorLoc, groundColor.r, groundColor.g, groundColor.b, 1.0f);

  // Bind the ground VAO
  glBindVertexArray(groundVAO);

  // Draw the ground plane using element array
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  // Unbind the VAO
  glBindVertexArray(0);
}

void Window::renderPendulum() {
  // Define the height of the pole
  float poleHeight = 2.0f;

  // Convert theta to radians
  float theta = glm::radians(static_cast<float>(thetaDegrees));

  // Define actualRopeLength
  actualRopeLength = static_cast<float>(ropeLength) / 100.0f;

  // Compute the horizontal radius (r)
  float r = actualRopeLength * std::sin(theta);

  // Compute the position of the ball
  float y = poleHeight -
            actualRopeLength * std::cos(theta); // Vertical position of the ball
  float x = r * std::cos(angle);                // Horizontal position along X
  float z = r * std::sin(angle);                // Horizontal position along Z

  // Model matrix for the ball
  glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
  modelMatrix =
      glm::scale(modelMatrix, glm::vec3(0.1f)); // Scale down the sphere
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  // Set the color to the selected ball color
  glUniform4f(colorLoc, ballColor.r, ballColor.g, ballColor.b, 1.0f);

  // Render the ball
  m_sphere.paint();

  // Reset the model matrix for the rope and pole
  modelMatrix = glm::mat4(1.0f);
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  // Set the color to white for the rope and pole
  glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

  // Set the line width
  glLineWidth(2.0f);

  // Render the rope (from top of the pole to the ball)
  glm::vec3 ropeStart(0.0f, poleHeight, 0.0f);
  glm::vec3 ropeEnd(x, y, z);
  m_line.paint(ropeStart, ropeEnd);

  // Render the pole
  glm::vec3 poleStart(0.0f, 0.0f, 0.0f);
  glm::vec3 poleEnd(0.0f, poleHeight, 0.0f);
  m_line.paint(poleStart, poleEnd);
}