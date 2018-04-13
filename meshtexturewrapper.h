#pragma once
#include "Mesh.h"
#define MATERIAL_TEXTURE_QUALITY 256 //the texture maps' resolution (they all have to be the same) 
#define MATERIAL_TEXTURE_QUALITY_SQUARED MATERIAL_TEXTURE_QUALITY * MATERIAL_TEXTURE_QUALITY
//^ could probably be constants 
namespace fRenderer { 
	namespace Rendering {
		namespace Mesh {
			struct WrappedModel {
				unsigned int MeshData, MaterialData; 
				Vector2i MeshDataResolution; 
				WrappedModel();
				bool Wrap(Model WrapModel); 
			}; 
		}
	}
}