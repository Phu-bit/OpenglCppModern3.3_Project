#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

// reads a file and returns the contents as a string
std::string get_file_contents(const char *filename);

class Shader
{
    public:
        GLuint ID; // the program ID
        Shader(const char* vertexFile, const char* fragmentFile);

        // activate the shader
        void Activate();
        void Delete();

    private:
        void compileErrors(unsigned int shader, const char* type); // checks for errors
};

#endif




