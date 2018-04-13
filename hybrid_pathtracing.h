#pragma once
#include "dependencies_pathtracing.h" 
#include "hybrid_rasterizing.h" 

namespace fRenderer {
	namespace Rendering {
		namespace PathTracing {
			class PathTracingPipeLine {
				Core::Shader TraceShader, DenoisedTraceShader, TracePreviousShader; 
				std::vector<Mesh::WrappedModel> WrappedModels; 
			public: 
				Core::FrameBufferObject Trace, DenoisedTrace, TracePrevious;
				PathTracingPipeLine(); 
				void Prepare(std::vector<Mesh::Model> &Models, Control::Camera & Camera, Window & Window);
				void ComputeTrace(Window & Window, Control::Camera & Camera, Rasterizer::DefferedPipeLine & Pipeline); 
			}; 
		}
	}
}