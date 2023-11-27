#include"Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
{
    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = textures;

    // Now that we have all the required data, set the vertex buffers and its attribute pointers.
	VAO.Bind();
    // Generate an array for the vertices
	VBO VBO(vertices); 
	EBO EBO(indices);

	// Linking Vertex Attributes
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // Vertex Coordinates
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float))); // Vertex Normals
	VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float))); // Vertex Colors
	VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float))); // Texture Coordinates

    // Unbind all 
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void Mesh::Draw
(
    Shader& shader,
    Camera& camera, 
    glm::mat4 matrix,
    glm::vec3 translation,
    glm::quat rotation,
    glm::vec3 scale
)
{
    // Set the shader as active
    shader.Activate();
    
    // Bind the VAO
    VAO.Bind();

    // Create counters for the number of diffuse and specular textures
    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;

    // Loop through all the textures and bind them to their respective texture units
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
        // Retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string type = textures[i].type;
        if(type == "diffuse")
        {
            number = std::to_string(numDiffuse++);
        }
        else if(type == "specular")
        {
            number = std::to_string(numSpecular++);
        }
        textures[i].texUnit(shader, (type + number).c_str(), i);
        textures[i].Bind();
    }
    // Setting the camera matrices
    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    camera.Matrix(shader, "camMatrix");

    // Setting transformation matrices
    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);

    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(scale);

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

    // Draw the mesh
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}