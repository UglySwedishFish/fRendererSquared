#pragma once
#include "dependencies_rendering.h" 
#include "dependencies_math.h" 


class Window {
	Vector2i Resolution; 
	bool Fullscreen; 
	sf::Window * RawWindow; 
	const char * Title; 
	float FrameTime; 
	float TimeOpened; 
	int FrameCount; 
	public: 
	void SetResolution(Vector2i Resolution); 
	Vector2i GetResolution(); 

	void SetFullscreen(bool Fullscreen); 
	bool GetFullscreen(); 

	void SetTitle(const char * Title); 
	const char * GetTitle(); 

	void SetFrameTime(float FrameTime);
	float GetFrameTime(); 

	void SetTimeOpened(float TimeOpened); 
	float GetTimeOpened();

	void SetFrameCount(int FrameCount); 
	int GetFrameCount(); 

	void CursorVisible(bool Visible); 

	sf::Window * GetRawWindow(); 

	inline Window() : Resolution(0), Fullscreen(false), RawWindow(nullptr), Title("fRenderer²") {}
	Window(Vector2i Resolution, bool Fullscreen); 
	~Window(); 
}; 