#version 330 core

layout(location = 0) in ivec2 aPos;
layout(location = 1) in ivec3 aColor;
layout(location = 2) in ivec2 aTexCoord;

uniform ivec2 drawingAreaTopLeft;
uniform ivec2 drawingAreaBotRight;

out vec3 ourColor;
out vec2 realUV;

void main ()
{
    ourColor = aColor / 255.0;

    gl_Position.xy = aPos;
    gl_Position.z = 0;
    gl_Position.w = 1.0;

    //TODO - Clip rendering bounds

    ivec2 pos = ivec2(aPos.xy) + drawingAreaTopLeft;
    vec2 f_pos;

    f_pos.x = pos.x / 512.0;
    f_pos.y = pos.y / 256.0;

    f_pos.x -= 1.0;
    f_pos.y -= 1.0;

    gl_Position.xy = f_pos;

    vec2 f_UV;
    f_UV.x = float(aTexCoord.x) / 255.0;
    f_UV.y = float(aTexCoord.y) / 255.0;

    realUV = f_UV;
}

// #version 330 core
// layout (location = 0) in vec3 aPos;

// void main()
// {
//     gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
// }