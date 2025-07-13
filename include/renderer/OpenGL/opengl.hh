#ifndef OPEN_GL_hpp 
#define OPEN_GL_hpp

#include <renderer/renderAPI.hh>

#include <GL/glew.h>

#include <vector>

class Core;

class OpenGL : public RenderAPI
{
private:
	std::vector<Triangle> tris;

	unsigned int shader;
	unsigned int final_shader;

	GLuint FramebufferName = 0;
	GLuint renderedTexture;
	GLuint VRAMTexture;

	GLuint gpuLoadShaders(const char* vertex_file_path, const char* fragment_file_path);
public:
	OpenGL(Core *_core);
	void render();
	void clearScreen();
	void init();
	void addPolygon(std::vector<Triangle> polygon);
};

#endif