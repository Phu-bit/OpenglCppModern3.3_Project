#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<glad/glad.h>
#include "VBO.h"

class VAO
{
    public: // public since we need to access them from outside the class
        GLuint ID; // ID reference for the Vertex Array Object
        VAO(); // constructor that generates a VAO ID

        void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset); // links a VBO to the VAO using a certain layout
        void Bind(); // binds the VAO
        void Unbind();   // unbinds the VAO
        void Delete();  // deletes the VAO

};
#endif