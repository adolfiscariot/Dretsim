#pragma once
#include <vector>
#include <algorithm>
#include <span>

class HashGrid{
	public:
		HashGrid(size_t particle_count, float dt); 

		std::pair<int, int> get_particle_cell(float x, float y);
		int calculate_hash(std::pair<int, int> cell);

		// Returns a lightweight view into particle_ids
		std::span<const int> get_particles_in_nearby_cell(std::pair<int, int> cell);

		// Takes a span of neighbouring particles's IDs
		void calculate_interaction(
				int main_particle, 
				std::span<const int> closest_particles, 
				std::vector<float> &x, 
				std::vector<float> &y, 
				std::vector<float> &vx, 
				std::vector<float> &vy
		);

		void build(std::vector<float> &x, std::vector<float> &y);
		void query(
				std::vector<float> &x, 
				std::vector<float> &y, 
				std::vector<float> &vx, 
				std::vector<float> &vy
		);

	private:
		size_t _particle_count;
		float _dt;

		// For hash calculation
		static constexpr int PRIME_1 = 73856093;
		static constexpr int PRIME_2 = 19349663;

		static constexpr float cell_size = 0.0625f;
		const float ATTR_STRENGTH = 0.0001f;
		const float REP_STRENGTH = -0.001f;

		// Flat Grid Data Structures
		std::vector<int> cell_offsets;    // Stores starting index of each cell's 1st particle
		std::vector<int> particle_ids;    // Stores each particle's ID
						  
		// Stores the # of particles in each cell. Needed to calculate cell offsets
		std::vector<int> cell_counts;     

		// Stores the cell hash of each particle. Needed to avoid repeating division
		// in the build function
		std::vector<int> particle_hashes; 

};
