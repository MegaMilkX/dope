#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>

#include "gl/glextutil.h"

class ShaderProgram
{
public:
    ShaderProgram()
    {
        program = glCreateProgram();
    }
    ~ShaderProgram()
    {
        glDeleteProgram(program);
    }

    void VertexShader(const std::string& source)
    {
        const char* csource = source.c_str();
        vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &csource, 0);
        glCompileShader(vs);

        GLint Result = GL_FALSE;
        int InfoLogLength;

        glGetShaderiv(vs, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &InfoLogLength);

        if (InfoLogLength > 1)
        {
            std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(vs, InfoLogLength, NULL, &ShaderErrorMessage[0]);
            std::cout << &ShaderErrorMessage[0] << std::endl;
        }

        glAttachShader(program, vs);
    }

    void FragmentShader(const std::string& source)
    {
        const char* csource = source.c_str();
        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &csource, 0);
        glCompileShader(fs);

        GLint Result = GL_FALSE;
        int InfoLogLength;

        glGetShaderiv(fs, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &InfoLogLength);

        if (InfoLogLength > 1)
        {
            std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(fs, InfoLogLength, NULL, &ShaderErrorMessage[0]);
            std::cout << &ShaderErrorMessage[0] << std::endl;
        }

        glAttachShader(program, fs);
    }

    void Link()
    {
        glBindAttribLocation(program, 0, "Position");
        glBindAttribLocation(program, 1, "UV");

        glBindFragDataLocation(program, 0, "outDiffuse");

        glLinkProgram(program);

        

        

        Bind();

        glUniform1i(glGetUniformLocation(program, "Diffuse"), 0);
    }

    void Bind()
    {
        glUseProgram(program);
    }

    GLint UniformLocation(const std::string& name) { return glGetUniformLocation(program, name.c_str()); }

private:
    GLuint program;
    GLuint vs;
    GLuint fs;
};

#endif
