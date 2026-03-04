#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "../include/stb_image.h"
#include <vector>
#include <random>
#include <chrono>
#include "simulation.cpp"
#include <algorithm>

const int PARTICLE_COUNT = 1000;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const double FIXED_DT = 1.0f / 60.0f; //How often we do our physics updates

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
		std::cout << "Failed to create GLFW window";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // For resizing the screen

	// Retrieve glad's opengl function pointers by asking the graphics driver for their address
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD";
		return -1;
	}

	glEnable(GL_PROGRAM_POINT_SIZE); // important for gl_PointSize

	// Build and compile our shader program
	Shader ourShader("../src/vertex.glsl", "../src/fragment.glsl");

	Simulation sim(PARTICLE_COUNT);
	const std::vector<Particle> &particles = sim.get_particles();
	size_t particleSize = sim.get_particle_size();
	size_t particlesCount = sim.get_particles_count();
	const Particle *particlesData = sim.get_particles_data();

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

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, particleSize, (void *)0);
	glEnableVertexAttribArray(0);

	double accumulator = 0.0f; // Timer for updating particles based on frame time
	double lastTime = glfwGetTime();

	std::vector<uint64_t> renderTimes;
	std::vector<uint64_t> updateTimes;
	std::vector<uint64_t> totalTimes;

	double printAccumulator = 0.0f; // Timer for printing render and update times

	while(!glfwWindowShouldClose(window)){
		// Simulation should happen 60 times per second regardless of the machine's frame rate
		double currentTime = glfwGetTime();
		double frameTime = currentTime - lastTime;
		lastTime = currentTime;

		accumulator += frameTime;
		processInput(window);

		auto startCounting = std::chrono::high_resolution_clock::now();

		while (accumulator >= FIXED_DT){
			auto updateStart = std::chrono::high_resolution_clock::now();
			sim.update_particles(FIXED_DT);
			auto updateStop = std::chrono::high_resolution_clock::now();
			auto updateDifference = std::chrono::duration_cast<std::chrono::microseconds>(updateStop - updateStart).count();
			updateTimes.push_back(updateDifference);
			accumulator -= FIXED_DT;
		}
		printAccumulator += frameTime;

		auto renderStart = std::chrono::high_resolution_clock::now();
		const Particle *particlesData = sim.get_particles_data();

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * particleSize, particlesData);

		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, particlesCount);

		// check for and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		auto renderStop = std::chrono::high_resolution_clock::now();
		auto renderDifference = std::chrono::duration_cast<std::chrono::microseconds>(renderStop - renderStart).count();
		renderTimes.push_back(renderDifference);

		if (!updateTimes.empty()){
			auto lastUpdateTime = updateTimes.back();
			auto totalTime = renderDifference + lastUpdateTime;
			totalTimes.push_back(totalTime);
		}

		auto stopCounting = std::chrono::high_resolution_clock::now();
		auto countingDifference = std::chrono::duration_cast<std::chrono::microseconds>(stopCounting - startCounting).count();

		// If it's been a second or more display the render, update and total times as well
		// as the 50th and 99th percentile of those times thus far in the program
		if (printAccumulator >= 1.0f){
			std::cout << "Render: " << renderTimes.back() << " microseconds \n";
			std::cout << "Update: " << updateTimes.back() << " microseconds \n";
			std::cout << "Total: " << totalTimes.back() << " microseconds \n";
			printAccumulator = 0.0f;
		
			std::sort(renderTimes.begin(), renderTimes.end());
			std::sort(updateTimes.begin(), updateTimes.end());
			std::sort(totalTimes.begin(), totalTimes.end());

			if (!renderTimes.empty()){
				size_t p99RenderIndex = (size_t)(renderTimes.size() * 0.99);
				size_t midRenderIndex = renderTimes.size() / 2;
				std::cout << "Render P99: " << renderTimes[p99RenderIndex] << " microseconds\n";
				std::cout << "Render P50: " << renderTimes[midRenderIndex] << " microseconds\n";
			}

			if (!updateTimes.empty()){
				size_t p99UpdateIndex = (size_t)(updateTimes.size() * 0.99);
				size_t midUpdateIndex = updateTimes.size() / 2;
				std::cout << "Update P99: " << updateTimes[p99UpdateIndex] << " microseconds\n";
				std::cout << "Update P50: " << updateTimes[midUpdateIndex] << " microseconds\n";
			}

			if (!totalTimes.empty()){
				size_t p99TotalIndex = (size_t)(totalTimes.size() * 0.99);
				size_t midTotalIndex = totalTimes.size() / 2;
				std::cout << "Total P99: " << totalTimes[p99TotalIndex] << " microseconds\n";
				std::cout << "Total P50: " << totalTimes[midTotalIndex] << " microseconds\n";
			}

		}
	}

	glfwTerminate();
	return 0;
}


