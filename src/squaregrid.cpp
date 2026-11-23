/*
 * This file handles the following for particles:
 * 1. mapping them into cells 
 * 2. finding neighbouring particles 
 */


#include "world.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <utility>
#include <cstddef>


class SquareGrid{
	public:
		SquareGrid(World &world, std::size_t particle_count): 
			_world(world),
			_particle_count(particle_count)
		{
			std::size_t total_cells = _world.cells_per_row * _world.cells_per_row;

			_particle_ids.   resize(particle_count);
			_cell_counts.    resize(total_cells);
			_cell_offsets.   resize(total_cells + 1);
			_cell_indices.   resize(particle_count);
		}


		/* 
		 * Map a particle to a cell. Our world begins at -1, -1 not at 0,0
		 * which is why we substitute x & y by World::MIN. Otherwise
		 * a particle with an x co-ordinate of -0.75 when divided directly
		 * by cell_size e.g. 0.5, would produce -1.5 whose floor would be -2
		 * and that doesn't exist in our world, however -0.75 - (-1.0) / 0.5
		 * equals 0.5 whose floor is 0 which is in our world.
		 */
		inline std::pair<int, int> get_particle_cell(float x, float y){
			return {

				static_cast<int>(
					std::floor((x - World::MIN) / _world.cell_size)
					),
				static_cast<int>(
					std::floor((y - World::MIN) / _world.cell_size)
					)
			};
		}


		inline int get_cell_index(std::pair<int, int> particle_cell){
			int x = std::clamp(particle_cell.first,  0, _world.cells_per_row - 1);
			int y = std::clamp(particle_cell.second, 0, _world.cells_per_row - 1);

			return y * _world.cells_per_row + x;
		}


		// Fill up the necessary vectors with correct data
		void build(std::vector<float> &x, std::vector<float> &y){
			std::fill(_cell_counts.begin(), _cell_counts.end(), 0);
			
			for (std::size_t i = 0; i < x.size(); i++){
				auto particle_cell = get_particle_cell(x[i], y[i]);
				auto cell_index = get_cell_index(particle_cell);
				_cell_indices[i] = cell_index;
				/* 
				 * add 1 to _cell_counts[_cell_index] to show that index 
				 * cell_index has received one more particle. 
				 */
				_cell_counts[cell_index]++; 
			}

			/*
			 *  We've filled up cell_counts. Now it's time to use that data
			 *  to fill up _cell_offsets which tells us where exactly in
			 *  _particle_ids each cell's particles start and end i.e.
			 *  _cell_offsets[i+1] - _cell_offsets[i] = _cell_counts[i].
			 *  Assume: p0->c1, p1->c3, p2->c0, p3->c0, p4->c2
			 *  _particle_ids = [2, 3, 0, 4, 1]
			 *  _cell_counts  = [2, 1, 1, 1, 0]
			 *  _cell_offsets = [0, 2, 3, 4, 5, 5]
			 */
			
			std::size_t current_offset = 0;
			for (std::size_t j = 0; j < _cell_counts.size(); j++){
				_cell_offsets[j] = current_offset;
				current_offset += _cell_counts[j];
			}
			_cell_offsets[_cell_counts.size()] = current_offset;

			
			/*
			 * _particle_cursor keeps track of the next available position in
			 * _particle_ids for each cell. For every particle k, we use
			 * _cell_indices[k] to determine which cell it belongs to then we
			 * look up that cell's cursor to know where the particle ID should be
			 * inserted in _particle_ids.
			 *
			 * After inserting the particle ID, we increment that cell's cursor
			 * so that if another particle belongs to the same cell, it gets
			 * inserted into the next available position rather than overwriting
			 * the previous particle.
			 */
			_particle_cursor.assign(
				_cell_offsets.begin(),
				_cell_offsets.end() - 1
			);
			for (std::size_t k = 0; k < x.size(); k++){
				std::size_t pos = _particle_cursor[_cell_indices[k]];
				_particle_ids[pos] = k;
				_particle_cursor[_cell_indices[k]]++;
			}
		}


		/*
		 * Once we get a particle's cell we check surrounding cells for all
		 * particles in there
		 */
		std::vector<int> get_neighbouring_particles(float x, float y){
			std::pair<int, int> particle_cell = get_particle_cell(x, y);
			/*
			 * Assuming we have 10,000 particles and therefore 100x100 cells, 
			 * each cell has width 2 (-1, 1) / 100 which equals 0.02. So if
			 * a cell's physical position is (0.8, 0.8), it's grid position 
			 * will be (0.8 - (-1) / 0.02) which equals 90. Since we're checking
			 * the 8 surrounding cells, we'll check (91, 90), (89, 90), 
			 * (90, 91), (90, 89), (91, 91), (91, 89), (89, 91) 
			 * and (0.7, 0.7). 
			 */ 
		}
		
	private:
		World &_world;
		std::size_t _particle_count;
		std::vector<std::size_t> _particle_ids;
		std::vector<std::size_t> _cell_offsets;
		std::vector<std::size_t> _cell_counts;
		std::vector<std::size_t> _cell_indices;
		std::vector<std::size_t> _particle_cursor;
};
