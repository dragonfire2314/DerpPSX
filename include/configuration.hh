#ifndef CONFIGURATION_hpp 
#define CONFIGURATION_hpp

#include <global.hh>

class Configuration
{
public:
    std::string configFile = "DerpPSX_Config.toml";

    struct Application
    {
        int width;
        int height;
        std::string windowName;

        std::string biosLocation;
    };
    struct GPU
    {
        int width;
        int height;
    };
private:
    Application appConfig;
    GPU gpuConfig;

    bool parse();
public:
    Configuration();

    bool loadConfiguration();

    Application& getAppConfiguration();
};

#endif
