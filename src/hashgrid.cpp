// SPATIAL HASH GRID IMPLEMENTATION
// What we wanna do is take each particle and get its cell. Then calculate the hash
// of each  cell and put use it as an index in the particle_hashes vector. We then
// ensure all particles live within a particle_ids vector. Then we find the particles
// in the surrounding 8 cells (2D field) and attract/repel them

#include <algorithm>
#include <iostream>
#include <cstdint>
#include <vector>
#include <cmath>
#include <span>
#include "hashgrid.h"

HashGrid::HashGrid(size_t particle_count, float dt): 
			_particle_count(particle_count),
			_dt(dt)
		{
			particle_hashes.resize(_particle_count);
			particle_ids.resize(_particle_count);
			cell_counts.resize(_particle_count,0);
			cell_offsets.resize(_particle_count + 1, 0);
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


// Get particles in those nearby cells using flat meory span
std::span<const int> HashGrid::get_particles_in_nearby_cell(std::pair<int, int> cell){
	int hash = calculate_hash(cell);	
	int start_idx = cell_offsets[hash];
	int end_idx = cell_offsets[hash + 1];

	if (start_idx >= end_idx) return {}; // cell is empty
	return std::span<const int>(&particle_ids[start_idx], end_idx - start_idx);
}

// Calculate interaction. Find distance between cells and interact.
void HashGrid::calculate_interaction(int main_particle, std::span<const int> closest_particles, std::vector<float> &x, std::vector<float> &y, std::vector<float> &vx, std::vector<float> &vy){
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

	for(int close_particle : closest_particles){
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
	// Set cell_counts to 0 every frame so that a cell doesn't 
	// report having more particles than it actually has
	std::fill(cell_counts.begin(), cell_counts.end(), 0);

	// Iterate through all particles, find their cell, cache the cell's hash
	// and increase that hash in cell_counts by 1 because now another particle
	// is living in that cell
	for (int i = 0; i < _particle_count; i++){
		int hash = calculate_hash(get_particle_cell(x[i],y[i]));
		particle_hashes[i] = hash;
		cell_counts[hash]++;
	}

	// Build cell_offsets from cell_counts. First offset is always 0
	int current_offset = 0;
	for (size_t c = 0; c < cell_counts.size(); c++){
		cell_offsets[c] = current_offset;
		current_offset += cell_counts[c];
	}
	cell_offsets[cell_counts.size()] = current_offset;

	// Fill particle_ids using hashed particle_hashes
	std::vector<int> temp_offsets = cell_offsets;
	for (int i = 0; i < _particle_count; i++){
		int hash = particle_hashes[i];
		int dest_index = temp_offsets[hash]++;
		particle_ids[dest_index] = i;
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

				// Fetch zero-allocation span slice directly
				std::span< const int> close_particles = get_particles_in_nearby_cell(close_cell);
				calculate_interaction(i, close_particles, x, y, vx, vy);

			}
		}
	}
}



