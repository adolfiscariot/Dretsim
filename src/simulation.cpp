#include <iostream>
#include <vector>
#include <random>
#include <cmath>

struct Particle{
	float x, y;
	float vx, vy;
};

class Simulation{

	public:
		Simulation(int count): 
			particles(count), 
			gen(ran_dev()), 
			dist(-1.0f, 1.0f), 
			wind_noise(-0.01f, 0.01f)
		{
			set_coordinates();
		}

		// update particles position
		void update_particles(float dt){

			/*
			 * ======================================
			 * 1. APPLY FORCES TO VELOCITY
			 * ======================================
			 */

			for (Particle &p : particles){
				// Gravity
				p.vy += -GRAVITY * dt; 

				// Wind 
				p.vx += (WIND_X + wind_noise(gen)) * dt;
				p.vy += (WIND_Y + wind_noise(gen)) * dt;

				// Attract to center
				float dx = 0.0f - p.x;
				float dy = 0.0f - p.y;

				/* 
				 * Pull multiplier ensures the force gets stronger as distances 
				 * get smaller
				 */

				p.vx += dx * PULL_MULTIPLIER * dt;
				p.vy += dy * PULL_MULTIPLIER * dt;
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

			for (size_t i = 0; i < particles.size(); i++){
				for (size_t j = i + 1; j < particles.size(); j++){
					float dist_x = particles[j].x - particles[i].x;
					float dist_y = particles[j].y - particles[i].y;

					float dist_sqr = (dist_x * dist_x) + (dist_y * dist_y);
					if (dist_sqr > 0.0001f){ // avoid division by 0
						float dist = sqrt(dist_sqr);

						float force;
						if (dist_sqr < DIST_LIMIT){
							force = REP_STRENGTH / dist_sqr;
						} else{
							force = ATTR_STRENGTH / dist_sqr;
						}

						float fx = (dist_x / dist) * force;
						float fy = (dist_y / dist) * force;

						particles[i].vx += fx  * dt;
						particles[i].vy += fy  * dt;
						particles[j].vx -= fx  * dt;
						particles[j].vy -= fy  * dt;
					}
				}
			}

			/*
			 * ======================================
			 * 3. UPDATE PARTICLE POSITION
			 * ======================================
			 */

			for (Particle &p : particles){
				p.x += p.vx * dt;
				p.y += p.vy  * dt;

				// 3. Bounce off walls
				if (p.x >= 1.0f && p.vx > 0.0f){
					p.x = 1.0f;
					p.vx = -p.vx;
				}
				if (p.x <= -1.0f && p.vx < 0.0f){
					p.x = -1.0f;
					p.vx = -p.vx;
				}
				if (p.y >= 1.0f && p.vy > 0.0f){
					p.y = 1.0f;
					p.vy = -p.vy;
				}
				if (p.y <= -1.0f && p.vy < 0.0f){
					p.y = -1.0f;
					p.vy = -p.vy;
				}
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

		// set particles start point coordinates
		void set_coordinates(){
			for (Particle &p : particles){
				p.x = dist(gen);
				p.y = dist(gen);

				p.vx = dist(gen);
				p.vy = dist(gen);
			}
		}

		// Particle list settings
		std::vector<Particle> particles;
		std::random_device ran_dev;
		std::mt19937 gen;
		std::uniform_real_distribution<float> dist;

		// Gravity settings
		const float GRAVITY = 0.1f;

		// Wind settings
		const float WIND_X = 0.05f;
		const float WIND_Y = 0.0f;
		std::uniform_real_distribution<float> wind_noise;

		// Attract to center settings
		const float PULL_MULTIPLIER = 0.001f;

		// Attraction & Repulsion settings
		const float ATTR_STRENGTH = 0.0001f;
		const float REP_STRENGTH = -0.001f;
		const float DIST_LIMIT = 0.05f;
};
