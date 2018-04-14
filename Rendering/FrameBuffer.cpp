#include "framebuffer.h"

fRenderer::Rendering::Core::FrameBufferObject::FrameBufferObject(glm::ivec2 Resolution, bool SamplerShadow, bool alpha, bool generatemip) 
	: GenerateMip(generatemip), FrameBuffer(0), ColorBuffer(0), DepthBuffer(0), Resolution(Resolution)
{
	glGenFramebuffers(1, &FrameBuffer); 
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	glGenTextures(1, &ColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ColorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA32F : GL_RGB32F, Resolution.x, Resolution.y, 0, alpha ? GL_RGBA : GL_RGB, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffer, 0); //attach it to the frame buffer

																								 //and depth buffer

	glGenTextures(1, &DepthBuffer);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, Resolution.x, Resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

fRenderer::Rendering::Core::FrameBufferObject::FrameBufferObject() 
	: Resolution(0), FrameBuffer(0), ColorBuffer(0), DepthBuffer(0), GenerateMip(false)
{
}

void fRenderer::Rendering::Core::FrameBufferObject::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	glViewport(0, 0, Resolution.x, Resolution.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void fRenderer::Rendering::Core::FrameBufferObject::UnBind(Window Window)
{
	glBindFramebuffer(GL_FRAMEBUFFER, NULL); 
	//if (GenerateMip) {
		glBindTexture(GL_TEXTURE_2D, ColorBuffer); 
		glGenerateMipmap(GL_TEXTURE_2D); 
		glBindTexture(GL_TEXTURE_2D, NULL);
	//}
	glViewport(0, 0, Window.GetResolution().x, Window.GetResolution().y);
}

fRenderer::Rendering::Core::MultiPassFrameBufferObject::MultiPassFrameBufferObject(glm::ivec2 Resolution, int stages, bool alpha, bool generatemip) : 
	Resolution(Resolution), FrameBuffer(0), DepthBuffer(0), GenerateMip(generatemip), ColorBuffers{}
{
	glGenFramebuffers(1, &FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	//now for the color buffer 
	for (int i = 0; i < stages; i++) {
		ColorBuffers.push_back(0);
		glGenTextures(1, &ColorBuffers[i]);
		glBindTexture(GL_TEXTURE_2D, ColorBuffers[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA32F : GL_RGB32F, Resolution.x, Resolution.y, 0, alpha ? GL_RGBA : GL_RGB, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, ColorBuffers[i], 0); //attach it to the frame buffer
	}
	//and depth buffer

	glGenTextures(1, &DepthBuffer);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, Resolution.x, Resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);

	GLenum DrawBuffers[30];
	for (int i = 0; i < stages; i++)
		DrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

	glDrawBuffers(stages, DrawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

fRenderer::Rendering::Core::MultiPassFrameBufferObject::MultiPassFrameBufferObject() : 
	Resolution(0), FrameBuffer(0), DepthBuffer(0), GenerateMip(false), ColorBuffers{}
{
}

void fRenderer::Rendering::Core::MultiPassFrameBufferObject::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer); 
	glViewport(0, 0, Resolution.x, Resolution.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void fRenderer::Rendering::Core::MultiPassFrameBufferObject::unBind(Window Window)
{
	glBindFramebuffer(GL_FRAMEBUFFER, NULL); 
	
	if (GenerateMip) {
		glBindTexture(GL_TEXTURE_2D, DepthBuffer); 
		glGenerateMipmap(GL_TEXTURE_2D); 
		
		for (auto ColorBuffer : ColorBuffers) {
			glBindTexture(GL_TEXTURE_2D, ColorBuffer); 
			glGenerateMipmap(GL_TEXTURE_2D); 
		}
		glBindTexture(GL_TEXTURE_2D, NULL);
	}
	glViewport(0, 0, Window.GetResolution().x, Window.GetResolution().y);

}

unsigned int PPQuadVBO, PPQuadVAO; 

void fRenderer::Rendering::Core::PreparePostProcess()
{
	GLfloat vertices[] = {
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &PPQuadVAO);
	glGenBuffers(1, &PPQuadVBO);
	glBindVertexArray(PPQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, PPQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr); 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
		reinterpret_cast<void*>(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

void fRenderer::Rendering::Core::DrawPostProcessQuad()
{
	glBindVertexArray(PPQuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}	
