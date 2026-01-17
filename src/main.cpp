#include <iostream>
#include <GLFW/glfw3.h>

int main() {
	std::cout << "Engine starting ...\n";

	for (int i = 0; i < 5; i++){
		std::cout << "Frame " << i << "\n";
	}

	std::cout << "Engine stopped\n";
	return 0;
}
