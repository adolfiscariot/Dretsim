#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "../include/stb_image.h"
#include <vector>

const int PARTICLE_COUNT = 1000;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;



struct Particle{
	float x, y;    // Position
	float vx, vy;  // Velocity 
};

// List of particles of type Particle
std::vector<Particle> particles(PARTICLE_COUNT);

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

// Close window if user presses esc key
void processInput(GLFWwindow *window){
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}
}

// Update particle position
void update_particles(std::vector<Particle> &particles, float dt){

	for (Particle &p: particles){
		p.x += p.vx * dt;
		p.y += p.vy * dt;

		// X bounce
		if (p.x >= 1.0 && p.vx > 0.0f){p.x = 1.0f; p.vx = -p.vx;}
		if (p.x <= -1.0 && p.vy < 0.0f){p.x = -1.0f; p.vx = -p.vx;}

		// Y bounce
		if (p.y >= 1.0 && p.vy > 0.0f){p.y = 1.0f; p.vy = -p.vy;}
		if (p.y <= -1.0 && p.vy < 0.0f){p.y = -1.0f; p.vy = -p.vy;}
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // For resizing the screen

	// Retrieve glad's opengl function pointers by asking the graphics driver for their address
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD";
		return -1;
	}

	glEnable(GL_PROGRAM_POINT_SIZE); // important for gl_PointSize

	// Build and compile our shader program
	Shader ourShader("../src/vertex.glsl", "../src/fragment.glsl");

	// Set coordinates for particles
	for (int i = 0; i < particles.size(); i++){
		particles[i].x = -1.0f + 2.0f * (rand() / (float)RAND_MAX); 
		particles[i].y = -1.0f + 2.0f * (rand() / (float)RAND_MAX); 
		
		float rx = -1.0f + 2.0f * (rand() / (float)RAND_MAX); 
		float ry = -1.0f + 2.0f * (rand() / (float)RAND_MAX); 

		particles[i].vx = rx * 1.0;
		particles[i].vy = ry * 1.0f;
	}

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
	glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)0);
	glEnableVertexAttribArray(0);

	double lastTime = glfwGetTime();

	while(!glfwWindowShouldClose(window)){
		double currentTime = glfwGetTime();
		float dt = (float)(currentTime - lastTime);
		lastTime = currentTime;

		processInput(window);

		update_particles(particles, dt);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), particles.data());

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, particles.size());

		// check for and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}


