#pragma once 
#include "hybrid_rasterizing.h" 
#include "hybrid_pathtracing.h" 
namespace fRenderer {
	namespace Rendering {
		namespace Combined {
			class Pipeline {
				Rasterizer::DefferedPipeLine Deffered;
				PathTracing::PathTracingPipeLine PathTracing;
				std::vector<Mesh::Model> Models;  
				Core::Shader CompositingShader; 
			public: 
				Pipeline(); 
				void Prepare(std::vector<Mesh::Model> Models, Window & Window, Control::Camera & Camera);
				void Run(Window & Window, Control::Camera & Camera); 
				void Compositing(); 
			}; 
		}
	}
}