#include <iostream>
#include <vector>
#include <random>

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
			// 1. Apply forces
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
				 * Pull multiplier ensures the wind gets stronger as distances 
				 * are smaller
				 */

				p.vx += dx * PULL_MULTIPLIER * dt;
				p.vy += dy * PULL_MULTIPLIER * dt;
			}

			// 2. Attraction and replusion

			/*
			 * O(n^2) loop as each particle measures it's distance from all other
			 * particles. The square of this distance is used to find the force
			 * to be applied (attraction/repulsion) via the inverse-square law.
			 * This force is applied to both parties as per Newton's 3rd law: each
			 * force begets an equal and opposite force
			 */

			for (int i = 0; i < particles.size(); i++){
				for (int j = i + 1; j < particles.size(); j++){
					float dist_x = particles[j].x - particles[i].x;
					float dist_y = particles[j].y - particles[i].y;

					float dist_sqr = (dist_x * dist_x) + (dist_y * dist_y);
					if (dist_sqr > 0.0001f){ // avoid division by 0
						float force = STRENGTH / dist_sqr;

						particles[i].vx += dist_x * force * dt;
						particles[i].vy += dist_y * force * dt;
						particles[j].vx -= dist_x * force * dt;
						particles[j].vy -= dist_y * force * dt;
					}
				}
			}


			// 3. Update position
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
		const float GRAVITY = 0.1f;

		// set particles start point coordinates
		void set_coordinates(){
			for (Particle &p : particles){
				p.x = dist(gen);
				p.y = dist(gen);

				p.vx = 0.5f;
				p.vy = 0.5f;
			}
		}

		// Particle list settings
		std::vector<Particle> particles;
		std::random_device ran_dev;
		std::mt19937 gen;
		std::uniform_real_distribution<float> dist;

		// Wind settings
		const float WIND_X = 0.05f;
		const float WIND_Y = 0.0f;
		std::uniform_real_distribution<float> wind_noise;

		// Attract to center settings
		const float PULL_MULTIPLIER = 0.001f;

		// Attraction & Repulsion settings
		const float STRENGTH = 0.0001f;
};
