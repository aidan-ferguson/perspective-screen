#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

int CreateShaderFromFile(std::string vertex_filename, std::string fragment_filename);
std::string ReadShaderFile(std::string filename);
void CompileShader(int shader, std::string shader_code);
void SetUniformVec3(int program_id, std::string name, glm::vec3* value);
void SetUniformMat4(int program_id, std::string name, glm::mat4* value);
void SetUniformInt(int program_id, std::string name, int value);