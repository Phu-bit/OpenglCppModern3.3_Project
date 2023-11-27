#version 330 core

// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

// Output data ; will be interpolated for each fragment.
out vec3 crntPos;
out vec3 normal;
out vec3 vColor; 
out vec2 texCoord;

uniform mat4 camMatrix; 
uniform mat4 model;

uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;


void main()
{
   // Calculate current position
   crntPos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f)); // position of the vertex in world space

   // Assign normal from vertex data to normal
   normal = aNormal;
   // Assign color from vertex data to vColor
   vColor = aColor; 
   // Assign texture coordinates from vertex data to texCoord
   texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;  
   
   gl_Position = camMatrix * vec4(aPos, 1.0f); // position of the vertex in camera space
}
