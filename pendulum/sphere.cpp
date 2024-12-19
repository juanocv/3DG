// sphere.cpp
#include "sphere.hpp"
#include <cmath>

void Sphere::create(GLuint program) {
  // Define the number of segments for latitude and longitude
  const unsigned int latitudeSegments = 40;
  const unsigned int longitudeSegments = 40;

  // Clear existing data
  m_vertices.clear();
  m_indices.clear();

  // Generate vertices
  for (unsigned int i = 0; i <= latitudeSegments; ++i) {
    float theta = i * glm::pi<float>() / latitudeSegments;
    float sinTheta = std::sin(theta);
    float cosTheta = std::cos(theta);

    for (unsigned int j = 0; j <= longitudeSegments; ++j) {
      float phi = j * 2.0f * glm::pi<float>() / longitudeSegments;
      float sinPhi = std::sin(phi);
      float cosPhi = std::cos(phi);

      glm::vec3 position;
      position.x = cosPhi * sinTheta;
      position.y = cosTheta;
      position.z = sinPhi * sinTheta;

      glm::vec3 normal = glm::normalize(position);

      m_vertices.push_back(Vertex{position, normal});
    }
  }

  // Generate indices
  for (unsigned int i = 0; i < latitudeSegments; ++i) {
    for (unsigned int j = 0; j < longitudeSegments; ++j) {
      unsigned int first = (i * (longitudeSegments + 1)) + j;
      unsigned int second = first + longitudeSegments + 1;

      m_indices.push_back(first);
      m_indices.push_back(second);
      m_indices.push_back(first + 1);

      m_indices.push_back(second);
      m_indices.push_back(second + 1);
      m_indices.push_back(first + 1);
    }
  }

  // Create and bind VAO
  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  // Create VBO and upload vertex data
  glGenBuffers(1, &m_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
               m_vertices.data(), GL_STATIC_DRAW);

  // Create EBO and upload index data
  glGenBuffers(1, &m_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint),
               m_indices.data(), GL_STATIC_DRAW);

  // Retrieve attribute locations
  m_positionLoc = glGetAttribLocation(program, "inPosition");
  m_normalLoc = glGetAttribLocation(program, "inNormal");

  // Set up vertex attribute pointers
  // Position attribute
  glEnableVertexAttribArray(m_positionLoc);
  glVertexAttribPointer(
      m_positionLoc,  // Attribute location
      3,              // Number of components (x, y, z)
      GL_FLOAT,       // Data type
      GL_FALSE,       // Normalized?
      sizeof(Vertex), // Stride (size of Vertex struct)
      reinterpret_cast<void *>(offsetof(Vertex, position)) // Offset to position
  );

  // Normal attribute
  glEnableVertexAttribArray(m_normalLoc);
  glVertexAttribPointer(
      m_normalLoc,    // Attribute location
      3,              // Number of components (x, y, z)
      GL_FLOAT,       // Data type
      GL_FALSE,       // Normalized?
      sizeof(Vertex), // Stride (size of Vertex struct)
      reinterpret_cast<void *>(offsetof(Vertex, normal)) // Offset to normal
  );

  // Unbind VAO
  glBindVertexArray(0);
}

void Sphere::paint() const {
  glBindVertexArray(m_VAO);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()),
                 GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

void Sphere::destroy() {
  glDeleteBuffers(1, &m_VBO);
  glDeleteBuffers(1, &m_EBO);
  glDeleteVertexArrays(1, &m_VAO);
}
