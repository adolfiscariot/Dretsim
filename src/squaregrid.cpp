/*
 * This file handles the following for particles:
 * 1. mapping them into cells 
 * 2. finding neighbouring particles 
 */


#include "world.cpp"
#include <iostream>


class SquareGrid{
	public:
		SquareGrid(size_t particle_count): 
			_particle_count(particle_count)
		{
			std::cout << "Its SquareGrid Bitch!\n"			
		}

		/* Map a particle to a cell. Our world begins at -1, -1 not at 0,0
		 * which is why we substitute x & y by World::MIN. Otherwise
		 * a particle with an x co-ordinate of -0.75 when divided directly
		 * by cell_size e.g. 0.5, would produce -1.5 whose floor would be -2
		 * and that doesn't exist in our world, however -0.75 - (-1.0) / 0.5
		 * equals 0.5 whose floor is 0 which is in our world.
		 */
		std::pair<int, int> get_particle_cell(float x, float y){
			return {

				static_cast<int>((std::floor(x - World::MIN) / cell_size)),
				static_cast<int>((std::floor(y - World::MIN) / cell_size))
			};

		}


	private:
		size_t _particle_count;
};
