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
	GLFWwindow* window = glfwCreateWindow(800, 600, "WANTAM!", NULL, NULL);
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

	/*
	 * ===========================================================
	 * VERTEX SHADER
	 * ===========================================================
	 */
	
	// Rectangle co-ordinates
	float vertices[] = {
		 0.5f,  0.5f, 0.0f, // top right
		 0.5f, -0.5f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f  // top left
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	// Create a vertex buffer object and give it an ID
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int EBO; // Element Buffer Obj. Useful for storing indices (for multiple triangles)
	glGenBuffers(1, &EBO);  

	// Bind created buffer to type of vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


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
	}

	/*
	 * ===========================================================
	 * FRAGMENT SHADER (Calculates colour output of pixels)
	 * ===========================================================
	 */
	
	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
			"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n";
	
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);


	/*
	 * ===========================================================
	 * SHADER PROGRAM (Final linked version of multiple shaders)
	 * ===========================================================
	 */
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	// Link previously compiled shaders to above program object
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check if linking worked
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success){
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	} 

	// Tell opengl how to interprex vector input
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Activate newly created program object
	glUseProgram(shaderProgram);

	// Delete shader objects after linking. We don't need them any more
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Generate a vertex attribute object and give it an ID
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// Bind VAO to VBO
	glBindVertexArray(VAO);

	// Copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set our vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);

	// Keep rendering until explicitly told to stop
	while (!glfwWindowShouldClose(window)){

		// input
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw the object 
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// check for and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

