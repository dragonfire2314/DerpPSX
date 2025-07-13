#include "app.hh"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
// #include "../extern/stb_image_write.h"

#include <tomlplusplus/toml.hpp>

#include <logging.hh>
#include <configuration.hh>
#include <chrono>

Application::Application()
{
	configuration.loadConfiguration();

	core.init(&configuration);

    glfw_init();
    imgui_init();

	debugger = new GUI(&core);

	runner = new Runner(&core);

	runner->emu_init();

	core.addDebugger((Debugger*)runner);
}

std::string Application::getBiosFile()
{
	return configuration.getAppConfiguration().biosLocation;
}

void Application::Run()
{
    do {
		runner->step();

		core.getRenderer()->clearScreen();

		//IM GUI new frame
        debugger->newFrame();

		//Do some rendering
        core.getRenderer()->render();

		//GUI render
		debugger->renderImGui();

		//Convert VRAM
		// ub* PIXELS = new ub[1024 * 512 * 3];
		// for (int i = 0; i < 1024 * 512; i++) {
		// 	uh temp = core.getGPU()->VRAM[i];
		// 	PIXELS[(i * 3)    ] = ((temp & 0x1F) * 8);
		// 	PIXELS[(i * 3) + 1] = ((temp & 0x3E0) >> 5 ) * 8;
		// 	PIXELS[(i * 3) + 2] = ((temp & 0x7C00) >> 10) * 8;
		// }

		// stbi_write_bmp("Test.bmp", 1024, 512, 3, PIXELS);


		// glDisable(GL_BLEND);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
}

void Application::imgui_init()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	float SCALE = 1.5f;
	ImFontConfig cfg;
	cfg.SizePixels = 13 * SCALE;
	io.Fonts->AddFontDefault(&cfg);
}

void Application::glfw_init()
{
    //Create a window
    glewExperimental = true; // Needed for core profile
	if (!glfwInit())
	{
        //TODO - Exit?
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(configuration.getAppConfiguration().width, configuration.getAppConfiguration().height, configuration.getAppConfiguration().windowName.c_str(), NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		glfwTerminate();
	}
	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
