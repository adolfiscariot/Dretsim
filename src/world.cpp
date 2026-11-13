#include <cmath>
#include "world.h" 

World::World(size_t particle_count) {
	cells_per_row = static_cast<int>(
			std::ceil(std::sqrt(static_cast<double>(particle_count)))
			);

	cell_size = SIZE / cells_per_row;

	// Maximum distance at which particles can interact.
	attraction_radius = cell_size;

	// Repulsion occurs within a smaller radius.
	repulsion_radius = REPULSION_RATIO * attraction_radius;
}
