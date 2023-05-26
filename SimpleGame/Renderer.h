#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include "Timer.h"
#include "Dependencies\glew.h"

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

public:
	void Initialize(int windowSizeX, int windowSizeY);
	bool IsInitialized();
	bool ReadFile(char* filename, std::string* target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	void GetGLPosition(float x, float y, float* newX, float* newY);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	int SetAttribVBO(int shaderProgram, const GLchar* attribName, GLuint VBO, GLint size);
	void Renderer::SetAttribute(int attribLoc, int size, int stride, int offset);

private:
	void CreateParticle(int numParticle);
	void CreateVertexBufferObjects();
	GLuint CreatePngTexture(char* filePath, GLuint saplingMethod);
	void CreateGridMesh();
	void CreateFBOs();

public:
	void DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a);
	void DrawParticleEffect();
	void DrawFragmentSandbox();
	void DrawAlphaClear();
	void DrawVertexSandbox();
	void DrawTextureSandbox();
	void DrawGridMesh();


private:
	Timer* m_Timer;

	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	bool m_Initialized = false;

	float m_VerticesCount = 0;
	float m_ParticleSize = 0;
	int m_ParticleVerticesCount = 0;

private:
	// particle
	GLuint m_ParticleShader = -1;
	GLuint m_ParticlePositionVBO = -1;
	GLuint m_ParticleColorVBO = -1;
	GLuint m_ParticleVelocityVBO = -1;
	GLuint m_ParticleEmitTimeVBO = -1;
	GLuint m_ParticleLifeTimeVBO = -1;	
	GLuint m_ParticlePeriodVBO = -1;
	GLuint m_ParticleAmpVBO = -1;
	GLuint m_ParticleValueVBO = -1;
	GLuint m_ParticlePosColorVelUVVBO = -1;

	// alpha
	GLuint m_AlphaClearShader = -1;
	GLuint m_AlphaClearVBO = -1;

	// VertexSandBox
	GLuint m_VertexSandboxShader = -1;
	GLuint m_HoriLineVBO = -1;
	GLuint m_HoriLineVertexCount = 0;

	// FragmentSandBox
	GLuint m_FragmentSandboxShader = -1;
	GLuint m_FragmentSandboxVBO = -1;

	// TextureSandBox
	GLuint m_TextureSandboxShader = -1;
	GLuint m_TextureSandboxVBO = -1;
	GLuint m_RGBTexture = -1;
	
	// Rect
	GLuint m_VBORect = 0;
	GLuint m_SolidRectShader = 0;

	// GridMesh
	GLuint m_GridMeshShader = 0;
	GLuint m_GridMeshVertexCount = 0;
	GLuint m_GridMeshVBO = 0;
	GLuint m_GridEmitTime = 0;

	// FrameBuffer Object
	GLuint m_AFBOTexture = 0;
	GLuint m_BFBOTexture = 0;
	GLuint m_CFBOTexture = 0;
	GLuint m_DepthRenderBuffer = 0;
	GLuint m_A_FBO = 0;
};

