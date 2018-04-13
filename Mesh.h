#pragma once

#include <vector> 
#include <unordered_map> 
#include "DependenciesMesh.h"
#include "DependenciesMath.h"
#include "texture.h" 
#include "Shader.h" 
#include "camera.h" 

namespace fRenderer {
	namespace Rendering {
		namespace Mesh {
			enum Material_Texture_Types { TEXTURE_ALBEDOMAP, TEXTURE_NORMALMAP, TEXTURE_SPECULARMAP, TEXTURE_ROUGHNESSMAP, TEXTURE_PARALLAXMAP, TEXTURE_AOMAP };

			struct Mesh { //note; this is NOT a model. It is only a collection of the data required to construct a model 

				struct MeshEntry {
					unsigned int NumIndices;
					unsigned int BaseVertex;
					unsigned int BaseIndex;
					unsigned int MaterialIndex;
					inline MeshEntry() :
						NumIndices(0),
						BaseVertex(0),
						BaseIndex(0),
						MaterialIndex(0)
					{}
				};


				std::vector<glm::vec3> Vertices;
				std::vector<glm::vec2> TextureCoordinates;
				std::vector<glm::vec3> Normals;
				std::vector<glm::vec3> Tangents;
				std::vector<unsigned int> Indices;
				std::vector<MeshEntry> MeshEntries;
				std::vector<unsigned int> Materials;
				void ClearVectors();
				~Mesh();

			};

			struct Material {
				Core::Texture2D AlbedoMap, NormalMap, SpecularMap, RoughnessMap;
				bool HasAlbedoMap, HasNormalMap, HasSpecularMap, HasRoughnessMap, isShadowCatcher;
				glm::vec3 DiffuseColor, SpecularColor;
				float Emmisive, Specular, Roughness, Refractivity, RefractiveIndex;

				inline Material() :
					AlbedoMap(Core::Texture2D()),
					NormalMap(Core::Texture2D()),
					SpecularMap(Core::Texture2D()),
					RoughnessMap(Core::Texture2D()),
					HasAlbedoMap(false),
					HasNormalMap(false),
					HasSpecularMap(false),
					HasRoughnessMap(false),
					isShadowCatcher(false),
					DiffuseColor(glm::vec3(0.)),
					SpecularColor(glm::vec3(0.)),
					Emmisive(0.),
					Specular(0.),
					Roughness(0.),
					Refractivity(0.),
					RefractiveIndex(1.33)
				{}

				inline void AllocateTexture(Material_Texture_Types Type, Core::Texture2D Tex, bool OverWrite = false) {
					switch (Type) {
					case TEXTURE_ALBEDOMAP:
						if (!HasAlbedoMap || OverWrite)
							AlbedoMap = Tex;
						HasAlbedoMap = true;
						break;
					case TEXTURE_NORMALMAP:
						if (!HasNormalMap || OverWrite)
							NormalMap = Tex;
						HasNormalMap = true;
						break;
					case TEXTURE_SPECULARMAP:
						if (!HasSpecularMap || OverWrite)
							SpecularMap = Tex;
						Specular = 1.0;
						HasSpecularMap = true;
						break;
					case TEXTURE_ROUGHNESSMAP:
						if (!HasRoughnessMap || OverWrite)
							RoughnessMap = Tex;
						HasRoughnessMap = true;
						break;
					}

				}

				~Material();
				void Bind(Core::Shader & Shader);
			};

			struct MeshMaterials {
				std::vector<Material> Materials;
				std::unordered_map<const char *, int> MaterialNames;
				~MeshMaterials();
			};

			bool InitMaterials(const aiScene* pScene, const std::string& Filename, MeshMaterials &Materials);
			bool InitMesh(const aiMesh * aiMesh, Mesh & mesh);

			void LoadMeshData(const char * file, Mesh & Mesh, MeshMaterials & Materials);


			struct Model {
				Mesh ModelData; //where all of the mesh data is stored
				MeshMaterials MaterialData; //where all of the material data is stored

				unsigned int Vao, Vbo[5];
				inline Model(Mesh ModelData, MeshMaterials MaterialData) :
					Vao(),
					Vbo{},
					ModelData(ModelData),
					MaterialData(MaterialData)
				{}
				inline Model(const char *file) :
					Vao(),
					Vbo{},
					ModelData(Mesh()),
					MaterialData(MeshMaterials())
				{
					LoadMeshData(file, ModelData, MaterialData);
					PrepareForRendering(); 
				}

				inline Model() :
					Vao(),
					Vbo{},
					ModelData(Mesh()),
					MaterialData(MeshMaterials())
				{}

				void PrepareForRendering();
			};

			void DrawModel(Model & Model, Core::Shader Shader, Control::Camera Camera, Window Window);
		}
	}
}