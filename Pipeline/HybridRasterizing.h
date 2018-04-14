#pragma once
#include <Dependencies\DependenicesRasterizing.h> 

namespace fRenderer {
	namespace Rendering {
		namespace Rasterizer {
			class DefferedPipeLine {
				Core::Shader Deffered; 
			public:
				Core::MultiPassFrameBufferObject DefferedFrameBuffer;
				void DrawDeffered(std::vector<Mesh::Model> & BaseModels, Control::Camera & Camera, Window & Window); 
				void Prepare(Window & Screen); 
				DefferedPipeLine();
			}; 
		}
	}
}