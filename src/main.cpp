#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

// Close window if user presses esc key
void processInput(GLFWwindow *window){
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}
}

const char *vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";
	
int main(){
	// Initalize glfw library
	glfwInit();

	// State the version 3.3 and profile (core, no deprecated functions)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create an opengl window and context
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Retrieve glad's opengl function pointers by asking the graphics driver for their address
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD";
		return -1;
	}

	// Create dimensions 
	glViewport(0, 0, 800, 600);

	// Tell GLFW to call the above function everytime on resize
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Keep rendering until explicitly told to stop
	while (!glfwWindowShouldClose(window)){

		// input
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// check for and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/*
	 * ===========================================================
	 * VERTEX SHADER
	 * ===========================================================
	 */
	
	// Triangle co-ordinates
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	// Create a vertex buffer object and give it an ID
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// Bind created buffer to type of vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Copy user data into buffer's memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	// Create a vertex shader object
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Compile the source code for the vertex shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Test the compilation's success
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
	} else {
		std::cout << "WE GOOD BABY!!!\n";
	}

	/*
	 * ===========================================================
	 * FRAGMENT SHADER
	 * ===========================================================
	 */



	glfwTerminate();
	return 0;
}

