#include "stdafx.h"
#include "Renderer.h"
#include "LoadPng.h"

using namespace std;

default_random_engine dre;
uniform_real_distribution urdColorRGB{ 0.f, 5.0f };
uniform_real_distribution urdColorA{ 0.f, 1.0f };
uniform_real_distribution urdPos{ -1.f, 1.f };
uniform_real_distribution urdVelX{ -1.f, 1.f };
uniform_real_distribution urdVelY{ 0.f, 2.f };
uniform_real_distribution urdEmitTime{ 0.f, 10.f };
uniform_real_distribution urdLifeTime{ 0.f, 3.f };
uniform_real_distribution urdPeriod{ 0.f, 1.f };
uniform_real_distribution urdAmp{ -1.f, 1.f };
uniform_real_distribution urdValue{ 0.f, 1.f };

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}

Renderer::~Renderer()
{
	delete m_Timer;
}

// Init
void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	//m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_ParticleShader = CompileShaders("./Shaders/Particle.vert", "./Shaders/Particle.frag");
	m_FragmentSandboxShader = CompileShaders("./Shaders/FragmentSandbox.vert", "./Shaders/FragmentSandbox.frag");
	m_AlphaClearShader = CompileShaders("./Shaders/AlphaClear.vert", "./Shaders/AlphaClear.frag");
	m_VertexSandboxShader = CompileShaders("./Shaders/VertexSandbox.vert", "./Shaders/VertexSandbox.frag");
	m_TextureSandboxShader = CompileShaders("./Shaders/TextureSandbox.vert", "./Shaders/TextureSandbox.frag");
	m_GridMeshShader = CompileShaders("./Shaders/GridMesh.vert", "./Shaders/GridMesh.frag");
	m_DrawTextureShader = CompileShaders("./Shaders/DrawTexture.vert", "./Shaders/DrawTexture.frag");

	PrepareBloom();

	// VBOs
	//CreateVertexBufferObjects();
	//CreateCheckerboard();
	//CreateGridMesh();
	CreateParticle(5000);

	// Textures
	//m_RGBTexture = CreatePngTexture("./newj.png", GL_NEAREST);
	//m_RGBTexture = CreatePngTexture("./rgb.png", GL_NEAREST);
	m_HaerinTexture = CreatePngTexture("./haerin.png", GL_NEAREST);
	m_RightSourceTexture = CreatePngTexture("./RightSource.png", GL_NEAREST);

	// FBOs
	CreateFBOs();

	//if (m_SolidRectShader > 0 && m_VBORect > 0)
	//{
	//	m_Initialized = true;
	//}
	m_Timer = new Timer();
}

void Renderer::Update()
{
	m_Timer->Update();
	g_Time += m_Timer->GetDeltaTime();
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

bool Renderer::ReadFile(char* filename, std::string* target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

void Renderer::GetGLPosition(float x, float y, float* newX, float* newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.";

	return ShaderProgram;
}

int Renderer::SetAttribVBO(int shaderProgram, const GLchar* attribName, GLuint VBO, GLint size)
{
	int attribLoc = -1;
	attribLoc = glGetAttribLocation(shaderProgram, attribName);
	glEnableVertexAttribArray(attribLoc);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(attribLoc, size, GL_FLOAT, GL_FALSE, 0, 0);
	return attribLoc;
}

void Renderer::SetAttribute(int attribLoc, int size, int stride, int offset)
{
	glEnableVertexAttribArray(attribLoc);
	glVertexAttribPointer(attribLoc, size, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (GLvoid*)(sizeof(float) * offset));
}

// Create
void Renderer::CreateParticle(int numParticle)
{
	m_ParticleSize = 0.02f;
	float centerX = 0.f;
	float centerY = 0.f;
	int particleCount = numParticle;
	m_ParticleVerticesCount = particleCount * 6;
	int floatCount = particleCount * 6 * 3;
	int floatCountPosColorVelUV = particleCount * 6 * (3 + 4 + 3 + 2);

#pragma region Time
	// EmitTime
	std::vector<float> verticesEmitTime;
	int floatCountSingle = particleCount * 6;

	for (int i = 0; i < particleCount; ++i) {
		float emitTime = urdEmitTime(dre);

		verticesEmitTime.push_back(emitTime);
		verticesEmitTime.push_back(emitTime);
		verticesEmitTime.push_back(emitTime);

		verticesEmitTime.push_back(emitTime);
		verticesEmitTime.push_back(emitTime);
		verticesEmitTime.push_back(emitTime);
	}

	glGenBuffers(1, &m_ParticleEmitTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCountSingle, &verticesEmitTime[0], GL_STATIC_DRAW);

	// LifeTime
	std::vector<float> verticesLifeTime;

	for (int i = 0; i < particleCount; ++i) {
		float lifeTime = urdLifeTime(dre);

		verticesLifeTime.push_back(lifeTime);
		verticesLifeTime.push_back(lifeTime);
		verticesLifeTime.push_back(lifeTime);

		verticesLifeTime.push_back(lifeTime);
		verticesLifeTime.push_back(lifeTime);
		verticesLifeTime.push_back(lifeTime);
	}

	glGenBuffers(1, &m_ParticleLifeTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCountSingle, &verticesLifeTime[0], GL_STATIC_DRAW);

	// Period
	std::vector<float> verticesPeriod;

	for (int i = 0; i < particleCount; ++i) {
		float randPeriod = urdPeriod(dre);

		verticesPeriod.push_back(randPeriod);
		verticesPeriod.push_back(randPeriod);
		verticesPeriod.push_back(randPeriod);

		verticesPeriod.push_back(randPeriod);
		verticesPeriod.push_back(randPeriod);
		verticesPeriod.push_back(randPeriod);
	}

	glGenBuffers(1, &m_ParticlePeriodVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePeriodVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCountSingle, &verticesPeriod[0], GL_STATIC_DRAW);

	// Amp
	std::vector<float> verticesAmp;

	for (int i = 0; i < particleCount; ++i) {
		float randAmp = urdAmp(dre);

		verticesLifeTime.push_back(randAmp);
		verticesLifeTime.push_back(randAmp);
		verticesLifeTime.push_back(randAmp);

		verticesLifeTime.push_back(randAmp);
		verticesLifeTime.push_back(randAmp);
		verticesLifeTime.push_back(randAmp);
	}

	glGenBuffers(1, &m_ParticleAmpVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleAmpVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCountSingle, &verticesLifeTime[0], GL_STATIC_DRAW);

	// Value
	std::vector<float> verticesValue;

	for (int i = 0; i < particleCount; ++i) {
		float randValue = urdAmp(dre);

		verticesValue.push_back(randValue);
		verticesValue.push_back(randValue);
		verticesValue.push_back(randValue);

		verticesValue.push_back(randValue);
		verticesValue.push_back(randValue);
		verticesValue.push_back(randValue);
	}

	glGenBuffers(1, &m_ParticleValueVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleValueVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCountSingle, &verticesValue[0], GL_STATIC_DRAW);
#pragma endregion

	// PosColor
	std::vector<float> verticesPosColorVelUV;

	for (int i = 0; i < particleCount; ++i) {
		centerX = 0.f;
		centerY = 0.f;
		float colorR = urdColorRGB(dre);
		float colorG = urdColorRGB(dre);
		float colorB = urdColorRGB(dre);
		float colorA = urdColorA(dre);
		float velocityX = urdVelX(dre);
		float velocityY = urdVelY(dre);

		// 1
		// position
		verticesPosColorVelUV.push_back(centerX - m_ParticleSize);
		verticesPosColorVelUV.push_back(centerY + m_ParticleSize);
		verticesPosColorVelUV.push_back(0.f);
		// color
		verticesPosColorVelUV.push_back(colorR);
		verticesPosColorVelUV.push_back(colorG);
		verticesPosColorVelUV.push_back(colorB);
		verticesPosColorVelUV.push_back(colorA);
		// velocity
		verticesPosColorVelUV.push_back(velocityX);
		verticesPosColorVelUV.push_back(velocityY);
		verticesPosColorVelUV.push_back(0.f);
		// uv
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(0.f);

		// 2
		verticesPosColorVelUV.push_back(centerX - m_ParticleSize);
		verticesPosColorVelUV.push_back(centerY - m_ParticleSize);
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(colorR);
		verticesPosColorVelUV.push_back(colorG);
		verticesPosColorVelUV.push_back(colorB);
		verticesPosColorVelUV.push_back(colorA);
		verticesPosColorVelUV.push_back(velocityX);
		verticesPosColorVelUV.push_back(velocityY);
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(1.f);

		// 3
		verticesPosColorVelUV.push_back(centerX + m_ParticleSize);
		verticesPosColorVelUV.push_back(centerY + m_ParticleSize);
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(colorR);
		verticesPosColorVelUV.push_back(colorG);
		verticesPosColorVelUV.push_back(colorB);
		verticesPosColorVelUV.push_back(colorA);
		verticesPosColorVelUV.push_back(velocityX);
		verticesPosColorVelUV.push_back(velocityY);
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(1.f);
		verticesPosColorVelUV.push_back(0.f);
		// 4
		verticesPosColorVelUV.push_back(centerX + m_ParticleSize);
		verticesPosColorVelUV.push_back(centerY + m_ParticleSize);
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(colorR);
		verticesPosColorVelUV.push_back(colorG);
		verticesPosColorVelUV.push_back(colorB);
		verticesPosColorVelUV.push_back(colorA);
		verticesPosColorVelUV.push_back(velocityX);
		verticesPosColorVelUV.push_back(velocityY);
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(1.f);
		verticesPosColorVelUV.push_back(0.f);
		// 5
		verticesPosColorVelUV.push_back(centerX - m_ParticleSize);
		verticesPosColorVelUV.push_back(centerY - m_ParticleSize);
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(colorR);
		verticesPosColorVelUV.push_back(colorG);
		verticesPosColorVelUV.push_back(colorB);
		verticesPosColorVelUV.push_back(colorA);
		verticesPosColorVelUV.push_back(velocityX);
		verticesPosColorVelUV.push_back(velocityY);
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(1.f);
		// 6
		verticesPosColorVelUV.push_back(centerX + m_ParticleSize);
		verticesPosColorVelUV.push_back(centerY - m_ParticleSize);
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(colorR);
		verticesPosColorVelUV.push_back(colorG);
		verticesPosColorVelUV.push_back(colorB);
		verticesPosColorVelUV.push_back(colorA);
		verticesPosColorVelUV.push_back(velocityX);
		verticesPosColorVelUV.push_back(velocityY);
		verticesPosColorVelUV.push_back(0.f);
		verticesPosColorVelUV.push_back(1.f);
		verticesPosColorVelUV.push_back(1.f);
	}

	glGenBuffers(1, &m_ParticlePosColorVelUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColorVelUVVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCountPosColorVelUV, &verticesPosColorVelUV[0], GL_STATIC_DRAW);

#pragma region Sandbox
	float size = 1.f;
	float rect1[] =
	{
		-size, -size, 0.f,	0.f, 1.f,
		-size, size, 0.f,	0.f, 0.f,
		size, size, 0.f,	1.f, 0.f,
		-size, -size, 0.f,	0.f, 1.f,
		size, size, 0.f,	1.f, 0.f,
		size, -size, 0.f,	1.f, 1.f
	};

	glGenBuffers(1, &m_FragmentSandboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_FragmentSandboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect1), rect1, GL_STATIC_DRAW);
#pragma endregion

	float sizeX = 1.f / m_WindowSizeX;
	float sizeY = 1.f / m_WindowSizeY;

	float DrawTextureRect[] =
	{
		-sizeX, -sizeY, 0.f,	0.f, 1.f,
		-sizeX, sizeY, 0.f,		0.f, 0.f,
		sizeX, sizeY, 0.f,		1.f, 0.f,
		-sizeX, -sizeY, 0.f,	0.f, 1.f,
		sizeX, sizeY, 0.f,		1.f, 0.f,
		sizeX, -sizeY, 0.f,		1.f, 1.f
	};

	glGenBuffers(1, &m_DrawTextureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_DrawTextureVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DrawTextureRect), DrawTextureRect, GL_STATIC_DRAW);


	float rect2[] =
	{
		-1.f, -1.f, 0.f,
		-1.f, 1.f, 0.f,
		1.f, 1.f, 0.f,
		-1.f, -1.f, 0.f,
		1.f, 1.f, 0.f,
		1.f, -1.f, 0.f,
	};

	glGenBuffers(1, &m_AlphaClearVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_AlphaClearVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect2), rect2, GL_STATIC_DRAW);

	m_HoriLineVertexCount = 100;
	int ffloatCount = m_HoriLineVertexCount * 3;
	int index = 0;
	float gap = 2.f / ((float)m_HoriLineVertexCount - 1.f);
	vector<float> lineVertice;
	for (int i = 0; i < m_HoriLineVertexCount; ++i) {
		lineVertice.push_back(-1.f + (float)i * gap);
		lineVertice.push_back(0.f);
		lineVertice.push_back(0.f);
	}
	glGenBuffers(1, &m_HoriLineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_HoriLineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ffloatCount, &lineVertice[0], GL_STATIC_DRAW);


}

void Renderer::CreateVertexBufferObjects()
{
	float textureRect[] = {
		-0.5f, 0.5f, 0.f,	0.f, 0.f,
		-0.5f, -0.5f, 0.f,	0.f, 1.f,
		0.5f, 0.5f, 0.f,	1.f, 0.f,
		0.5f, 0.5f, 0.f,	1.f, 0.f,
		-0.5f, -0.5f, 0.f,	0.f, 1.f,
		0.5f, -0.5f, 0.f,	1.f, 1.f
	};
	glGenBuffers(1, &m_TextureSandboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TextureSandboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureRect), textureRect, GL_STATIC_DRAW);

	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
}

void Renderer::CreateGridMesh()
{
	float basePosX = -0.5f;
	float basePosY = -0.5f;
	float targetPosX = 0.5f;
	float targetPosY = 0.5f;

	int pointCountX = 64;
	int pointCountY = 64;

	float width = targetPosX - basePosX;
	float height = targetPosY - basePosY;

	float* point = new float[pointCountX * pointCountY * 2];
	float* vertices = new float[(pointCountX - 1) * (pointCountY - 1) * 2 * 3 * 3];
	m_GridMeshVertexCount = (pointCountX - 1) * (pointCountY - 1) * 2 * 3;

	//Prepare points
	for (int x = 0; x < pointCountX; x++)
	{
		for (int y = 0; y < pointCountY; y++)
		{
			point[(y * pointCountX + x) * 2 + 0] = basePosX + width * (x / (float)(pointCountX - 1));
			point[(y * pointCountX + x) * 2 + 1] = basePosY + height * (y / (float)(pointCountY - 1));
		}
	}

	//Make triangles
	int vertIndex = 0;
	for (int x = 0; x < pointCountX - 1; x++)
	{
		for (int y = 0; y < pointCountY - 1; y++)
		{
			//Triangle part 1
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;

			//Triangle part 2
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
		}
	}

	glGenBuffers(1, &m_GridMeshVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_GridMeshVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (pointCountX - 1) * (pointCountY - 1) * 2 * 3 * 3, vertices, GL_STATIC_DRAW);

	delete[] vertices;
	delete[] point;
}

GLuint Renderer::CreatePngTexture(char* filePath, GLuint samplingMethod)
{
	vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filePath);

	if (error != 0)
	{
		cout << "PNG image loading failed: " << filePath << endl;
		assert(0);
	}

	GLuint temp;
	glGenTextures(1, &temp);
	glBindTexture(GL_TEXTURE_2D, temp);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplingMethod);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplingMethod);

	return temp;
}

void Renderer::CreateOffscreenTexture(GLuint& texID, GLsizei width, GLsizei height)
{
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

void Renderer::CreateDepthRenderBuffer(GLuint& depthRenderBuffer, GLsizei width, GLsizei height)
{
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderer::CreateFBO(GLuint& FBO, GLuint& depthRenderBuffer, GLuint& texID, GLenum attach)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attach, GL_TEXTURE_2D, texID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		cout << "fbo creation failed" << endl;
	}
}

void Renderer::CreateFBOs()
{
	CreateOffscreenTexture(m_AFBOTexture, 512, 512);
	CreateOffscreenTexture(m_BFBOTexture, 512, 512);
	CreateOffscreenTexture(m_B_1_FBOTexture, 512, 512);
	CreateOffscreenTexture(m_B_2_FBOTexture, 512, 512);
	CreateOffscreenTexture(m_B_3_FBOTexture, 512, 512);
	CreateOffscreenTexture(m_CFBOTexture, 512, 512);

	CreateDepthRenderBuffer(m_DepthRenderBuffer, 512, 512);

	glGenFramebuffers(1, &m_A_FBO);
	CreateFBO(m_A_FBO, m_DepthRenderBuffer, m_AFBOTexture, GL_COLOR_ATTACHMENT0);
	
	glGenFramebuffers(1, &m_B_FBO);
	CreateFBO(m_B_FBO, m_DepthRenderBuffer, m_BFBOTexture, GL_COLOR_ATTACHMENT0);
	CreateFBO(m_B_FBO, m_DepthRenderBuffer, m_B_1_FBOTexture, GL_COLOR_ATTACHMENT1);
	CreateFBO(m_B_FBO, m_DepthRenderBuffer, m_B_2_FBOTexture, GL_COLOR_ATTACHMENT2);
	CreateFBO(m_B_FBO, m_DepthRenderBuffer, m_B_3_FBOTexture, GL_COLOR_ATTACHMENT3);
	
	glGenFramebuffers(1, &m_C_FBO);
	CreateFBO(m_C_FBO, m_DepthRenderBuffer, m_CFBOTexture, GL_COLOR_ATTACHMENT0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CreateCheckerboard()
{
	static const GLulong checkerboard[] = {
		0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
		0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
		0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
		0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
		0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
		0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
		0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
		0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
	};

	glGenTextures(1, &m_CheckerBoardTexture);
	glBindTexture(GL_TEXTURE_2D, m_CheckerBoardTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerboard);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::PrepareBloom()
{
	m_GaussianBlurHShader = CompileShaders("./Shaders/GaussianBlur.vert", "./Shaders/GaussianBlurH.frag");
	m_GaussianBlurVShader = CompileShaders("./Shaders/GaussianBlur.vert", "./Shaders/GaussianBlurV.frag");
	m_DrawMergeTextureShader = CompileShaders("./Shaders/MergeTexture.vert", "./Shaders/MergeTexture.frag");

	float sizeX = 1.f;
	float sizeY = 1.f;

	float fullTextureRect[] =
	{
		-sizeX,	+sizeY, 0.f,	0.f, 0.f,
		-sizeX,	-sizeY, 0.f,	0.f, 1.f,
		+sizeX,	+sizeY, 0.f,	1.f, 0.f,
		+sizeX,	+sizeY, 0.f,	1.f, 0.f,
		-sizeX,	-sizeY, 0.f,	0.f, 1.f,
		+sizeX,	-sizeY, 0.f,	1.f, 1.f
	};

	glGenBuffers(1, &m_FullRectVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_FullRectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fullTextureRect), fullTextureRect, GL_STATIC_DRAW);

	// HDR 관련 FBO
	glGenFramebuffers(1, &m_HDRFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_HDRFBO);

	glGenTextures(1, &m_HDRHeighTexture);
	glBindTexture(GL_TEXTURE_2D, m_HDRHeighTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &m_HDRLowTexture);
	glBindTexture(GL_TEXTURE_2D, m_HDRLowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_HDRLowTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_HDRHeighTexture, 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		cout << "fbo creation failed" << endl;
	}

	// Pingpong 관련 FBO
	glGenFramebuffers(2, m_PingpongFBO);
	glGenTextures(2, m_PingpongTexture);
	for (int i = 0; i < 2; ++i) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_PingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, m_PingpongTexture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PingpongTexture[i], 0);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			cout << "fbo creation failed" << endl;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Draw
void Renderer::DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a)
{
	float newX, newY;

	GetGLPosition(x, y, &newX, &newY);

	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), newX, newY, 0, size);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawParticleEffect()
{
	int shaderProgram = m_ParticleShader;
	glUseProgram(shaderProgram);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posAttribLoc = glGetAttribLocation(shaderProgram, "a_Position");
	int colorAttribLoc = glGetAttribLocation(shaderProgram, "a_Color");
	int velocityAttribLoc = glGetAttribLocation(shaderProgram, "a_Vel");
	int uvAttribLoc = glGetAttribLocation(shaderProgram, "a_TexPos");

	int emitAttribLoc = SetAttribVBO(shaderProgram, "a_EmitTime", m_ParticleEmitTimeVBO, 1);
	int lifeAttribLoc = SetAttribVBO(shaderProgram, "a_LifeTime", m_ParticleLifeTimeVBO, 1);
	int	periodAttribLoc = SetAttribVBO(shaderProgram, "a_Period", m_ParticlePeriodVBO, 1);
	int ampAttribLoc = SetAttribVBO(shaderProgram, "a_Amp", m_ParticleAmpVBO, 1);
	int valueAttribLoc = SetAttribVBO(shaderProgram, "a_Value", m_ParticleValueVBO, 1);

	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColorVelUVVBO);
	SetAttribute(posAttribLoc, 3, 12, 0);
	SetAttribute(colorAttribLoc, 4, 12, 3);
	SetAttribute(velocityAttribLoc, 3, 12, 7);
	SetAttribute(uvAttribLoc, 2, 12, 10);

	GLuint SamplerULoc = glGetUniformLocation(shaderProgram, "u_TexSampler");
	glUniform1i(SamplerULoc, 0); // 0번째 슬롯에 넘기겠다는 의미
	glActiveTexture(GL_TEXTURE0); // 0번째 슬롯에 넘기며 바인드
	glBindTexture(GL_TEXTURE_2D, m_RightSourceTexture); // 0번째 슬롯에 바인드가 됨

	glUniform1f(glGetUniformLocation(m_ParticleShader, "u_Time"), g_Time);

	static float period = 1;

	glUniform1f(glGetUniformLocation(m_ParticleShader, "u_Period"), period);

	glDrawArrays(GL_TRIANGLES, 0, m_ParticleVerticesCount);
	glDisable(GL_BLEND); // 다른 함수에서도 사용하기 때문에 꺼야함 나중에

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawFragmentSandbox()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_B_FBO);
	glViewport(0, 0, 512, 512);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
	glDrawBuffers(5, drawBuffers);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint shader = m_FragmentSandboxShader;
	glUseProgram(shader);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLoc = glGetAttribLocation(shader, "a_Position");
	int texLoc = glGetAttribLocation(shader, "a_Texcoord");
	glEnableVertexAttribArray(posLoc);
	glEnableVertexAttribArray(texLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_FragmentSandboxVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	GLuint SamplerULoc = glGetUniformLocation(shader, "u_TexSampler");
	glUniform1i(SamplerULoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_HaerinTexture);

#pragma region Rader
	float x = 0.5f;
	float y = 0.5f;
	glUniform2f(glGetUniformLocation(shader, "u_Point"), x, y);

	float points[] = { 0.5f, 0.5f,
					   0.3f, 0.3f,
					   0.8f, 0.8f };

	glUniform2fv(glGetUniformLocation(shader, "u_Points"), 3, points);  
#pragma endregion 
	 
	glUniform1f(glGetUniformLocation(shader, "u_Time"), g_Time);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawAlphaClear()
{
	GLuint shader = m_AlphaClearShader;
	glUseProgram(shader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint posLoc = glGetAttribLocation(shader, "a_Position");
	glBindBuffer(GL_ARRAY_BUFFER, m_AlphaClearVBO);
	SetAttribute(posLoc, 3, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_BLEND);
}

void Renderer::DrawVertexSandbox()
{
	GLuint shader = m_VertexSandboxShader;
	glUseProgram(shader);

	GLuint posAttribLoc = glGetAttribLocation(shader, "a_Position");
	glBindBuffer(GL_ARRAY_BUFFER, m_FragmentSandboxVBO);
	SetAttribute(posAttribLoc, 3, 0, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUniform1f(glGetUniformLocation(m_ParticleShader, "u_Time"), g_Time);
	glDrawArrays(GL_TRIANGLES, 0, m_HoriLineVertexCount);

	//glUniform1f(glGetUniformLocation(m_ParticleShader, "u_Time"), g_Time + 0.5f);
	//glDrawArrays(GL_LINE_STRIP, 0, m_HoriLineVertexCount);

	//glUniform1f(glGetUniformLocation(m_ParticleShader, "u_Time"), g_Time + 1.f);
	//glDrawArrays(GL_LINE_STRIP, 0, m_HoriLineVertexCount);

	glDisable(GL_BLEND);
}

void Renderer::DrawTextureSandbox()
{
	GLuint shader = m_TextureSandboxShader;
	glUseProgram(shader);

	GLuint posLoc = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(posLoc);
	GLuint textureLoc = glGetAttribLocation(shader, "a_TexPos");
	glEnableVertexAttribArray(textureLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_TextureSandboxVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 0));
	glVertexAttribPointer(textureLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	GLuint SamplerULoc = glGetUniformLocation(shader, "u_TexSampler");
	glUniform1i(SamplerULoc, 0); // 0번째 슬롯에 넘기겠다는 의미
	glActiveTexture(GL_TEXTURE0); // 0번째 슬롯에 넘기며 바인드
	glBindTexture(GL_TEXTURE_2D, m_HaerinTexture); // 0번째 슬롯에 바인드가 됨

	glUniform1f(glGetUniformLocation(shader, "u_Time"), g_Time);

	glUniform1f(glGetUniformLocation(shader, "u_SeqNum"), g_Time * 30.f);

	glUniform2f(glGetUniformLocation(shader, "xy_repeat"), floor(g_Time), floor(g_Time));

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawGridMesh()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_C_FBO);
	glViewport(0, 0, 512, 512);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLuint shader = m_GridMeshShader;

	glUseProgram(shader);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_GridMeshVBO);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	GLuint textureLoc = glGetAttribLocation(shader, "a_TexPos");
	glEnableVertexAttribArray(textureLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_GridMeshVBO);
	glVertexAttribPointer(textureLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	GLuint SamplerULoc = glGetUniformLocation(shader, "u_TexSampler");
	glUniform1i(SamplerULoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_RGBTexture);

	glDrawArrays(GL_LINE_STRIP, 0, m_GridMeshVertexCount);

	glDisableVertexAttribArray(attribPosition);

	glUniform1f(glGetUniformLocation(shader, "u_Time"), g_Time);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawTexture(float x, float y, float scaleX, float scaleY, GLuint texID)
{
	GLuint shader = m_DrawTextureShader;
	glUseProgram(shader);

	int posLoc = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(posLoc);
	int texLoc = glGetAttribLocation(shader, "a_TexPos");
	glEnableVertexAttribArray(texLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_DrawTextureVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	GLuint samplerULoc = glGetUniformLocation(shader, "u_TexSampler");
	glUniform1i(samplerULoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);

	glUniform4f(glGetUniformLocation(shader, "u_PosScale"), x, y, scaleX, scaleY);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawFBOTexture()
{
	DrawTexture(-0.75, 0.75, 128, 128, m_AFBOTexture);
	DrawTexture(-0.25, 0.75, 128, 128, m_BFBOTexture);
	DrawTexture( 0.25, 0.75, 128, 128, m_B_1_FBOTexture);
	DrawTexture(-0.75, 0.25, 128, 128, m_B_2_FBOTexture);
	DrawTexture(-0.25, 0.25, 128, 128, m_B_3_FBOTexture);
	DrawTexture( 0.75, 0.75, 128, 128, m_CFBOTexture);
}

void Renderer::DrawParticleWidthBloom()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_HDRFBO);
	GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawBuffers);
	glClear(GL_COLOR_BUFFER_BIT);
	DrawParticleEffect();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawGaussianBlur(m_HDRLowTexture, m_PingpongFBO[0], m_GaussianBlurHShader);

	for (int i = 0; i < 20; ++i) {
		DrawGaussianBlur(m_PingpongTexture[0], m_PingpongFBO[1], m_GaussianBlurVShader);
		DrawGaussianBlur(m_PingpongTexture[1], m_PingpongFBO[0], m_GaussianBlurHShader);
	}
	DrawGaussianBlur(m_PingpongTexture[0], m_PingpongFBO[1], m_GaussianBlurVShader);

	DrawTexture(-0.5, -0.5, 512, 512, m_PingpongTexture[1]);
	DrawTexture(0.5, -0.5, 512, 512, m_HDRLowTexture);

	DrawMergeBloomTexture(m_HDRLowTexture, m_PingpongTexture[1], 1.f);
}

void Renderer::DrawGaussianBlur(GLuint texID, GLuint targetFBOID, GLuint shader)
{
	glBindFramebuffer(GL_FRAMEBUFFER, targetFBOID);
	glUseProgram(shader);

	int posAttribLoc = glGetAttribLocation(shader, "a_Position");

	int uvAttribLoc = glGetAttribLocation(shader, "a_TexPos");

	GLuint SamplerULoc = glGetUniformLocation(shader, "u_TexSampler");
	glUniform1i(SamplerULoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);

	glBindBuffer(GL_ARRAY_BUFFER, m_FullRectVBO);

	SetAttribute(posAttribLoc, 3, 5, 0);
	SetAttribute(uvAttribLoc, 2, 5, 3);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawMergeBloomTexture(GLuint sceneTexID, GLuint bloomTexID, float exposure)
{
	GLuint shader = m_DrawMergeTextureShader;
	glUseProgram(shader);

	int posAttribLoc = glGetAttribLocation(shader, "a_Position");
	int uvAttribLoc = glGetAttribLocation(shader, "a_TexPos");

	glUniform1i(glGetUniformLocation(shader, "u_TexSamplerScene"), 0);
	glUniform1i(glGetUniformLocation(shader, "u_TexSamplerbloom"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTexID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloomTexID);

	glUniform1f(glGetUniformLocation(shader, "u_Exposure"), exposure);

	glBindBuffer(GL_ARRAY_BUFFER, m_FullRectVBO);
	SetAttribute(posAttribLoc, 3, 5, 0);
	SetAttribute(uvAttribLoc, 2, 5, 3);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
