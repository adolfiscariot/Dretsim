#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include "hashgrid.h"
#include "simulation.h"

Simulation::Simulation(size_t count, HashGrid &grid): 
	count(count),
	grid(grid),
	gen(ran_dev()),
	wind_noise(-0.01f, 0.01f),
	rand_colour(0.0f, 1.0f)
{
	x.resize(count);
	y.resize(count);
	vx.resize(count);
	vy.resize(count);
	r.resize(count);
	g.resize(count);
	b.resize(count);

	set_coordinates();
	set_colours();

	pack_gpu_buffer();
}

// update particles position
void Simulation::update_particles(float dt){

	/*
	 * ======================================
	 * 1. APPLY FORCES TO VELOCITY
	 * ======================================
	 */

	for (size_t i = 0; i < count; i++){
		// Gravity
		vy[i] += -GRAVITY * dt; 

		// Wind 
		vx[i] += (WIND_X + wind_noise(gen)) * dt;
		vy[i] += (WIND_Y + wind_noise(gen)) * dt;

		// Attract to center
		float dx = 0.0f - x[i];
		float dy = 0.0f - y[i];

		/* 
		 * Pull multiplier ensures the force gets stronger as distances 
		 * get smaller
		 */

		vx[i] += dx * PULL_MULTIPLIER * dt;
		vy[i] += dy * PULL_MULTIPLIER * dt;
	}

	/*
	 * ======================================
	 * 2. INTER-PARTICLE ATTRACTION
	 * ======================================
	 */

	grid.build(x, y);
	grid.query(x, y, vx, vy);

	/*
	 * ======================================
	 * 3. UPDATE PARTICLE POSITION
	 * ======================================
	 */

	for (size_t i = 0; i < count; i++){
		x[i] += vx[i] * dt;
		y[i] += vy[i]  * dt;

		// Bounce off walls
		if (x[i] >= 1.0f && vx[i] > 0.0f){
			x[i] = 1.0f;
			vx[i] = -vx[i];
		}
		if (x[i] <= -1.0f && vx[i] < 0.0f){
			x[i] = -1.0f;
			vx[i] = -vx[i];
		}
		if (y[i] >= 1.0f && vy[i] > 0.0f){
			y[i] = 1.0f;
			vy[i] = -vy[i];
		}
		if (y[i] <= -1.0f && vy[i] < 0.0f){
			y[i] = -1.0f;
			vy[i] = -vy[i];
		}
	}
}


/*
* Re-package the position and color vectors
* for the GPU's VBO
*/	
void Simulation::pack_gpu_buffer(){
	gpu_buffer.resize(count * 5);

	for (int i = 0; i < count; i++){
		gpu_buffer[i*5 + 0] = x[i];
		gpu_buffer[i*5 + 1] = y[i];
		gpu_buffer[i*5 + 2] = r[i];
		gpu_buffer[i*5 + 3] = g[i];
		gpu_buffer[i*5 + 4] = b[i];
	}
}


const std::vector<float> &Simulation::get_gpu_buffer() const{
	return gpu_buffer;
}

// Size of a stride(x, y, r, g, b)
const size_t Simulation::get_gpu_stride() {
	return sizeof(float) * 5;
}

// Num of particles in gpu_buffer vector
const size_t Simulation::get_gpu_particle_count() const{
	return gpu_buffer.size() / 5;
}

const float *Simulation::get_gpu_buffer_data() {
	return gpu_buffer.data();
}
