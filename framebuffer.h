#pragma once
#include "dependencies_rendering.h"
#include "dependencies_math.h" 
#include "window.h" 
#include "Mesh.h" 
namespace fRenderer {
	namespace Rendering {
		namespace Core {
			struct FrameBufferObject {
				GLuint FrameBuffer, ColorBuffer, DepthBuffer;
				glm::ivec2 Resolution;
				bool GenerateMip; 
				FrameBufferObject(glm::ivec2 Resolution, bool SamplerShadow, bool alpha = false, bool generatemip = false);
				FrameBufferObject(); 
				void Bind();
				void UnBind(Window Window);

			};
			struct MultiPassFrameBufferObject {
				GLuint FrameBuffer, DepthBuffer;
				std::vector<GLuint> ColorBuffers;
				glm::ivec2 Resolution;
				bool GenerateMip; 
				MultiPassFrameBufferObject(glm::ivec2 Resolution, int stages, bool alpha = false, bool generatemip = false);
				MultiPassFrameBufferObject(); 
				void Bind();
				void unBind(Window Window);
			};
			void PreparePostProcess();
			void DrawPostProcessQuad();
		}
	}
}