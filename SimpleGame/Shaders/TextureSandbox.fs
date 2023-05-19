#version 330

in vec2 v_TexPos;

out vec4 FragColor;
uniform sampler2D u_TexSampler;

void main()
{
	float tx = fract(v_TexPos.x * 3.0);
	float ty = v_TexPos.y + floor(v_TexPos.x * 3.0) / 3;



	FragColor = texture(u_TexSampler, vec2(tx, ty));
}


//	float x = fract(v_TexPos.x * 3.0);
//	float y =  (floor(v_TexPos.x / 0.3333) + v_TexPos.y) / 3;
