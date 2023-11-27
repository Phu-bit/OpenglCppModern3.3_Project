#include"Model.h"
#include<math.h>

#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"


const unsigned int width = 800;
const unsigned int height = 800;

//Frame buffer rectangle vertices
float rectangleVertices[] =
{
	// Coords    // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};

float randf()
{
	return -1.0f + (rand() / (RAND_MAX / 2.0f));
}


int main()
{

	/**
	 *  Setup OpenGL context
	 */
	// Initialize GLFW
	glfwInit();
	// Tell GLFW what version of opengl we are using
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	const char* glsl_version = "#version 330";

	// Any Profile, so for a more optimized solution
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	// Create the window specs
	GLFWwindow* window = glfwCreateWindow(width, height, "Opengl Window", NULL, NULL);

	// Error Check window creation
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	// Introduce window into context
	glfwMakeContextCurrent(window);
	// Load Glad so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of Opengl
	glViewport(0, 0, width, height);
	
	/*
	* Generate Shaders
	*/
	Shader shaderProgram("Shaders/default/default.vert", "Shaders/default/default.frag");
	Shader frameBufferProgram("Shaders/framebuffer/framebuffer.vert", "Shaders/framebuffer/framebuffer.frag");
	Shader skyboxShader("Shaders/skybox/skybox.vert", "Shaders/skybox/skybox.frag");


	/**
	 * Light
	 * */  
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 lightModel = glm::mat4(1.0f); 
	lightModel = glm::translate(lightModel, lightPos);

	/**
	 * Setup Shader Uniforms
	*/
	// Activate the shader program and send the uniforms to the GPU
	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	frameBufferProgram.Activate();
	glUniform1i(glGetUniformLocation(frameBufferProgram.ID, "screenTexture"), 0);
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

	
	/**
	 * Setup OpenGL options
	 */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	/**
	 * Setup Camera
	 */
	Camera camera(width, height, glm::vec3(0.0f, 75.0f, 260.0f));
	// Camera camera(width, height, glm::vec3(-64.0f, 164.0f, 264.0f));

	/**
	 * Load models
	 * */ 
	Model model("Resources/models/asteroid/scene.gltf");
	Model jupiter("Resources/models/jupiter/scene.gltf");

	/**
	 * Load skybox
	 */
	// Prepare skybox VBO and VAO
	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// All the faces of the cubemap (make sure they are in this exact order)
	std::string facesCubemap[6] =
	{
		"Resources/spacebox/right.jpg",
		"Resources/spacebox/left.jpg",
		"Resources/spacebox/top.jpg",
		"Resources/spacebox/bottom.jpg",
		"Resources/spacebox/front.jpg",
		"Resources/spacebox/back.jpg"
	};

	// Creates the cubemap texture object
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

	/**
	 * Instancing Model
	*/

	// The number of asteroids to be created
	const unsigned int number = 1000;
	// Radius of circle around which asteroids orbit
	float radius = 100.0f;
	// How much ateroids deviate from the radius
	float radiusDeviation = 25.0f;

	// Holds all transformations for the asteroids
	glm::vec3 translations[number];
	glm::quat rotations[number];
	glm::vec3 scales[number];

	for (unsigned int i = 0; i < number; i++)
	{
		// Generates x and y for the function x^2 + y^2 = radius^2 which is a circle
		float x = randf();
		float finalRadius = radius + randf() * radiusDeviation;
		float y = ((rand() % 2) * 2 - 1) * sqrt(1.0f - x * x);

		// Makes the random distribution more even
		if (randf() > 0.5f)
		{
			// Generates a translation near a circle of radius "radius"
			translations[i] = glm::vec3(y * finalRadius, randf(), x * finalRadius);
		}
		else
		{
			// Generates a translation near a circle of radius "radius"
			translations[i] = glm::vec3(x * finalRadius, randf(), y * finalRadius);
		}

		// Generates random rotations
		rotations[i] = glm::quat(1.0f, randf(), randf(), randf());
		// Generates random scales
		scales[i] = 0.1f * glm::vec3(randf(), randf(), randf());
	}


	/*
	* Enable Vsync
	*/
	glfwSwapInterval(1);

	/**
	 * Render to Framebuffer
	 */
	// Prepare framebuffer rectangle VBO and VAO
	unsigned int rectVAO, rectVBO;
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// Create Frame Buffer Object
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Create Framebuffer Texture
	unsigned int framebufferTexture;
	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);  

	// Create Render Buffer Object
	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// Error checking framebuffer
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fboStatus << std::endl;

	/**
	 * Setup Dear ImGui context
	 */ 
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		/**
		 * Bind framebuffer, clear color, enable depth testing, and clear depth buffer
		 */
		glBindFramebuffer(GL_FRAMEBUFFER, FBO); // Bind the framebuffer so we can render to it
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // Clear the screen to a dark blue color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer and the depth buffer
		glEnable(GL_DEPTH_TEST); // Enable depth testing

		/**
		 * ImGui new frame
		*/
		ImGui_ImplOpenGL3_NewFrame(); 
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		/*
		* Camera input and matrix update
		*/ 
		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 1000.0f); // FOV, near plane, far plane

		/**
		 * draw models
		 * */ 

		// Draw jupiter
		jupiter.Draw(shaderProgram, camera);

		for (unsigned int i = 0; i < number; i++)
		{
			model.Draw(shaderProgram, camera, translations[i], rotations[i], scales[i]);
		}
		/**
		 * Draw skybox
		*/
		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);

		skyboxShader.Activate();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
		// The last row and column affect the translation of the skybox (which we don't want to affect)
		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
		// where an object is present (a depth of 1.0f will always fail against any object's depth value)
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);

		/**
		 * ImGui window
		*/
		ImGui::Begin("Light Controls");
		ImGui::SliderFloat3("Light Pos", &lightPos.x, 0.0f, 2.0f);
		// ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", camera.Position.x, camera.Position.y, camera.Position.z);
		ImGui::InputFloat3("Camera Position", glm::value_ptr(camera.Position));
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();

		/**
		 * Render to default framebuffer
		*/
		// Bind the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Draw the framebuffer rectangle
		frameBufferProgram.Activate(); // Activate the framebuffer shader
		glBindVertexArray(rectVAO); // Bind the rectangle VAO
		glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
		glDisable(GL_CULL_FACE); // prevents framebuffer rectangle from being discarded
		glBindTexture(GL_TEXTURE_2D, framebufferTexture); // Bind the framebuffer texture
		glDrawArrays(GL_TRIANGLES, 0, 6); // Draw the rectangle

		/**
		 * Render ImGui
		 * */ 
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);

		// Handle GLFW events
		glfwPollEvents();
	}


	/**
	 * Cleanup
	 */
	// Delete all the objects we've created
	shaderProgram.Delete();
	skyboxShader.Delete();
	glDeleteFramebuffers(1, &FBO);

	// Delete Dear ImGui
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	// Delete Window before ending program
	glfwDestroyWindow(window);
	glfwTerminate();
	

	return 0;
}
