#include "stdafx.h"
#include "Timer.h"

Timer::Timer()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&periodFrequency);
	// 1초 동안 CPU의 진동수를 반환(고정값)

	QueryPerformanceCounter((LARGE_INTEGER*)&lastTime);
	// 생성자는 Update함수보다 이전에 실행되기 때문에 lastTime에 저장

	timeScale = 1.0 / static_cast<double>(periodFrequency);
	// 미리 (1 / 초당진동수)의 형태로 만들어 놓기
}

void Timer::Update()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&curTime); 
	// 현재 프레임 진동수 저장

	deltaTime = static_cast<double>(curTime - lastTime) * timeScale;
	// (현재 프레임 진동수 - 이전 프레임 진동수)

	lastTime = curTime;
	// 현재 프레임을 이전 프레임으로 저장, 이후 계속 반복
}
