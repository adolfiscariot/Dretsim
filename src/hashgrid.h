#pragma once
#include <vector>
#include <algorithm>

class HashGrid{
	public:
		HashGrid(size_t particle_count, float dt); 
		std::pair<int, int> get_particle_cell(float x, float y);
		int calculate_hash(std::pair<int, int> cell);
		std::vector<std::pair<int, int>> get_closest_cells(std::pair<int, int> cell);
		std::vector<int> &get_particles_in_nearby_cell(std::pair<int, int> cell);
		void calculate_interaction(int main_particle, std::vector<int> &closest_particles, std::vector<float> &x, std::vector<float> &y, std::vector<float> &vx, std::vector<float> &vy);
		void build(std::vector<float> &x, std::vector<float> &y);
		void query(std::vector<float> &x, std::vector<float> &y, std::vector<float> &vx, std::vector<float> &vy);

	private:
		size_t _particle_count;
		float _dt;
		static constexpr int PRIME_1 = 73856093;
		static constexpr int PRIME_2 = 19349663;
		std::vector<std::vector<int>> hashtable;
		static constexpr float cell_size = 0.0625f;
		const float ATTR_STRENGTH = 0.0001f;
		const float REP_STRENGTH = -0.001f;
};
