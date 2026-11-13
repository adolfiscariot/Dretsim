/*
 * This file handles the following for particles:
 * 1. mapping them into cells 
 * 2. finding neighbouring particles 
 */


#include "world.h"
#include <iostream>


class SquareGrid{
	public:
		SquareGrid(World &world, size_t particle_count): 
			_world(world),
			_particle_count(particle_count)
		{
			_particle_ids.resize(particle_count);

			// put each particle in it's own _particle_id 
			for (int i = 0; i < _particle_count; i++)
				_particle_ids[i] = i;
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
					std::floor((x - _world::MIN) / _world::cell_size)
					),
				static_cast<int>(
					std::floor((y - _world::MIN) / _world::cell_size)
					)
			};
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
		size_t _particle_count;
		std::vector<int> _particle_ids;
		std::vector<int> _cell_offsets;
};
