#pragma once

class Timer
{
public:
	Timer();

public:
	void Update();
	double GetDeltaTime() { return deltaTime; }

private:
	__int64 periodFrequency = 0;
	__int64 curTime = 0;
	__int64 lastTime = 0;
	double timeScale = 0;
	double deltaTime = 0;
};

