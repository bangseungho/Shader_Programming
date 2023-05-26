#version 330

layout(location = 0) out vec4 FragColor;
uniform sampler2D u_TexSampler;

in vec2 v_TexPos;
in float v_greyScale;

void GridMesh()
{
	vec2 newTexPos = fract(v_TexPos * 5.0);
	FragColor = 0.8 * (v_greyScale + 1.0) * texture(u_TexSampler, newTexPos);
}

void main()
{
	GridMesh();

}
