#include "HybridRasterizing.h"
#include <iostream> 

//simple 

void fRenderer::Rendering::Rasterizer::DefferedPipeLine::DrawDeffered(std::vector<Mesh::Model> & BaseModels, Control::Camera & Camera, Window & Window)
{
	DefferedFrameBuffer.Bind(); 
	Deffered.Bind(); 

	glUniformMatrix4fv(glGetUniformLocation(Deffered.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Camera.View));
	glUniformMatrix4fv(glGetUniformLocation(Deffered.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Camera.Project));

	for (auto Model : BaseModels) {
		Mesh::DrawModel(Model, Deffered, Camera, Window); //draw the model itself 
	}

	Deffered.UnBind(); 

	DefferedFrameBuffer.unBind(Window); 
}

void fRenderer::Rendering::Rasterizer::DefferedPipeLine::Prepare(Window & Screen)
{
	//Multi pass frame buffer object

	DefferedFrameBuffer = Core::MultiPassFrameBufferObject(Screen.GetResolution(), 4, true, true); //simple deffered framebuffer 
	Deffered = Core::Shader("Shaders/Deffered/vert.glsl", "Shaders/Deffered/frag.glsl"); //simple deffered shader 
}

fRenderer::Rendering::Rasterizer::DefferedPipeLine::DefferedPipeLine() : Deffered(), DefferedFrameBuffer()
{

}
