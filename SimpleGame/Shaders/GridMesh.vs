#version 330

in vec3 a_Position;

const float PI = 3.141592;

void Sine()
{
	float x = a_Position.x;
	float value = (a_Position.x + 0.5) * 2 * PI;
	float y = a_Position.y + sin(value) * 0.5;
	vec4 newPosition = vec4(x, y, 0.0, 1.0);
	gl_Position = newPosition;
}

void main()
{
	Sine();
}
