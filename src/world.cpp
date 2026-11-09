struct World{
	// World dimensions
	static constexpr float SIZE =  2.0f;  //x and y axis stretch from -1 to 1
	static constexpr float MIN  = -1.0f; 
	static constexpr float MAX  =  1.0f; 

	// Physics
	static constexpr float REPULSION_RATIO = 0.5f; // at what distance should particles repel

	// Grid
	int   cells_per_row;
	float cell_size;
	float attraction_radius;
	float repulsion_radius;

	World(size_t particle_count) {
		cells_per_row = static_cast<int>(
				std::ceil(std::sqrt(static_cast<float>(particle_count)))
				);

		cell_size = SIZE / cells_per_row;

		attraction_radius = cell_size; // a particle can only interact with those in its own cell and the 8 neightbouring cells 
		repulsion_radius  = REPULSION_RATIO * attraction_radius; 
	}
};
