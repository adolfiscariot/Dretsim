#include <iostream>
#include <vector>
#include <random>



struct Particle{
	float x, y;
	float vx, vy;
};

class Simulation{

	public:
		Simulation(int count): particles(count), gen(ran_dev()), dist(-1.0f, 1.0f){
			set_coordinates();
		}

		// update particles position
		void update_particles(float dt){
			for (Particle &p : particles){
				// 1. Apply gravity
				p.vy += -GRAVITY *dt;

				// 2. Update position
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

				p.vx = dist(gen);
				p.vy = dist(gen);
			}
		}

		std::vector<Particle> particles;
		std::random_device ran_dev;
		std::mt19937 gen;
		std::uniform_real_distribution<float> dist;
};
