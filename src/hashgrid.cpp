// SPATIAL HASH GRID IMPLEMENTATION
// What we wanna do is take each particle and get its cell. Then create a hashtable
// where all particles in the same cell are placed in the same array index. Then 
// we find the particles in the surrounding 8 cells (2D field) and attract/repel them


#include <iostream>
#include <cstdint>
#include <vector>
#include <cmath>
#include "hashgrid.h"

HashGrid::HashGrid(size_t particle_count, float dt): 
			_particle_count(particle_count),
			_dt(dt)
		{
			hashtable.resize(_particle_count);
		}

// Take the position of a particle and find its cell
std::pair<int, int> HashGrid::get_particle_cell(float x, float y){
	return {static_cast<int>(std::floor(x/cell_size)), static_cast<int>(std::floor(y/cell_size))};
}


// Calculate hash 
int HashGrid::calculate_hash(std::pair<int, int> cell){
	uint32_t hash = ((cell.first * PRIME_1) ^ (cell.second * PRIME_2)) ;
	return static_cast<int>(hash % _particle_count);
}


// Get particles in those nearby cells. Return vector of particle ids. 
std::vector<int> &HashGrid::get_particles_in_nearby_cell(std::pair<int, int> cell){
	int hash = calculate_hash(cell);	
	return hashtable[hash];
}

// Calculate interaction. Find distance between cells and interact.
void HashGrid::calculate_interaction(int main_particle, std::vector<int> &closest_particles, std::vector<float> &x, std::vector<float> &y, std::vector<float> &vx, std::vector<float> &vy){
	// Pointers to the starting address of these vectors to
	// avoid double jumps to the vector manager object then
	// the starting address
	const float* const p_x = x.data();
	const float* const p_y = y.data();
	float* const p_vx = vx.data();
	float* const p_vy = vy.data();

	float main_x = p_x[main_particle];
	float main_y = p_y[main_particle];

	// Store accumulations in registers then at the end store in memory 
	float main_vx_accum = 0.0f;
	float main_vy_accum = 0.0f;

	for(int &close_particle : closest_particles){
		// Calculate the force once i.e. calculate it
		// when main_particle is index 2 and close_particle
		// is 7 but when main_partiicle becomes 7
		// and close_particle becomes 2 don't calculate
		// it again.
		if(main_particle >= close_particle) continue;

		float dist_x = p_x[close_particle] - main_x;
		float dist_y = p_y[close_particle] - main_y;

		float dist_sqr = (dist_x * dist_x) + (dist_y * dist_y);
		if (dist_sqr < 0.0001f) dist_sqr = 0.0001f;

		float force = (dist_sqr < 0.05f) ? REP_STRENGTH / dist_sqr : ATTR_STRENGTH / dist_sqr;

		// Use the inverse to avoid expensive division arithmetic in inv_dist_by_force 
		// since (dist_x/ sqrtf(dist_sqr)) * force == 1/sqrtf(dist_sqr) * force 
		float inv_dist = 1.0f / sqrtf(dist_sqr);
		float inv_dist_by_force = inv_dist * force * _dt;

		float fx = dist_x * inv_dist_by_force;
		float fy = dist_y * inv_dist_by_force;

		main_vx_accum += fx;
		main_vy_accum += fy;

		// Add to main  particle and subtract from close particle. Newton's 3rd law: for
		// every action there's an equal and opposite reaction
		p_vx[close_particle] -= fx;
		p_vy[close_particle] -= fy;
	}	
	p_vx[main_particle] += main_vx_accum;
	p_vy[main_particle] += main_vy_accum;
}

// Insert in hashtable
void HashGrid::build(std::vector<float> &x, std::vector<float> &y){
	for (auto &bucket: hashtable) bucket.clear();
	for (int i = 0; i < _particle_count; i++){
		std::pair<int, int> cell = get_particle_cell(x[i], y[i]);
		int hash = calculate_hash(cell);
		hashtable[hash].push_back(i); 

	}
}

// For each particle, find its cell, find that cell's closest cells, get the particles in those
// cells and calculate the interaction
void HashGrid::query(std::vector<float> &x, std::vector<float> &y, std::vector<float> &vx, std::vector<float> &vy){
	for (int i = 0; i < _particle_count; i++){
		std::pair<int, int> cell = get_particle_cell(x[i], y[i]);
		for (int dx = -1; dx <= 1; dx++){
			for (int dy = -1; dy <= 1; dy++){
				std::pair<int, int> close_cell = {cell.first + dx, cell.second + dy};
				int hash = calculate_hash(close_cell);
				std::vector<int> &close_particles = hashtable[hash];
				calculate_interaction(i, close_particles, x, y, vx, vy);

			}
		}
	}
}



