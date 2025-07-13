#ifndef GUI_hpp 
#define GUI_hpp

class Core;

class GUI 
{
private:
    Core* core;
public:
    GUI(Core* _core)
    {
        core = _core;
    }
    void newFrame();
    void renderImGui();
};

#endif