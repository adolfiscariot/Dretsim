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
		std::vector<float> wind_x, wind_y;
		std::random_device ran_dev;
		std::mt19937 gen;
		std::uniform_real_distribution<float> rand_colour;
		const float GRAVITY = 0.1f;
		const float WIND_X = 0.05f;
		const float WIND_Y = 0.0f;
		std::uniform_real_distribution<float> wind_noise;
		const float PULL_MULTIPLIER = 0.001f;
		//void set_coordinates(){
		//	for (size_t i = 0; i < count; i++){
		//		x[i]  = 0.0f;
		//		y[i]  = 0.0f;
		//		vx[i] = 0.1f;
		//		vy[i] = 0.1f;
		//	}
		//}
		void set_coordinates(){
			std::uniform_real_distribution<float> pos(-1.0f, 1.0f);
			std::uniform_real_distribution<float> vel(-0.05f, 0.05f);
			for (size_t i = 0; i < count; i++){
				x[i]  = pos(gen);
				y[i]  = pos(gen);
				vx[i] = vel(gen);
				vy[i] = vel(gen);
			}
		}
		void set_colours(){
			static constexpr float colour_palette[3][3] = {
				{1.0f, 0.549f, 0.0f},  // orange
				{0.0f, 0.749f, 1.0f},  // blue
				{1.0f, 0.09f,  0.267f} // red
			};
			std::uniform_int_distribution<int> pick(0, 2);

			for (size_t i = 0; i < count; i++){
				int c = pick(gen);
				r[i] = colour_palette[c][0];
				g[i] = colour_palette[c][1];
				b[i] = colour_palette[c][2];
			}
		}

};
