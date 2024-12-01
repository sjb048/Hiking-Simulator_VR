#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>  // Ensure you have GLEW correctly set up
#include <GLFW/glfw3.h> // Ensure you have GLFW correctly set up

#include <string>
#include <unordered_map>

class Shader {
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();
    void use() const;
    GLuint getProgramID() const;
    bool isLoaded() const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
    std::string getErrorLog() const;
    bool checkCompileErrors(unsigned int shader, const std::string &type);
    void linkProgram(GLuint vertexShader, GLuint fragmentShader);

private:
    GLuint programID;
    bool loaded;
    std::string errorLog;
    mutable std::unordered_map<std::string, GLint> uniformCache;
    unsigned int compileShader(const std::string& source, GLenum shaderType);
    GLint getUniformLocation(const std::string& name) const;
};

#endif
