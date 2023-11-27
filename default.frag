#version 330 core

out vec4 FragColor;


in vec3 crntPos; // the input current position from the vertex shader
in vec3 normal; // the input normal vector from the vertex shader
in vec3 vColor;  // the input per vertex color from the vertex shader
in vec2 texCoord; // the input texture coordinate from the vertex shader

uniform sampler2D diffuse0; // the texture sampler
uniform sampler2D specular0; // the spec map 

uniform vec4 lightColor; // the light color
uniform vec3 lightPos; // the light position 
uniform vec3 camPos; // the camera position 

float ambient = 0.20f; // ambient lighting amount

vec4 pointLight(){

   // calculate attenuation
   vec3 lightVec = lightPos - crntPos; // calculate distance (lightVec) between light source and fragment position
   float dist = length(lightVec); // calculate distance between light source and fragment position
   float a = 0.05f; // attenuation quadratic factor
   float b = 0.01f; // attenuation linear factor
   float intensity = 1.0f / (a * dist * dist + b * dist + 1.0f); // calculate attenuation

   // calculate diffuse lighting amount
   vec3 norm = normalize(normal); // normalize because interpolation can cause the vector to not be unit length
   vec3 lightDir = normalize(lightVec); // calculate distance (lightDir) between light source and fragment position
   float diff = max(dot(normal, lightDir), 0.0); // calculate diffuse lighting amount

   // calculate specular lighting amount
   float specularLight = 0.50f; // specular lighting amount
   vec3 viewDir = normalize(camPos - crntPos); // calculate distance (viewDir) between camera and fragment position
   vec3 reflectDir = reflect(-lightDir, norm); // calculate reflection vector
   float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), 32); // calculate specular lighting amount
   float specular = specularLight * specAmount; // calculate specular lighting amount

   return (texture(diffuse0, texCoord) * (diff * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lightColor ; // calculate final color

}

vec4 directionalLight()
{
	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// diffuse lighting
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - crntPos);
	float diff = max(dot(norm, lightDir), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDirection, reflectDir), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDir);
	float intensity = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(diffuse0, texCoord) * (diff * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lightColor;
}

float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth)
{
	float steepness = 0.5f;
	float offset = 5.0f;
	float zVal = linearizeDepth(depth);
	return (1 / (1 + exp(-steepness * (zVal - offset))));
}

void main()
{
	// outputs final color
	FragColor = directionalLight();
}