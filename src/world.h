truct World{
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

	World(size_t particle_count); 
};
