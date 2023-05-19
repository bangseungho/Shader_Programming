#version 330

in vec2 v_Texcoord;

out vec4 FragColor;
uniform sampler2D u_TexSampler;
uniform float u_Time;

uniform float u_SeqNum;

const float c_PI = 3.141592;

void brick()
{
	float tx = fract(v_Texcoord.x * 3.0);
	float ty = v_Texcoord.y + floor(v_Texcoord.x * 3.0) / 3;
	FragColor = texture(u_TexSampler, vec2(tx, ty));
}

void realFlag()
{
	float period = (v_Texcoord.x + 1.0) * 1.0;
	float xValue = v_Texcoord.x*2.0*c_PI*period;
	float yValue = ((1.0-v_Texcoord.y)-0.5)*2.0;
	float sinValue = 0.25*sin(xValue-100.0*u_Time * 0.05);
	if(sinValue*v_Texcoord.x + 0.75 > yValue 
	&& 
	   sinValue*v_Texcoord.x - 0.75 < yValue)
	{
		float vX = v_Texcoord.x;
		float yWidth = 1.5;
		float yDistance = yValue - (sinValue*v_Texcoord.x - 0.75);
		float vY = 1.0-yDistance/yWidth;
		FragColor = texture(u_TexSampler, vec2(vX, vY));
		//FragColor = vec4(vX, vY, 0, 1);
	}
	else
	{
		FragColor = vec4(0);
	}
}

void SATexture()
{
	float seqNum = u_SeqNum;
	float nX = int(seqNum) % 8;
	float nY = floor(seqNum / 8.0);
	float stepX = nX / 8.0f;
	float stepY = nY / 6.0f;
	float x = stepX + v_Texcoord.x / 8.0;
	float y = stepY + v_Texcoord.y / 6.0;
	vec2 newTexPos = vec2(x, y);
	FragColor = texture(u_TexSampler, newTexPos);
}

void main()
{
	//brick();
	realFlag();
	//SATexture();
}


//	float x = fract(v_TexPos.x * 3.0);
//	float y =  (floor(v_TexPos.x / 0.3333) + v_TexPos.y) / 3;
