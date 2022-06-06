#include "Shader.h"

int CreateShaderFromFile(std::string vertex_filename, std::string fragment_filename)
{
	int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string v_shader_code = ReadShaderFile(vertex_filename);
	std::string f_shader_code = ReadShaderFile(fragment_filename);
	
	CompileShader(vertex_shader, v_shader_code);
	CompileShader(fragment_shader, f_shader_code);

	int program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader);
	glAttachShader(program_id, fragment_shader);
	glLinkProgram(program_id);

	// Check for linking errors
	int linkSuccess;
	glGetProgramiv(program_id, GL_LINK_STATUS, &linkSuccess);
	if (!linkSuccess) {
		std::cout << "Shaders failed to link" << std::endl;

		// Get the error buffer length
		int errorLength;
		glGetShaderiv(program_id, GL_INFO_LOG_LENGTH, &errorLength);
		char* error = new char[errorLength];

		// Get the error and print it to the console
		glGetShaderInfoLog(program_id, errorLength, NULL, error);
		std::cout << error << std::endl;

		assert(false);
	}

	// The Shaders can now be deleted as they have been compiled and linked
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program_id;
}

std::string ReadShaderFile(std::string filename) {
	std::ifstream shaderFile;
	std::stringstream shaderStream;

	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		shaderFile.open(filename);
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		return shaderStream.str();
	}
	catch (std::ifstream::failure& error) {
		std::cout << "Shader read file failed (" << filename << "): " << error.code() << std::endl;
		assert(false);
	}
}

void CompileShader(int shader, std::string shader_code)
{
	const char* new_shader_code = shader_code.c_str();

	// Attempt compilation
	glShaderSource(shader, 1, &new_shader_code, NULL);
	glCompileShader(shader);

	// Validate compilation
	int successful_compilation;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &successful_compilation);
	if (!successful_compilation) {
		// Compilation failed, get the shader type
		int shaderType;
		glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);
		if (shaderType == GL_VERTEX_SHADER) {
			std::cout << "Vertex Shader failed to compile" << std::endl;
		}
		else {
			std::cout << "Fragment Shader failed to compile" << std::endl;
		}

		// Get the error buffer length
		int errorLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorLength);
		char* error = new char[errorLength];

		// Get the error and print it to the console
		glGetShaderInfoLog(shader, errorLength, NULL, error);
		std::cout << error << std::endl;

		assert(false);
	}
}

void SetUniformVec3(int program_id, std::string name, glm::vec3* value) {
	glUniform3fv(glGetUniformLocation(program_id, name.c_str()), 1, glm::value_ptr(*value));
}

void SetUniformMat4(int program_id, std::string name, glm::mat4* value) {
	glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, &(*value)[0][0]);
}

void SetUniformInt(int program_id, std::string name, int value) {
	glUniform1i(glGetUniformLocation(program_id, name.c_str()), value);
}