#version 330

in vec2 v_TexPos;

out vec4 FragColor;
uniform sampler2D u_TexSampler;
uniform float u_Time;
uniform float u_SeqNum;
uniform vec2 xy_repeat;

const float c_PI = 3.141592;

void brick()
{
	float tx = fract(v_TexPos.x * 3.0);
	float ty = v_TexPos.y + floor(v_TexPos.x * 3.0) / 3;
	FragColor = texture(u_TexSampler, vec2(tx, ty));
}

void realFlag()
{
	float period = (v_TexPos.x + 1.0) * 1.0;
	float xValue = v_TexPos.x*2.0*c_PI*period;
	float yValue = ((1.0-v_TexPos.y)-0.5)*2.0;
	float sinValue = 0.25*sin(xValue-100.0*u_Time * 0.05);
	if(sinValue*v_TexPos.x + 0.75 > yValue 
	&& 
	   sinValue*v_TexPos.x - 0.75 < yValue)
	{
		float vX = v_TexPos.x;
		float yWidth = 1.5;
		float yDistance = yValue - (sinValue*v_TexPos.x - 0.75);
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
	float x = stepX + v_TexPos.x / 8.0;
	float y = stepY + v_TexPos.y / 6.0;
	vec2 newTexPos = vec2(x, y);
	FragColor = texture(u_TexSampler, newTexPos);
}

void RGBBGR()
{
	float x = v_TexPos.x;
	float y = 1 - abs(v_TexPos.y * 2.f - 1.f);

	vec2 newTexPos = vec2(x, y);
	FragColor = texture(u_TexSampler, newTexPos);
}

void URGB()
{
	float x = fract(v_TexPos.x * 3.f);
	float y = (floor(v_TexPos.x / 0.3333f) + v_TexPos.y) / 3;

	vec2 newTexPos = vec2(x, y);
	FragColor = texture(u_TexSampler, newTexPos);
}

void UBGR()
{
	float x = fract(v_TexPos.x * 3.f);
	float y = (floor(3 - v_TexPos.x / 0.3333f) + v_TexPos.y) / 3;

	vec2 newTexPos = vec2(x, y);
	FragColor = texture(u_TexSampler, newTexPos);
}

void VBGR()
{
	float x = v_TexPos.x;
	float y = ((2 - floor(v_TexPos.y / 0.3333)) + fract(v_TexPos.y * 3)) / 3;

	vec2 newTexPos = vec2(x, y);
	FragColor = texture(u_TexSampler, newTexPos);
}

void RGBGBRBRG()
{
	float x = fract(v_TexPos.x * 3);
	float y = v_TexPos.y + floor(v_TexPos.x / 0.3333) / 3 ;

	vec2 newTexPos = vec2(x, y);
	FragColor = texture(u_TexSampler, newTexPos);
}

void RepeatX()
{
	float x_repeat = xy_repeat.x;
	float y_repeat = xy_repeat.y;

	float x = v_TexPos.x * x_repeat + floor((1 - v_TexPos.y) * y_repeat) * 0.5;
	float y = fract(v_TexPos.y * y_repeat);

	vec2 newTexPos = vec2(x, y);
	FragColor = texture(u_TexSampler, newTexPos);
}

void RepeatY()
{
	float x_repeat = xy_repeat.x;
	float y_repeat = xy_repeat.y;

	float x = v_TexPos.x * x_repeat;
	float y = v_TexPos.y * y_repeat + floor(v_TexPos.x * x_repeat) * 0.5;

	vec2 newTexPos = vec2(x, y);
	FragColor = texture(u_TexSampler, newTexPos);
}

void Rotate()
{
	//float x_repeat = xy_repeat.x;
	//float y_repeat = xy_repeat.y;

	float dx = floor((v_TexPos.y + v_TexPos.x)) * 0.5;
	float x = (v_TexPos.y + v_TexPos.x) * 3;
	float y = v_TexPos.y * 3 + dx;

	vec2 newTexPos = vec2(x, y);
	FragColor = texture(u_TexSampler, newTexPos);
}

void main()
{
	//brick();
	//realFlag();
	//SATexture();
	Rotate();
}


//	float x = fract(v_TexPos.x * 3.0);
//	float y =  (floor(v_TexPos.x / 0.3333) + v_TexPos.y) / 3;
