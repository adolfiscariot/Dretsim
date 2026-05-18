// SPATIAL HASH GRID IMPLEMENTATION
// What we wanna do is take each particle and get its cell. Then create a hashtable
// where all particles in the same cell are placed in the same array index. Then 
// we find the particles in the surrounding 8 cells (2D field) and attract/repel them


#include <iostream>
#include <cstdint>
#include <vector>
#include <cmath>

class HashGrid{
	public:
		HashGrid(size_t particle_count, float dt): 
			_particle_count(particle_count),
			_dt(dt)
		{
			hashtable.resize(_particle_count);
		}

		// Take the position of a particle and find its cell
		std::pair<int, int> get_particle_cell(float x, float y){
			int cx = std::floor(x / cell_size);
			int cy = std::floor(y / cell_size);

			std::pair<int, int> cell = {cx, cy};
			return cell;
		}

		// Calculate hash 
		int calculate_hash(std::pair<int, int> cell){
			return ((cell.first * PRIME_1) ^ (cell.second * PRIME_2)) % _particle_count;
		}


		// For each cell get the cells next to it
		std::vector<std::pair<int, int>> get_closest_cells(std::pair<int, int> cell){
			std::pair<int, int> top_left      = {cell.first - 1, cell.second + 1};
			std::pair<int, int> top_middle    = {cell.first,     cell.second + 1};
			std::pair<int, int> top_right     = {cell.first + 1, cell.second + 1};
			std::pair<int, int> middle_left   = {cell.first - 1, cell.second    };
			std::pair<int, int> middle_right = {cell.first + 1, cell.second    };
			std::pair<int, int> bottom_left   = {cell.first - 1, cell.second - 1};
			std::pair<int, int> bottom_middle = {cell.first,     cell.second - 1};
			std::pair<int, int> bottom_right  = {cell.first + 1, cell.second - 1};

			std::vector<std::pair<int, int>> cells = {top_left, top_middle, top_right, middle_left,  middle_right, bottom_left, bottom_middle, bottom_right};
			return cells;
		}	

		// Get particles in those nearby cells. Return vector of particle ids. 
		std::vector<int> &get_particles_in_nearby_cell(std::pair<int, int> cell){
			int hash = calculate_hash(cell);	
			return hashtable[hash];
		}

		// Calculate interaction. Find distance between cells and interact.
		void calculate_interaction(int main_particle, std::vector<int> closest_particles, std::vector<float> x, std::vector<float> y, std::vector<float> vx, std::vector<float> vy){
			for(int close_particle : closest_particles){
				if(main_particle == close_particle) continue;
				float dist_x = x[close_particle] - x[main_particle];
				float dist_y = y[close_particle] - y[main_particle];
				float dist_sqr = std::max((dist_x * dist_x) + (dist_y * dist_y), 0.0001f);
				float force = (dist_sqr < 0.05f)? (-0.001f / dist_sqr) : (0.001f * dist_sqr);
				float dist = sqrt(dist_sqr);
				float fx = (dist_x / dist) * force;
				float fy = (dist_y / dist) * force;

				vx[main_particle] += fx * _dt;
				vy[main_particle] += fy * _dt;

			}	
		}
		
		// Insert in hashtable
		void build(std::vector<float> x, std::vector<float> y){
			for (auto &bucket: hashtable) bucket.clear();
			for (int i = 0; i < _particle_count; i++){
				std::pair<int, int> cell = get_particle_cell(x[i], y[i]);
				int hash = calculate_hash(cell);
				hashtable[hash].push_back(i); // put particle in hashtable

			}
		}

		// Query
		void query(std::vector<float> x, std::vector<float> y, std::vector<float> vx, std::vector<float> vy){
			for (int i = 0; i < _particle_count; i++){
				//Particle
				std::pair<float, float> particle = {x[i], y[i]};
				// Get cell
				std::pair<int, int> cell = get_particle_cell(x[i], y[i]);
				// Get neighouring cells
				std::vector<std::pair<int, int>> closest_cells = get_closest_cells(cell);
				// Get particles in neighbouring cells
				for (auto &close_cell : closest_cells){
					std::vector<int> close_particles = get_particles_in_nearby_cell(close_cell);
					// Calculate forces
					calculate_interaction(i, close_particles, x, y, vx, vy);	
				}

			}
		}

	private:
		// Number of particles and delta time per frame
		size_t _particle_count;
		float _dt;
		
		// primes for hash function
		static constexpr int PRIME_1 = 73856093;
		static constexpr int PRIME_2 = 19349663;

		// flat array to put particles in same cell
		// hashtable[0] = {1,2,3}, hashtable[1] = {}, hashtable[2] = {4,5,6,7}, etc.
		std::vector<std::vector<int>> hashtable;

		// 10k particles @ 10 per cell = 1000 cells which means 31x31 cells i.e.
		// 16 in each direction.
		// if our grid ranges from -1 ro 1 then thats a cell size of 0.0625x0.0625
		static constexpr float cell_size = 0.0625f;
		static constexpr size_t interaction_radius = cell_size;
};	


