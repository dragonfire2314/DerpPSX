#include <sstream>
#include <vector>

#include "app.h"
#include <iostream>
#include <logging.h>

int main(int argc, char *argv[]) 
{
	printf("%s\n", argv[0]);

	Application_Config config;
	config.height = 720;
	config.width = 1280;
	config.windowName = "DerpPSX";

	Application DerpPSX(config);

	LOG::debug(LOG::SystemLocation::APP, "Starting: %s", config.windowName.c_str());

	DerpPSX.Run();
}
