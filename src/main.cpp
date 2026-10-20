#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "../include/stb_image.h"
#include <vector>
#include <random>
#include <chrono>
#include "simulation.h"
#include <algorithm>
#include "hashgrid.h"

constexpr size_t PARTICLE_COUNT = 1000;
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr float FIXED_DT = 1.0f / 60.0f; //How often we do our physics updates
constexpr int MAX_STEPS_PER_FRAME = 4;

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

// Close window if user presses esc key
void processInput(GLFWwindow *window){
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}
}

int main(){

	// Initalize glfw library
	glfwInit();

	// State the version 3.3 and profile (core, no deprecated functions)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create an opengl window and context
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "DRETSIM", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // For resizing the screen

	// Retrieve glad's opengl function pointers by asking the graphics driver for their address
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD";
		return -1;
	}

	glEnable(GL_PROGRAM_POINT_SIZE); // important for gl_PointSize

	// Build and compile our shader program
	Shader ourShader("../src/vertex.glsl", "../src/fragment.glsl");

	HashGrid hashgrid(PARTICLE_COUNT, FIXED_DT);
	Simulation sim(PARTICLE_COUNT, hashgrid);
	const std::vector<float> &particles = sim.get_gpu_buffer();
	size_t particleSize = sim.get_gpu_stride();
	size_t particlesCount = sim.get_gpu_particle_count();
	const float *particlesData = sim.get_gpu_buffer_data();

	/*
	 * ===========================================================
	 * CONFIGURE VERTEX DATA, BUFFERS & ATTRIBUTES
	 * ===========================================================
	 */

	unsigned int VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, particlesCount * particleSize, particlesData, GL_DYNAMIC_DRAW);

	// Position Attributes.
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, particleSize, (void *)0);
	glEnableVertexAttribArray(0);

	// Color Attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, particleSize, (void *) (2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	double accumulator = 0.0f; // Timer for updating particles based on frame time
	double lastTime = glfwGetTime();

	while(!glfwWindowShouldClose(window)){
		double currentTime = glfwGetTime();
		// Cap elapsed time to 100ms because too large of an elapsed time means
		// more physics updates to do 
		double frameTime = std::min(currentTime - lastTime, 0.1);
		lastTime = currentTime;

		accumulator += frameTime;
		processInput(window);

		// We're sacrificing perfect synchronization with responsiveness by
		// capping the number of physics updates done at any one time to 4.
		// Assume the CPU is slow and the accumulator grows to 200, we'd need
		// to run about 12 physics updates which will only slow the system even
		// more creating this nasty loop. Instead we say no more than 4 updates
		// and if there are any more, forget about them.
		int steps = 0;
		while (accumulator >= FIXED_DT && steps < MAX_STEPS_PER_FRAME){
			sim.update_particles(FIXED_DT);
			accumulator -= FIXED_DT;
			++steps;
		}

		if (steps == MAX_STEPS_PER_FRAME) accumulator = 0.0;

		sim.pack_gpu_buffer();
		const float *particlesData = sim.get_gpu_buffer_data();

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * particleSize, particlesData);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, particlesCount);

		// check for and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}



