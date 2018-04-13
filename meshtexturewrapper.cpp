#include "meshtexturewrapper.h"

fRenderer::Rendering::Mesh::WrappedModel::WrappedModel()
{
}

void SetPixelByte(std::vector<GLubyte> &Vector, int Index, Vector3u Pixel) {
	Vector[Index] = Pixel.x; 
	Vector[Index + 1] = Pixel.y; 
	Vector[Index + 2] = Pixel.z; 
}

#define EmptyDataVector std::vector<Vector3f>(Resolution*Resolution)

Vector3f To3DFrom2DF(Vector2f v, float f) { return Vector3f(v.x, v.y, f); }




bool fRenderer::Rendering::Mesh::WrappedModel::Wrap(Model WrapModel)
{
	const unsigned int Resolution = ceil(	sqrt(	WrapModel.ModelData.Vertices.size()		)	); 

	MeshDataResolution = Vector2i(Resolution, Resolution); 

	//MODEL DATA: 

	std::vector<Vector3f> ModelData[5] = { EmptyDataVector,EmptyDataVector,EmptyDataVector,EmptyDataVector,EmptyDataVector };
	// Vertices, Normals, Tangents, TextureCoordinates + Material

	int CurrentIndex = 0; //current model index

	for (int x = 0; x < Resolution; x++) {
		for (int y = 0; y < Resolution; y++) {
			
			if (CurrentIndex > WrapModel.ModelData.Vertices.size() - 1)
				goto LoopExit; //is this ok? 

			int CurrentMaterialIndex = WrapModel.ModelData.Materials[CurrentIndex]; 

			ModelData[0][x*Resolution + y] = WrapModel.ModelData.Vertices[CurrentIndex]; //vertices
			ModelData[1][x*Resolution + y] = WrapModel.ModelData.Normals[CurrentIndex]; //normals
			ModelData[2][x*Resolution + y] = WrapModel.ModelData.Tangents[CurrentIndex]; //tangents 
			ModelData[3][x*Resolution + y] = To3DFrom2DF(WrapModel.ModelData.TextureCoordinates[CurrentIndex], static_cast<float>(CurrentMaterialIndex)/10.0); //texture coordinates and material id 
			CurrentIndex++; //iterate the current model index 
		}
	}
LoopExit: 

	glGenTextures(1, &MeshData); //generate the texture for the mesh data 
	glBindTexture(GL_TEXTURE_2D_ARRAY, MeshData); 
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB32F, Resolution, Resolution, 4, 0, GL_RGB, GL_FLOAT, NULL);
	for (int SubTexture = 0; SubTexture < 4; SubTexture++)
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, SubTexture, Resolution, Resolution, 1, GL_RGB, GL_FLOAT, &ModelData[SubTexture][0]); //store the mesh data
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_MIN_FILTER,
		GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	//MATERIAL DATA: 

	std::vector<GLubyte> BaseMaterialData(MATERIAL_TEXTURE_QUALITY_SQUARED*3); //the base data for the material 

	glGenTextures(1, &MaterialData);
	glBindTexture(GL_TEXTURE_2D_ARRAY, MaterialData);


	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 4);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, 256, 256, WrapModel.MaterialData.Materials.size(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	for (int Material = 0; Material < std::min(static_cast<int>(WrapModel.MaterialData.Materials.size()), MATERIAL_TEXTURE_QUALITY); Material++) {
		int Index = Material * MATERIAL_TEXTURE_QUALITY * 3; 
		SetPixelByte(BaseMaterialData, Index, Vector3u(WrapModel.MaterialData.Materials[Material].DiffuseColor * 256.f)); //diffusecolor is multiplied by 256.f because I don't think I've ever seen any material being above 1.0 in any of the individual colors 
		SetPixelByte(BaseMaterialData, Index + 3, Vector3u(WrapModel.MaterialData.Materials[Material].SpecularColor * 256.f)); //^ 
		SetPixelByte(BaseMaterialData, Index + 6, Vector3u(WrapModel.MaterialData.Materials[Material].Emmisive*16.f, WrapModel.MaterialData.Materials[Material].Specular*256.f, WrapModel.MaterialData.Materials[Material].Roughness*256.f)); //emmision is only multiplied by 16.f for higher potential values (you do see emmsivie surfaces >1 sometimes!) 
		SetPixelByte(BaseMaterialData, Index + 9, Vector3u(WrapModel.MaterialData.Materials[Material].Refractivity * 256.f, WrapModel.MaterialData.Materials[Material].RefractiveIndex * 256.f, WrapModel.MaterialData.Materials[Material].HasAlbedoMap ? 255 : 0));
	}

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
		0,
		0, 0, 0,
		256, 256, 1,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		&BaseMaterialData[0]);

	//(VERY) HACKY IMAGE SHIT: 

	for (int Material = 0; Material < std::min(static_cast<int>(WrapModel.MaterialData.Materials.size()), MATERIAL_TEXTURE_QUALITY); Material++) { 
		//the set amount of materials is limited to MATERIAL_TEXTURE_QUALITY (for simplicities sake, we could of course change that in the future for potentially unlimited material
		if (WrapModel.MaterialData.Materials[Material].HasAlbedoMap) {
			std::vector<float> Data(WrapModel.MaterialData.Materials[Material].AlbedoMap.resolution.x*WrapModel.MaterialData.Materials[Material].AlbedoMap.resolution.y * 3);
			glBindTexture(GL_TEXTURE_2D, WrapModel.MaterialData.Materials[Material].AlbedoMap.ID);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &Data[0]);
			glBindTexture(GL_TEXTURE_2D, 0);

			sf::Image NewImage = sf::Image(); 
			NewImage.create(MATERIAL_TEXTURE_QUALITY, MATERIAL_TEXTURE_QUALITY); //create an empty image

			for (int x = 0; x < MATERIAL_TEXTURE_QUALITY; x++) {
				for (int y = 0; y < MATERIAL_TEXTURE_QUALITY; y++) {
					int RequestedPixelY = floor(static_cast<float>(x) * (static_cast<float>(WrapModel.MaterialData.Materials[Material].AlbedoMap.resolution.x)) / 256.f);
					int RequestedPixelX = floor(static_cast<float>(y) * (static_cast<float>(WrapModel.MaterialData.Materials[Material].AlbedoMap.resolution.y)) / 256.f);
					sf::Color c;
					c.r = std::min(std::max(static_cast<int>(Data[((RequestedPixelY * WrapModel.MaterialData.Materials[Material].AlbedoMap.resolution.y) + RequestedPixelX) * 3] *     255), 0), 255);
					c.g = std::min(std::max(static_cast<int>(Data[((RequestedPixelY * WrapModel.MaterialData.Materials[Material].AlbedoMap.resolution.y) + RequestedPixelX) * 3 + 1] * 255), 0), 255);
					c.b = std::min(std::max(static_cast<int>(Data[((RequestedPixelY * WrapModel.MaterialData.Materials[Material].AlbedoMap.resolution.y) + RequestedPixelX) * 3 + 2] * 255), 0), 255);
					NewImage.setPixel(x, y, c); //set the pixel 

				}
			}

			glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
				0,
				0, 0, Material,
				256, 256, 1,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				NewImage.getPixelsPtr()); //super straight forward 

		}
	}


	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0); //unbind texture 



}
