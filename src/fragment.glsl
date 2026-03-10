#version 330 core
in vec3 fragColour;
out vec4 FragColour;

void main()
{
	FragColour = vec4(fragColour, 1.0); 
}

