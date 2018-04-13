#pragma once
#include "DependenciesMath.h"
namespace fRenderer {
	namespace Rendering {
		namespace Core {
			struct Texture2D {
				unsigned int ID;
				glm::ivec2 resolution;
				~Texture2D();
				Texture2D(unsigned int id = 4294967295, glm::ivec2 res = glm::ivec2(0));
				void Bind(unsigned int target);
			};
			struct Texture3D {
				unsigned int ID;
				glm::ivec3 resolution;
				~Texture3D();
				Texture3D(unsigned int id = 4294967295, glm::ivec3 res = glm::ivec3(0));
				void Bind(unsigned int target);
			};
			Texture2D LoadTexture2D(const char *name, bool linear, bool mipmaps);
			Texture3D EmptyTexture3D(glm::ivec3 resolution, bool linear, bool mipmaps);
			Texture3D LoadTexture3D(const char *name, bool linear, bool mipmaps);
		}
	}
}