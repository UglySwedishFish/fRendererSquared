#include "Mesh.h"
#include <Dependencies\DependenciesRendering.h> 
#include <iostream> 

void fRenderer::Rendering::Mesh::Mesh::ClearVectors()
{
}

fRenderer::Rendering::Mesh::Mesh::~Mesh()
{
	ClearVectors(); 
}

fRenderer::Rendering::Mesh::Material::~Material()
{

}

void fRenderer::Rendering::Mesh::Material::Bind(Core::Shader & Shader)
{
	glUniform1f(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.Roughness"), Roughness);
	glUniform1f(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.Refractivity"), Refractivity);
	glUniform1f(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.Specularity"), Specular);
	glUniform1f(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.Emmision"), Emmisive);
	glUniform3f(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.DiffColor"), DiffuseColor.r, DiffuseColor.g, DiffuseColor.b);
	glUniform1f(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.RefractiveIndex"), RefractiveIndex);

	glUniform1i(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.HasTexture"), AlbedoMap.ID != -1);
	if (AlbedoMap.ID != -1) {
		AlbedoMap.Bind(20);
		glUniform1i(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.Texture"), 20);
	}


	glUniform1i(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.HasNormalMap"), NormalMap.ID != -1);
	if (NormalMap.ID != -1) {
		NormalMap.Bind(21);
		glUniform1i(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.NormalMap"), 21);
	}


	glUniform1i(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.HasSpecularMap"), SpecularMap.ID != -1);
	if (SpecularMap.ID != -1) {
		SpecularMap.Bind(22);
		glUniform1i(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.SpecularMap"), 22);
	}



	glUniform1i(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.HasRoughnessMap"), RoughnessMap.ID != -1);
	if (RoughnessMap.ID != -1) {
		RoughnessMap.Bind(24);
		glUniform1i(glGetUniformLocation(Shader.ShaderID, "FragmentMaterial.RoughnessMap"), 24);
	}
}

fRenderer::Rendering::Mesh::MeshMaterials::~MeshMaterials()
{
}

bool fRenderer::Rendering::Mesh::InitMaterials(const aiScene * pScene, const std::string & Filename, MeshMaterials & Materials)
{
	std::string::size_type SlashIndex = Filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;


	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		aiColor3D diffuse = aiColor3D();
		aiColor3D specular = aiColor3D();
		float Reflectivity, Shininess, Refractivity;

		aiString NAME;
		aiReturn rDiff = pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

		aiReturn rRefl = pMaterial->Get(AI_MATKEY_REFLECTIVITY, Reflectivity);
		aiReturn rRefr = pMaterial->Get(AI_MATKEY_OPACITY, Refractivity);
		aiReturn rShin = pMaterial->Get(AI_MATKEY_SHININESS, Shininess);


		aiReturn rName = pMaterial->Get(AI_MATKEY_NAME, NAME);
		aiReturn rSpec = pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);

		glm::vec3 DiffuseComponent = glm::vec3(diffuse.r, diffuse.g, diffuse.b);



		glm::vec3 SpecularComponent = glm::vec3(1.0);
		if (rSpec == aiReturn_FAILURE)
			SpecularComponent = glm::vec3(specular.r, specular.g, specular.b);
		Materials.Materials[i].SpecularColor = SpecularComponent;

		Materials.Materials[i].DiffuseColor = DiffuseComponent;

		if (rRefl == aiReturn_SUCCESS)
			Materials.Materials[i].Specular = Reflectivity;
		else
			Materials.Materials[i].Specular = 0.0;

		Materials.Materials[i].Refractivity = 0.0;


		if (rName == aiReturn_FAILURE) {
			NAME = std::to_string(i).c_str();
			std::cout << "Could not recive material name!\n";
		}
		std::string eName = NAME.C_Str();
		Materials.MaterialNames[eName.c_str()] = i;
		std::cout << "Material: " << eName << " has value: " << i << '\n';

		Material_Texture_Types textureTypes[4] = { TEXTURE_ALBEDOMAP, TEXTURE_NORMALMAP, TEXTURE_SPECULARMAP, TEXTURE_PARALLAXMAP };
		aiTextureType loopElements[4] = { aiTextureType::aiTextureType_DIFFUSE,aiTextureType::aiTextureType_AMBIENT,aiTextureType::aiTextureType_SPECULAR,aiTextureType::aiTextureType_DISPLACEMENT };
		for (int j = 0; j < 4; j++) {
			if (pMaterial->GetTextureCount(loopElements[j]) > 0) {
				aiString Path;

				if (pMaterial->GetTexture(loopElements[j], 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
					std::string p(Path.data);



					std::string FullPath = p;

					Core::Texture2D Texture = Core::LoadTexture2D(FullPath.c_str(), true, false);

					Materials.Materials[i].AllocateTexture(textureTypes[j], Texture); //we allocate this texture

				}
			}
		} //handles the texturing

	}
	return true;
}

const aiVector3D ai_Null(0.0, 0.0, 0.0);

bool fRenderer::Rendering::Mesh::InitMesh(const aiMesh * aiMesh, Mesh & mesh)
{
	for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) {
		const aiVector3D *VertexPos = &(aiMesh->mVertices[i]);
		const aiVector3D *VertexNormal = &(aiMesh->mNormals[i]);
		const aiVector3D* VertexTextureCoordinate = aiMesh->HasTextureCoords(0) ? &(aiMesh->mTextureCoords[0][i]) : &ai_Null;
		const aiVector3D* VertexTangent = aiMesh->HasTangentsAndBitangents() ? &(aiMesh->mTangents[i]) : &ai_Null;

		mesh.Vertices.push_back(glm::vec3(VertexPos->x, VertexPos->y, VertexPos->z));
		mesh.Normals.push_back(glm::vec3(VertexNormal->x, VertexNormal->y, VertexNormal->z));
		mesh.TextureCoordinates.push_back(glm::vec3(VertexTextureCoordinate->x, VertexTextureCoordinate->y, VertexTextureCoordinate->z));
		mesh.Tangents.push_back(glm::vec3(VertexTangent->x, VertexTangent->y, VertexTangent->z));
		mesh.Materials.push_back(aiMesh->mMaterialIndex);
	}

	for (unsigned int i = 0; i < aiMesh->mNumFaces; i++) {
		const aiFace& Face = aiMesh->mFaces[i];
		if (Face.mNumIndices == 3) { //if it isn't a triangle, skip this face
			mesh.Indices.push_back(Face.mIndices[0]);
			mesh.Indices.push_back(Face.mIndices[1]);
			mesh.Indices.push_back(Face.mIndices[2]);
		}
		else {
			std::cout << "Face: " << i << " is not a triangle! \n";
		}
	}
	return true;
}

void fRenderer::Rendering::Mesh::LoadMeshData(const char * file, Mesh & Mesh, MeshMaterials & Materials)
{
	Assimp::Importer ObjectImporter;

	const aiScene * Scene = ObjectImporter.ReadFile(file, aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);

	if (Scene) {

		Mesh.MeshEntries.resize(Scene->mNumMeshes);
		Materials.Materials.resize(Scene->mNumMaterials);	


		unsigned int NumVertices = 0;
		unsigned int NumIndices = 0;

		// Count the number of vertices and indices
		for (unsigned int i = 0; i < Mesh.MeshEntries.size(); i++) {
			Mesh.MeshEntries[i].MaterialIndex = Scene->mMeshes[i]->mMaterialIndex;
			Mesh.MeshEntries[i].NumIndices = Scene->mMeshes[i]->mNumFaces * 3;
			Mesh.MeshEntries[i].BaseVertex = NumVertices;
			Mesh.MeshEntries[i].BaseIndex = NumIndices;

			NumVertices += Scene->mMeshes[i]->mNumVertices;
			NumIndices += Mesh.MeshEntries[i].NumIndices;
		}

		// Reserve space in the vectors for the vertex attributes and indices
		Mesh.Vertices.reserve(NumVertices);
		Mesh.Normals.reserve(NumVertices);
		Mesh.TextureCoordinates.reserve(NumVertices);
		Mesh.Tangents.reserve(NumVertices);
		Mesh.Indices.reserve(NumIndices);
		Mesh.Materials.reserve(NumVertices);
		InitMaterials(Scene, file, Materials);


		for (unsigned int i = 0; i < Mesh.MeshEntries.size(); i++) {
			const aiMesh* aiMesh = Scene->mMeshes[i];
			InitMesh(aiMesh, Mesh);
		}
	}
	else {
		std::cout << "Failed to load model: " << file << " (Error: " << ObjectImporter.GetErrorString() << ") \n";
	}
}

void fRenderer::Rendering::Mesh::DrawModel(Model & _Model, Core::Shader _Shader, Control::Camera _Camera, Window _Window)
{
	glBindVertexArray(_Model.Vao);

	for (int i = 0; i < _Model.ModelData.MeshEntries.size(); i++) {

		unsigned int MaterialIndex = _Model.ModelData.MeshEntries[i].MaterialIndex;

		//glUniform3f(glGetUniformLocation(_Shader.ShaderID, "CamPos"), _Camera.Position.x, _Camera.Position.y, _Camera.Position.z);
		//glUniform1f(glGetUniformLocation(_Shader.ShaderID, "Time"), _Window.GetTimeOpened());

		_Model.MaterialData.Materials[MaterialIndex].Bind(_Shader);

		glDrawElementsBaseVertex(GL_TRIANGLES,
			_Model.ModelData.MeshEntries[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int) * _Model.ModelData.MeshEntries[i].BaseIndex),
			_Model.ModelData.MeshEntries[i].BaseVertex);
	}



	glBindVertexArray(0);
}

void fRenderer::Rendering::Mesh::Model::PrepareForRendering()
{
	glGenVertexArrays(1, &Vao);
	glBindVertexArray(Vao);
	glGenBuffers(5, Vbo);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ModelData.Vertices[0]) * ModelData.Vertices.size(), &ModelData.Vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ModelData.TextureCoordinates[0]) * ModelData.TextureCoordinates.size(), &ModelData.TextureCoordinates[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ModelData.Normals[0]) * ModelData.Normals.size(), &ModelData.Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);


	glBindBuffer(GL_ARRAY_BUFFER, Vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ModelData.Tangents[0]) * ModelData.Tangents.size(), &ModelData.Tangents[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Vbo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ModelData.Indices[0]) * ModelData.Indices.size(), &ModelData.Indices[0], GL_STATIC_DRAW);



	glBindVertexArray(0);
}
