#ifndef RENDER_API_hpp 
#define RENDER_API_hpp

#include <gpu.h>
    
class Core;

//Abstract class that is overrided by a graphics class
class RenderAPI
{
private:
protected:
	Core* core;
public:
	virtual void render() = 0;
	virtual void clearScreen() = 0;
	virtual void init() = 0;

	virtual void addPolygon(std::vector<Triangle> polygon) = 0;
};

#endif