#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../path.h"

namespace Viewer
{
	class Shader
	{
	public:
		Shader(const std::string& vertexPath, const std::string& fragmentPath)
		{
			root = Path::Root({ "Rasterizer", "Assets", "Shaders" });
			
			std::string vertexCode;
			std::string fragmentCode;

			std::ifstream vShaderFile;
			std::ifstream fShaderFile;

			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			try
			{
				vShaderFile.open(root / vertexPath);
				fShaderFile.open(root / fragmentPath);

				std::stringstream vShaderStream, fShaderStream;

				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();

				vShaderFile.close();
				fShaderFile.close();

				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
			}
			catch (std::ifstream::failure& e)
			{
				std::cout << "[SHADER] One of the shaders failed to load!" << std::endl;
			}

			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();

			unsigned int vertex, fragment;

			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, nullptr);
			glCompileShader(vertex);
			CheckCompileErrors(vertex, "VERTEX");

			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, nullptr);
			glCompileShader(fragment);
			CheckCompileErrors(fragment, "FRAGMENT");

			ID = glCreateProgram();
			glAttachShader(ID, vertex);
			glAttachShader(ID, fragment);

			glLinkProgram(ID);
			CheckCompileErrors(ID, "PROGRAM");

			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}

		void Use() const
		{
			glUseProgram(ID);
		}

	private:
		unsigned int ID;
		std::filesystem::path root;

		static void CheckCompileErrors(GLuint shader, std::string type)
		{
			GLint success;
			GLchar infoLog[1024];
			if (type != "PROGRAM")
			{
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
					std::cout << "[SHADER] Program compile error " << type << "\n" << infoLog << std::endl;
				}
			}
			else
			{
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
					std::cout << "[SHADER] Shader link error " << type << "\n" << infoLog << std::endl;
				}
			}
		}
	};
}
