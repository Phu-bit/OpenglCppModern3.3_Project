#include"Model.h"

#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"


const unsigned int width = 800;
const unsigned int height = 800;

int main()
{
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





	// Generate Shader
	Shader shaderProgram("default.vert", "default.frag");

	// Light 
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 lightModel = glm::mat4(1.0f); 
	lightModel = glm::translate(lightModel, lightPos);

	// Activate the shader program and send the uniforms to the GPU
	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);






	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	// Set up our camera
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	// Load models
	Model ground("models/ground/scene.gltf");
	Model grass("models/grass/scene.gltf");


	// Setup Dear ImGui context
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
		// Clear color and depth buffer
		glClearColor(0.85f, 0.85f, 0.90f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Start new ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Camera input and matrix update
		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 1000.0f); // FOV, near plane, far plane

		ground.Draw(shaderProgram, camera);
		grass.Draw(shaderProgram, camera);


		// ImGui window for light control
		ImGui::Begin("Light Controls");
		ImGui::SliderFloat3("Light Pos", &lightPos.x, 0.0f, 2.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);

		// Handle GLFW events
		glfwPollEvents();
	}


	// Delete all the objects we've created
	shaderProgram.Delete();

	// Delete Dear ImGui
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	// Delete Window before ending program
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
