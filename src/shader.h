#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // get all opengl headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
	public:
		// the program ID
		unsigned int ID;

		// constructor reads and builds the shader
		Shader(const char *vertexPath, const char *fragmentPath){
			// retrieve the vertex/fragment source code from filepath
			std::string vertexCode;
			std::string fragmentCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;

			// ensure ifstream objects can throw exceptions
			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			try{
				// open files
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);
				std::stringstream vShaderStream, fShaderStream;

				// read file's buffer content into streams
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();

				// close file handlers
				vShaderFile.close();
				fShaderFile.close();

				// convert stream into string
				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
			}
			catch(std::ifstream::failure e){
				std::cout << "ERROR::SHADER::FILE NOT SUCCESSFULLY READ\n";
			}
			const char *vShaderCode = vertexCode.c_str();
			const char *fShaderCode = fragmentCode.c_str();

			// compile shaders
			unsigned int vertex, fragment;
			int success;
			char infoLog[512];

			// vertex shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);

			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (!success){
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION FAILED\n" << infoLog;
			}

			// fragment shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);

			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if (!success){
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION FAILED\n" << infoLog;
			}

			// shader program
			ID = glCreateProgram();
			glAttachShader(ID, vertex);
			glAttachShader(ID, fragment);
			glLinkProgram(ID);
			glGetProgramiv(ID, GL_LINK_STATUS,&success);
			if (!success){
				glGetProgramInfoLog(ID, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::LINKING FAILED\n" << infoLog;
			}

			//delete shaders 
			glDeleteShader(vertex);
			glDeleteShader(fragment);

		}

		// use/activate the shader
		void use(){
			glUseProgram(ID);
		}

		// utility uniform functions
		void setBool(const std::string &name, bool value) const{
			glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
		}
		void setInt(const std::string &name, int value) const{
			glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
		}
		void setFloat(const std::string &name, float value) const{
			glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
		}
	private:
		void checkCompileErrors(unsigned int shader, std::string type){
			int success;
			char infoLog[1024];
			if (type != "PROGRAM"){
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success){
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "ERROR::SHADER COMPILATION ERROR OF TYPE: " << type << "\n" << infoLog << "\n";
				}
		} else{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success){
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM LINKING ERROR OF TYPE: " << type << "\n" << infoLog << "\n";

			}
		};
	};
};

#endif
