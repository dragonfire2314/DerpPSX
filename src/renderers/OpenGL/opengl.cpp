#include <renderer/OpenGL/opengl.h>
#include <gpu.h>

#include <core.h>

#include <logging.h>

OpenGL::OpenGL(Core* _core)
{
    core = _core;
}

void OpenGL::addPolygon(std::vector<Triangle> polygon)
{
    tris.insert(tris.end(), polygon.begin(), polygon.end());
}

void OpenGL::render()
{
	// glDisable(GL_DEPTH_TEST);  

	//Generate framebuffer
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	// glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, core->getGPU()->VRAM);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glViewport(0, 0, 1024, 512); // Render on the whole framebuffer, complete from the lower left corner to the upper right

	//Load VRAM into texture
	// glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &VRAMTexture);
	glBindTexture(GL_TEXTURE_2D, VRAMTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, core->getGPU()->VRAM);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, VRAMTexture);

	//Render Tris to texture (size of VRAM (1024 * 512))
	for (auto tri : tris)
	{
		printf("tri.v1.pos.x: %i, tri.v1.pos.y: %i\n", tri.v1.pos.x, tri.v1.pos.y);
		// printf("tri.v1.color.r: %i,, tri.v1.color.g: %i,, tri.v1.color.b: %i,\n", tri.v1.color.r, tri.v1.color.g, tri.v1.color.b);
		printf("tri.v1.texCoord.x: %i, tri.v1.texCoord.y: %i\n", tri.v1.texCoord.x, tri.v1.texCoord.y);
		printf("tri.v2.texCoord.x: %i, tri.v2.texCoord.y: %i\n", tri.v2.texCoord.x, tri.v2.texCoord.y);
		printf("tri.v3.texCoord.x: %i, tri.v3.texCoord.y: %i\n", tri.v3.texCoord.x, tri.v3.texCoord.y);
		const GLushort g_vertex_buffer_data[] = {
			//POSITION						//COLOR												//UV
			tri.v1.pos.x, tri.v1.pos.y, 	tri.v1.color.r, tri.v1.color.g, tri.v1.color.b,		tri.v1.texCoord.x, tri.v1.texCoord.y,
			tri.v2.pos.x, tri.v2.pos.y, 	tri.v2.color.r, tri.v2.color.g, tri.v2.color.b,		tri.v2.texCoord.x, tri.v2.texCoord.y,
			tri.v3.pos.x, tri.v3.pos.y, 	tri.v3.color.r, tri.v3.color.g, tri.v3.color.b,		tri.v3.texCoord.x, tri.v3.texCoord.y
		};

		GLuint VertexArrayID;
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		GLuint vertexbuffer;
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribIPointer(0, 2, GL_SHORT, 7 * sizeof(GLushort), (void*)0);
		glVertexAttribIPointer(1, 3, GL_SHORT, 7 * sizeof(GLushort), (void*)(2 * sizeof(GLushort)));
		glVertexAttribIPointer(2, 2, GL_SHORT, 7 * sizeof(GLushort), (void*)(5 * sizeof(GLushort)));

		glUseProgram(shader);
		(tri.clutXloc > 0) ? printf("TEXTURE\n") : printf("NOT TEX\n");
		glUniform1i(glGetUniformLocation(shader, "vram"), 0);
		printf("drawingAreaTopLeft: %i, : %i\n", core->getGPU()->drawingAreaTopLeftX, core->getGPU()->drawingAreaTopLeftY);
		printf("drawingAreaBotRight: %i, : %i\n", core->getGPU()->drawingAreaBotRightX, core->getGPU()->drawingAreaBotRightY);
		// glUniform2i(glGetUniformLocation(shader, "drawingAreaTopLeft"), core->getGPU()->drawingAreaTopLeftX, core->getGPU()->drawingAreaTopLeftY);
		glUniform2i(glGetUniformLocation(shader, "drawingAreaTopLeft"), 0, 0);
		glUniform2i(glGetUniformLocation(shader, "drawingAreaBotRight"), core->getGPU()->drawingAreaBotRightX, core->getGPU()->drawingAreaBotRightY);

		glUniform1i(glGetUniformLocation(shader, "isTexture"), (tri.clutXloc > 0) ? true : false);
		glUniform1i(glGetUniformLocation(shader, "bitDepth"), core->getGPU()->status.colorDepth);
		//Texture stuff
		printf("clutXloc: %i, clutYloc: %i\n", tri.clutXloc, tri.clutYloc);
		printf("texPageX: %i, texPageY: %i\n", tri.texPageX, tri.texPageY);
		glUniform2i(glGetUniformLocation(shader, "clut"), tri.clutXloc, tri.clutYloc);
		glUniform2i(glGetUniformLocation(shader, "texPage"), tri.texPageX, tri.texPageY);
		// printf("drawingAreaTopLeftX: %i, drawingAreaTopLeftY: %i\n", core->getGPU()->drawingAreaTopLeftX, core->getGPU()->drawingAreaTopLeftY);

		glBindTexture(GL_TEXTURE_2D, VRAMTexture);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}

	tris.clear();
	//Copy texture back into VRAM
	// glActiveTexture(GL_TEXTURE0);
	void* frameData = new uh[1024 * 512];
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, frameData);
	memcpy(core->getGPU()->VRAM, frameData, 1024 * 512 * 2) ;
	delete frameData;
	//Display Screen from VRAM, or render at higher res and display
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	GLuint quad_VertexArrayID;
	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);

	const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	float displayAreaStartX = core->getGPU()->displayAreaStartX / 1024.0f;
	float displayAreaStartY = core->getGPU()->displayAreaStartY / 512.0f;
	float width = core->getGPU()->PS_Width / 1024.0f;
	float height = core->getGPU()->PS_Height / 512.0f;

	// printf("fStartX: %f, fStartY: %f, StartX: %i, StartY: %i,\n",
	// displayAreaStartX, displayAreaStartY,
	// core->getGPU()->displayAreaStartX, core->getGPU()->displayAreaStartY);

	const GLfloat g_quad_uv_buffer_data[] = {
		/* 0.0f, 1.0f, */ displayAreaStartX, displayAreaStartY + height,
		/* 1.0f, 1.0f, */ displayAreaStartX + width, displayAreaStartY + height,
		/* 0.0f, 0.0f, */ displayAreaStartX, displayAreaStartY,

		/* 0.0f, 0.0f, */ displayAreaStartX, displayAreaStartY,
		/* 1.0f, 1.0f, */ displayAreaStartX + width, displayAreaStartY + height,
		/* 1.0f, 0.0f, */ displayAreaStartX + width, displayAreaStartY,
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);  

	glGenTextures(1, &VRAMTexture);
	glBindTexture(GL_TEXTURE_2D, VRAMTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, core->getGPU()->VRAM);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLuint quad_UVbuffer;
	glGenBuffers(1, &quad_UVbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_UVbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_uv_buffer_data), g_quad_uv_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, quad_UVbuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


	glBindTexture(GL_TEXTURE_2D, VRAMTexture);
	glUseProgram(final_shader);
	glViewport(0,0,1280,720);
	glDrawArrays(GL_TRIANGLES, 0, 6);


// GLuint VertexArrayID;
// glGenVertexArrays(1, &VertexArrayID);
// glBindVertexArray(VertexArrayID);
// static const GLfloat g_vertex_buffer_data[] = {
//    -1.0f, -1.0f, 0.0f,
//    1.0f, -1.0f, 0.0f,
//    0.0f,  1.0f, 0.0f,
// };
// // This will identify our vertex buffer
// GLuint vertexbuffer;
// // Generate 1 buffer, put the resulting identifier in vertexbuffer
// glGenBuffers(1, &vertexbuffer);
// // The following commands will talk about our 'vertexbuffer' buffer
// glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
// // Give our vertices to OpenGL.
// glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
// // 1st attribute buffer : vertices
// glEnableVertexAttribArray(0);
// glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
// glVertexAttribPointer(
//    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//    3,                  // size
//    GL_FLOAT,           // type
//    GL_FALSE,           // normalized?
//    0,                  // stride
//    (void*)0            // array buffer offset
// );
// // Draw the triangle !
// glUseProgram(shader);
// glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
// glDisableVertexAttribArray(0);


	// // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	// glGenFramebuffers(1, &FramebufferName);
	// glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	// glActiveTexture(GL_TEXTURE0);
	// glGenTextures(1, &renderedTexture);
	// glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, core->getGPU()->PS_Width, core->getGPU()->PS_Height, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, 0);

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// // Set "renderedTexture" as our colour attachement #0
	// glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// // Set the list of draw buffers.
	// GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	// glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// // Render to our framebuffer
	// glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	// glViewport(0,0,core->getGPU()->PS_Width, core->getGPU()->PS_Height); // Render on the whole framebuffer, complete from the lower left corner to the upper right


	// //Set up VRAM as texture for shader to access
	// //We need VRAM to allow the shader to grab texture and clut data for rendering
	// glActiveTexture(GL_TEXTURE1);
	// glGenTextures(1, &VRAMTexture);
	// glBindTexture(GL_TEXTURE_2D, VRAMTexture);

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, core->getGPU()->VRAM);

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // for (auto polygon : polygons) 
    // {
	// 	if (4 == polygon.verts.size()) 
	// 	{
	// 		GLfloat vertex_buffer_data[18] = {
	// 			polygon.verts[0].f_x, polygon.verts[0].f_y, 0.0f,
	// 			polygon.verts[1].f_x, polygon.verts[1].f_y, 0.0f,
	// 			polygon.verts[2].f_x, polygon.verts[2].f_y, 0.0f,

	// 			polygon.verts[1].f_x, polygon.verts[1].f_y, 0.0f,
	// 			polygon.verts[3].f_x, polygon.verts[3].f_y, 0.0f,
	// 			polygon.verts[2].f_x, polygon.verts[2].f_y, 0.0f
	// 		};

	// 		GLfloat color_buffer_data[18] = {
	// 			polygon.verts[0].r, polygon.verts[0].g, polygon.verts[0].b,
	// 			polygon.verts[1].r, polygon.verts[1].g, polygon.verts[1].b,
	// 			polygon.verts[2].r, polygon.verts[2].g, polygon.verts[2].b,

	// 			polygon.verts[1].r, polygon.verts[1].g, polygon.verts[1].b,
	// 			polygon.verts[3].r, polygon.verts[3].g, polygon.verts[3].b,
	// 			polygon.verts[2].r, polygon.verts[2].g, polygon.verts[2].b
	// 		};

	// 		GLfloat uv_buffer_data[12] = {
	// 			polygon.verts[0].texCoordX, polygon.verts[0].texCoordY,
	// 			polygon.verts[1].texCoordX, polygon.verts[1].texCoordY,
	// 			polygon.verts[2].texCoordX, polygon.verts[2].texCoordY,

	// 			polygon.verts[1].texCoordX, polygon.verts[1].texCoordY,
	// 			polygon.verts[3].texCoordX, polygon.verts[3].texCoordY,
	// 			polygon.verts[2].texCoordX, polygon.verts[2].texCoordY
	// 		};

	// 		GLint page[1] = {polygon.page};
	// 		GLint palette[1] = {polygon.palette};

	// 		glDisable(GL_BLEND);

	// 		unsigned int VAO;
	// 		glGenVertexArrays(1, &VAO);
	// 		glBindVertexArray(VAO);  

	// 		unsigned int vertexBuffer;
	// 		glGenBuffers(1, &vertexBuffer);  
	// 		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// 		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

	// 		unsigned int colorbuffer;
	// 		glGenBuffers(1, &colorbuffer);
	// 		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	// 		glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

	// 		unsigned int UVbuffer;
	// 		glGenBuffers(1, &UVbuffer);
	// 		glBindBuffer(GL_ARRAY_BUFFER, UVbuffer);
	// 		glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);

	// 		unsigned int Pagebuffer;
	// 		glGenBuffers(1, &Pagebuffer);
	// 		glBindBuffer(GL_ARRAY_BUFFER, Pagebuffer);
	// 		glBufferData(GL_ARRAY_BUFFER, sizeof(page), page, GL_STATIC_DRAW);

	// 		unsigned int Palettebuffer;
	// 		glGenBuffers(1, &Palettebuffer);
	// 		glBindBuffer(GL_ARRAY_BUFFER, Palettebuffer);
	// 		glBufferData(GL_ARRAY_BUFFER, sizeof(palette), palette, GL_STATIC_DRAW);

	// 		glEnableVertexAttribArray(0);
	// 		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// 		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 		glEnableVertexAttribArray(1);
	// 		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	// 		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 		glEnableVertexAttribArray(2);
	// 		glBindBuffer(GL_ARRAY_BUFFER, UVbuffer);
	// 		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 		glEnableVertexAttribArray(3);
	// 		glBindBuffer(GL_ARRAY_BUFFER, Pagebuffer);
	// 		glVertexAttribIPointer(3, 1, GL_INT, 0, (void*)0);

	// 		glEnableVertexAttribArray(4);
	// 		glBindBuffer(GL_ARRAY_BUFFER, Palettebuffer);
	// 		glVertexAttribIPointer(4, 1, GL_INT, 0, (void*)0);	

	// 		glUseProgram(shader);
	// 		glDrawArrays(GL_TRIANGLES, 0, 6);
	// 	}
	// 	else if (3 == polygon.verts.size()) 
	// 	{
	// 		GLfloat vertex_buffer_data[9] = {
	// 			polygon.verts[0].f_x, polygon.verts[0].f_y, 0.0f,
	// 			polygon.verts[1].f_x, polygon.verts[1].f_y, 0.0f,
	// 			polygon.verts[2].f_x, polygon.verts[2].f_y, 0.0f,
	// 		};

	// 		GLfloat color_buffer_data[9] = {
	// 			polygon.verts[0].r, polygon.verts[0].g, polygon.verts[0].b,
	// 			polygon.verts[1].r, polygon.verts[1].g, polygon.verts[1].b,
	// 			polygon.verts[2].r, polygon.verts[2].g, polygon.verts[2].b,
	// 		};

	// 		glDisable(GL_BLEND);

	// 		unsigned int VAO;
	// 		glGenVertexArrays(1, &VAO);
	// 		glBindVertexArray(VAO);  

	// 		unsigned int vertexBuffer;
	// 		glGenBuffers(1, &vertexBuffer);  
	// 		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// 		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

	// 		unsigned int colorbuffer;
	// 		glGenBuffers(1, &colorbuffer);
	// 		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	// 		glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

	// 		glEnableVertexAttribArray(0);
	// 		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// 		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 		glEnableVertexAttribArray(1);
	// 		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	// 		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 		glUseProgram(shader);
	// 		glDrawArrays(GL_TRIANGLES, 0, 3);
	// 	}
    // }

    // polygons.clear();

	// // Store the rendered texture back into the VARM
	// glActiveTexture(GL_TEXTURE0);
	// void* frameData = new uh[core->getGPU()->PS_Width * core->getGPU()->PS_Height];
	// glBindTexture(GL_PIXEL_PACK_BUFFER, renderedTexture);
	// glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, frameData);
	// core->getGPU()->gpu_store_frame(frameData);
	// delete frameData;

	// // The fullscreen quad's FBO
	// GLuint quad_VertexArrayID;
	// glGenVertexArrays(1, &quad_VertexArrayID);
	// glBindVertexArray(quad_VertexArrayID);

	// const GLfloat g_quad_vertex_buffer_data[] = {
	// 	-1.0f, -1.0f, 0.0f,
	// 	1.0f, -1.0f, 0.0f,
	// 	-1.0f,  1.0f, 0.0f,
	// 	-1.0f,  1.0f, 0.0f,
	// 	1.0f, -1.0f, 0.0f,
	// 	1.0f,  1.0f, 0.0f,
	// };

	// const GLfloat g_quad_uv_buffer_data[] = {
	// 	0.0f, 0.0f,
	// 	1.0f, 0.0f, 
	// 	0.0f, 1.0f,
	// 	0.0f, 1.0f,
	// 	1.0f, 0.0f, 
	// 	1.0f, 1.0f, 
	// };

	// unsigned int VAO;
	// glGenVertexArrays(1, &VAO);
	// glBindVertexArray(VAO);  

	// GLuint quad_vertexbuffer;
	// glGenBuffers(1, &quad_vertexbuffer);
	// glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	// glEnableVertexAttribArray(0);
	// glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// GLuint quad_UVbuffer;
	// glGenBuffers(1, &quad_UVbuffer);
	// glBindBuffer(GL_ARRAY_BUFFER, quad_UVbuffer);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_uv_buffer_data), g_quad_uv_buffer_data, GL_STATIC_DRAW);

	// glEnableVertexAttribArray(1);
	// glBindBuffer(GL_ARRAY_BUFFER, quad_UVbuffer);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// glUseProgram(final_shader);
	// glViewport(0,0,1280,720);
	// glDrawArrays(GL_TRIANGLES, 0, 6);
}

void OpenGL::clearScreen()
{
    glClearColor(0.0f, 0.4f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL::init()
{
    // glViewport(0, 0, 800, 600);

    // shader = gpuLoadShaders("J:/Programming/c++/DerpPSX_CMAKE/shaders/vertex.glsl", "J:/Programming/c++/DerpPSX_CMAKE/shaders/pixel.glsl");
	final_shader = gpuLoadShaders("shaders/final_vertex.glsl", "shaders/final_pixel.glsl");

	shader = gpuLoadShaders("shaders/test_vertex.glsl", "shaders/test_pixel.glsl");
}

GLuint OpenGL::gpuLoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
		system("PAUSE");
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
		system("PAUSE");
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
		system("PAUSE");
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}