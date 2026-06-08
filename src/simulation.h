#pragma once
#include <vector>
#include <random>
#include "hashgrid.h"

class Simulation{
	public:
		Simulation(size_t count, HashGrid &grid);
		void update_particles(float dt);
		void pack_gpu_buffer();
		const std::vector<float> &get_gpu_buffer() const;
		const size_t get_gpu_stride();
		const size_t get_gpu_particle_count() const;
		const float *get_gpu_buffer_data();
	private:
		std::vector<float> gpu_buffer;
		size_t count;
		HashGrid &grid;
		std::vector<float> x, y;
		std::vector<float> vx, vy;
		std::vector<float> r, g, b;
		std::random_device ran_dev;
		std::mt19937 gen;
		std::uniform_real_distribution<float> rand_colour;
		const float GRAVITY = 0.1f;
		const float WIND_X = 0.05f;
		const float WIND_Y = 0.0f;
		std::uniform_real_distribution<float> wind_noise;
		const float PULL_MULTIPLIER = 0.001f;
		void set_coordinates(){
			for (size_t i = 0; i < count; i++){
				x[i]  = 0.0f;
				y[i]  = 0.0f;
				vx[i] = 0.1f;
				vy[i] = 0.1f;
			}
		}
		void set_colours(){
			for (size_t i = 0; i < count; i++){
				r[i] = rand_colour(gen);
				g[i] = rand_colour(gen);
				b[i] = rand_colour(gen);
			}
		}

};
