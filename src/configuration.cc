#include <configuration.hh>

#include <tomlplusplus/toml.hpp>

Configuration::Configuration()
{
}

bool Configuration::loadConfiguration()
{
    return parse();
}

bool Configuration::parse()
{
    auto toml = toml::parse_file( configFile );

    std::string strData;
    int intData;


	strData = toml["bios"]["location"].value_or("");
	appConfig.biosLocation = strData;

    intData = toml["app"]["screenWidth"].value_or(600);
	appConfig.width = intData;

    intData = toml["app"]["screenHeight"].value_or(800);
	appConfig.height = intData;

    strData = toml["app"]["windowName"].value_or("");
	appConfig.windowName = strData;

    return true;
}

Configuration::Application& Configuration::getAppConfiguration()
{
    return appConfig;
}
