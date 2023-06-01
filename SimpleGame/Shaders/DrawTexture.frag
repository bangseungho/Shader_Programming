#version 330

layout(location = 0) out vec4 FragColor;

in vec2 v_TexPos;

uniform sampler2D u_TexSampler;

void main()
{
	FragColor = texture(u_TexSampler, v_TexPos);
}
