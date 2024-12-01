#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
    : programID(0), loaded(false)
{
    // Load shader source code from files
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile(vertexPath), fShaderFile(fragmentPath);
    if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
        errorLog = "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ";
        return;
    }
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    vShaderFile.close();
    fShaderFile.close();

    // Compile shaders
    unsigned int vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);
    if (!vertexShader || !fragmentShader) {
        errorLog += "ERROR::SHADER::COMPILATION_FAILED\n";
        glDeleteProgram(programID);
        programID = 0;
        return;
    }

    // Link program
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    // Check for linking errors
    int success;
    char infoLog[1024];
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programID, 1024, NULL, infoLog);
        errorLog = "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(programID);
        programID = 0;
        return;
    }
    loaded = true;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() const {
    if (loaded) {
        glUseProgram(programID);
    } else {
        std::cerr << "ERROR::SHADER::USE_CALLED_ON_UNLOADED_SHADER: Shader program is not loaded.\n";
    }
}
// Destructor
Shader::~Shader() {
    if (programID > 0) {
        glDeleteProgram(programID);
    }
}

// Set uniform 4x4 matrix
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }
    else {
        std::cerr << "ERROR::SHADER::SETMAT4_ON_UNLOADED_SHADER\n";
    }
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform3fv(location, 1, &value[0]);
    }
}

void Shader::setFloat(const std::string& name, float value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void Shader::setInt(const std::string& name, int value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}


bool Shader::isLoaded() const {
    return loaded;
}

std::string Shader::getErrorLog() const {
    return errorLog;
}



unsigned int Shader::compileShader(const std::string& source, GLenum shaderType) {
    unsigned int shader;
    shader = glCreateShader(shaderType);
    const char* shaderCode = source.c_str();
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);

    // Check for compile errors
    int success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::string type = shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
        errorLog = "ERROR::SHADER::" + type + "::COMPILATION_FAILED\n" + std::string(infoLog);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}


void Shader::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

bool Shader::checkCompileErrors(unsigned int shader, const std::string &type) {
    int success;
    char infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
            return false;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n";
            return false;
        }
    }
    return true;
}
GLint Shader::getUniformLocation(const std::string& name) const {
    // Check if the location is already cached
    auto it = uniformCache.find(name);
    if (it != uniformCache.end()) {
        return it->second;
    }

    // Retrieve the location from the shader program
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader program!" << std::endl;
    }

    // Cache the location for future use
    uniformCache[name] = location;

    return location;
}
