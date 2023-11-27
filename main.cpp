#include"Model.h"

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
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	/**
	 * Load models
	 * */ 
	Model ground("Resources/models/sword/scene.gltf");
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
		ground.Draw(shaderProgram, camera);

		/**
		 * ImGui window
		*/
		ImGui::Begin("Light Controls");
		ImGui::SliderFloat3("Light Pos", &lightPos.x, 0.0f, 2.0f);
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
