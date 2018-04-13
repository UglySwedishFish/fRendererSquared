#include "texture.h"
#include "DependenciesRendering.h"

fRenderer::Rendering::Core::Texture2D::~Texture2D()
{

}

fRenderer::Rendering::Core::Texture2D::Texture2D(unsigned int id, glm::ivec2 res) :
	ID(id), resolution(res)
{

}

void fRenderer::Rendering::Core::Texture2D::Bind(unsigned int target)
{
	glActiveTexture(GL_TEXTURE0 + target); 
	glBindTexture(GL_TEXTURE_2D,ID); 
}

fRenderer::Rendering::Core::Texture3D::~Texture3D()
{

}

fRenderer::Rendering::Core::Texture3D::Texture3D(unsigned int id, glm::ivec3 res) : 
	ID(id), resolution(res)
{

}

void fRenderer::Rendering::Core::Texture3D::Bind(unsigned int target)
{
	glActiveTexture(GL_TEXTURE0 + target);
	glBindTexture(GL_TEXTURE_3D, ID);
}

fRenderer::Rendering::Core::Texture2D fRenderer::Rendering::Core::LoadTexture2D(const char * name, bool linear, bool mipmaps)
{
	unsigned int id;
	sf::Image Image;

	if (Image.loadFromFile(name)) {

		Image.flipVertically();
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		if (mipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? (mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR) : (mipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Image.getSize().x, Image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image.getPixelsPtr());

		glBindTexture(GL_TEXTURE_2D, 0);
		return Texture2D(id, glm::ivec2(Image.getSize().x, Image.getSize().y));
	}
	else {
		return Texture2D();
		throw std::exception((std::string("Failed to load texture: ") + name).c_str());
	}
}

fRenderer::Rendering::Core::Texture3D fRenderer::Rendering::Core::EmptyTexture3D(glm::ivec3 resolution, bool linear, bool mipmaps)
{
	unsigned int id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_3D, id);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, linear ? (mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR) : (mipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST));
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, resolution.x, resolution.y, resolution.z, 0, GL_RGBA, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_3D, 0);

	return Texture3D(id, resolution);
}

#define sfgetpxl(p,i) float(i == 0 ? p.r : i == 1 ? p.g : i == 2 ? p.b : p.a) / 255.; 

fRenderer::Rendering::Core::Texture3D fRenderer::Rendering::Core::LoadTexture3D(const char * name, bool linear, bool mipmaps)
{
	unsigned int id;
	sf::Image Image;



	if (Image.loadFromFile(name)) {

		//find the actual resolution 

		unsigned int pixelCount = Image.getSize().x * Image.getSize().y;
		unsigned int res = cbrt(pixelCount);

		std::vector<float> Pixels;
		Pixels.resize(res*res*res * 4);
		for (int x = 0; x < res; x++) {
			for (int y = 0; y < res; y++) {
				for (int z = 0; z < res; z++) {

					sf::Color pixel = Image.getPixel(x*res + z, y);



					for (int color = 0; color < 4; color++) {
						Pixels[(z*(res*res) + y*res + x) * 4 + color] = sfgetpxl(pixel, color);
					}
				}
			}
		}

		Image.flipVertically();
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_3D, id);
		if (mipmaps)
			glGenerateMipmap(GL_TEXTURE_3D);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, linear ? (mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR) : (mipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST));
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, res, res, res, 0, GL_RGBA, GL_FLOAT, Pixels.data());

		glBindTexture(GL_TEXTURE_3D, 0);


		return Texture3D(id, glm::ivec3(res));

	}
	else {



		return Texture3D();
		throw std::exception((std::string("Failed to load texture: ") + name).c_str());
	}
}
