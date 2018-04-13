#include "hybrid_pathtracing.h"
#include <iostream> 

fRenderer::Rendering::PathTracing::PathTracingPipeLine::PathTracingPipeLine() 
	: Trace(), TraceShader(), DenoisedTrace(), DenoisedTraceShader(), TracePrevious(), TracePreviousShader(), WrappedModels{}
{

}

void fRenderer::Rendering::PathTracing::PathTracingPipeLine::Prepare(std::vector<Mesh::Model> &Models, Control::Camera & Camera,Window & Window)
{
	for (auto Model : Models) {
		WrappedModels.push_back(Mesh::WrappedModel()); 
		WrappedModels[WrappedModels.size() - 1].Wrap(Model); 
	}

	//^ and at that moment the models are now locked into place, they can be moved with model matrices but nothing else	

	TraceShader = Core::Shader("Shaders/PathTracer/vert.glsl", "Shaders/PathTracer/frag.glsl");

	Trace = Core::FrameBufferObject(Window.GetResolution(), false); 

	//do static uniforms in a static way

	TraceShader.Bind(); 

	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "TotalModels"), static_cast<int>(Models.size())); 

	for (int Model = 0; Model < Models.size(); Model++) {
		
		std::string _Models = "Models[" + std::to_string(Model) + "]."; 

		

		std::string ModelData = _Models + "ModelData";
		std::string MaterialData = _Models + "MaterialData";
		std::string VerticeCount = _Models + "VerticeCount";
		std::string Resolution = _Models + "Resolution";


		glUniform1i(glGetUniformLocation(TraceShader.ShaderID, ModelData.c_str()), Model*2+3);
		glUniform1i(glGetUniformLocation(TraceShader.ShaderID, MaterialData.c_str()), Model*2+4);
		glUniform1i(glGetUniformLocation(TraceShader.ShaderID, VerticeCount.c_str()), Models[Model].ModelData.Vertices.size());
		glUniform2i(glGetUniformLocation(TraceShader.ShaderID, Resolution.c_str()), WrappedModels[Model].MeshDataResolution.x, WrappedModels[Model].MeshDataResolution.y);

	}

	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "PositionSpecular"), 0);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "NormalRefractive"), 1);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "IOREmmisisionRoughness"), 2);


	TraceShader.UnBind(); 
}

void fRenderer::Rendering::PathTracing::PathTracingPipeLine::ComputeTrace(Window & Window, Control::Camera & Camera, Rasterizer::DefferedPipeLine & Pipeline)
{
	Trace.Bind(); 

	TraceShader.Bind(); 

	for (int Model = 0; Model < WrappedModels.size(); Model++) {

		glActiveTexture(GL_TEXTURE3 + Model * 2);
		glBindTexture(GL_TEXTURE_2D_ARRAY, WrappedModels[Model].MeshData);
		glActiveTexture(GL_TEXTURE4 + Model * 2);
		glBindTexture(GL_TEXTURE_2D_ARRAY, WrappedModels[Model].MaterialData);

	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[2]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[3]);

	glUniformMatrix4fv(glGetUniformLocation(TraceShader.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Camera.View));
	glUniformMatrix4fv(glGetUniformLocation(TraceShader.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Camera.Project));
	glUniform3f(glGetUniformLocation(TraceShader.ShaderID, "CameraPosition"), Camera.Position.x, Camera.Position.y, Camera.Position.z);

	//^ those are dynamic uniforms 

	Core::DrawPostProcessQuad(); //draw the "post processing" quad (i;e the quad that fills the entire display) 

	TraceShader.UnBind(); 

	Trace.UnBind(Window);
}
