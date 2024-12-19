// sphere.hpp
#ifndef SPHERE_HPP_
#define SPHERE_HPP_

#include "abcgOpenGL.hpp"
#include <vector>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;
};

class Sphere {
public:
    void create(GLuint program);
    void paint() const;
    void destroy();

private:
    GLuint m_VAO{};
    GLuint m_VBO{};
    GLuint m_EBO{};

    GLint m_positionLoc{};
    GLint m_normalLoc{};

    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;
};

#endif
