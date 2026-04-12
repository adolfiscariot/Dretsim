#include <iostream>
#include <vector>
#include <random>
#include <cmath>

class Simulation{

	public:
		Simulation(int count): 
			particles(count), 
			gen(ran_dev()), 
			dist(-0.5f, 0.5f), 
			wind_noise(-0.01f, 0.01f),
			rand_colour(0.0f, 1.0f)
		{
			set_coordinates();
			set_colours();
		}

		// update particles position
		void update_particles(float dt){

			/*
			 * ======================================
			 * 1. APPLY FORCES TO VELOCITY
			 * ======================================
			 */

			for (int i = 0; i < count; i++){
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

			/*
			 * O(n^2) loop as each particle measures it's distance from all other
			 * particles. The square of this distance is used to find the force
			 * to be applied via inverse-square law.This force is applied to both 
			 * particles as per Newton's 3rd law: each force begets an equal and 
			 * opposite force. Attraction & Repulsion are both implemented.
			 */

			for (size_t i = 0; i < count); i++){
				for (size_t j = i + 1; j < count; j++){
					float dist_x = x[j] - x[i];
					float dist_y = y[j] - y[i];

					float dist_sqr = (dist_x * dist_x) + (dist_y * dist_y);
					if (dist_sqr > 0.0001f){ // avoid division by 0
						float dist = sqrt(dist_sqr);

						float force;
						if (dist_sqr < 0.05f){
							force = REP_STRENGTH / dist_sqr;
						} else{
							force = ATTR_STRENGTH / dist_sqr;
						}

						float fx = (dist_x / dist) * force;
						float fy = (dist_y / dist) * force;

						vx[i] += fx  * dt;
						vy[i] += fy  * dt;
						vx[j] -= fx  * dt;
						vy[j] -= fy  * dt;
					}
				}
			}

			/*
			 * ======================================
			 * 3. UPDATE PARTICLE POSITION
			 * ======================================
			 */

			for (int i = 0; i < count; i++){
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

			/*
			 * ======================================
			 * 4. UPDATE PARTICLE COLOUR
			 * ======================================
			 */
			
			for (int i = 0; i < count; i++){
				r[i] = rand_colour(gen);
				g[i] = rand_colour(gen);
				b[i] = rand_colour(gen);
			}
		}

		const std::vector<Particle> &get_particles() const{
			return particles;
		}

		// Size of a Particle
		const size_t get_particle_size() const{
			return sizeof(Particle);
		}

		// Size of particles vector
		const size_t get_particles_count() const{
			return particles.size();
		}

		const Particle *get_particles_data() const{
			return particles.data();
		}

	private:

		std::vector<float> x, y, z;
		std::vector<float> vx, vy;
		std::vector<float> r, g, b;	

		// set particles start point coordinates
		void set_coordinates(){
			for (int i = 0; i < count; i++){
				x[i] = 0.0f;
				y[i] = 0.0f;

				vx[i] = 0.0f;
				vy[i] = 0.0f;
			}
		}

		void set_colours(){
			for (int i = 0; i < count; i++){
				// All white

				r[i] = 1.0f;
				g[i] = 1.0f;
				b[i] = 1.0f;
			}
		}

		// Particle list settings
		std::vector<Particle> particles;
		std::random_device ran_dev;
		std::mt19937 gen;
		std::uniform_real_distribution<float> dist;

		// Colour settings
		std::uniform_real_distribution<float> rand_colour;

		// Gravity settings
		const float GRAVITY = 0.1f;

		// Wind settings
		const float WIND_X = 0.05f;
		const float WIND_Y = 0.0f;
		std::uniform_real_distribution<float> wind_noise;

		// Attract to center settings
		const float PULL_MULTIPLIER = 0.001f;

		// Attraction & Repulsion settings. Close particles = strong repulsion & vice-versa.
		const float ATTR_STRENGTH = 0.0001f;
		const float REP_STRENGTH = -0.001f;
};
