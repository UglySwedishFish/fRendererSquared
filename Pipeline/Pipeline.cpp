#include "Pipeline.h"
#include <iostream> 
fRenderer::Rendering::Combined::Pipeline::Pipeline()
{
}

void fRenderer::Rendering::Combined::Pipeline::Prepare(std::vector<Mesh::Model> _Models, Window & Window, Control::Camera & Camera)
{
	Deffered.Prepare(Window);
	PathTracing.Prepare(_Models, Camera, Window); 
	Core::PreparePostProcess(); 
	
	Models = _Models; 
	glEnable(GL_DEPTH_TEST);

	CompositingShader = Core::Shader("Shaders/Compositing/vert.glsl", "Shaders/Compositing/frag.glsl"); 
}

void fRenderer::Rendering::Combined::Pipeline::Run(Window & Window, Control::Camera & Camera)
{
	bool Running = true;
	//Window.CursorVisible(false); 
	sf::Clock GameClock; 
	sf::Event Event; 
	float T = 0.; 
	while (Running) {
		while (Window.GetRawWindow()->pollEvent(Event)) {
			switch (Event.type)
			{
				case sf::Event::Closed:
					delete &Window;
					return;		
				default:
					break;
			}		
		}
		Window.SetFrameTime(GameClock.getElapsedTime().asSeconds()); 
		GameClock.restart(); 
		T += Window.GetFrameTime(); 
		Window.SetFrameCount(Window.GetFrameCount()+1);
		Window.SetTimeOpened(Window.GetTimeOpened() + Window.GetFrameTime()); 
		fRenderer::Core::HandleInput(Camera, 2.0, 0.15,  Window, true, true); 
		Camera.View = fRenderer::Core::ViewMatrix(Camera.Position, Camera.Rotation);
		
	

		Deffered.DrawDeffered(Models, Camera, Window); 
		PathTracing.ComputeTrace(Window, Camera, Deffered); 
		Compositing(); 

		Window.GetRawWindow()->display(); 
	}
}

void fRenderer::Rendering::Combined::Pipeline::Compositing()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	CompositingShader.Bind(); 

	glUniform1i(glGetUniformLocation(CompositingShader.ShaderID, "Albedo"), 0);
	glUniform1i(glGetUniformLocation(CompositingShader.ShaderID, "Lighting"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Deffered.DefferedFrameBuffer.ColorBuffers[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, PathTracing.Trace.ColorBuffer);

	Core::DrawPostProcessQuad(); 

	CompositingShader.UnBind(); 
}
