#version 330 core

out vec4 color;
in vec3 ourColor;

in vec2 realUV;

uniform ivec2 clut;
uniform ivec2 texPage;

uniform bool isTexture;
uniform int bitDepth;

uniform sampler2D vram;

int internalToPsxColor(vec4 c) {
    int a = int(floor(c.a + 0.5));
    int r = int(floor(c.r * 31.0 + 0.5));
    int g = int(floor(c.g * 31.0 + 0.5));
    int b = int(floor(c.b * 31.0 + 0.5));
    return (b << 10) | (g << 5) | r;
}

void main()
{
    if (isTexture) 
    {
        //Real UV is a point in 0-1.0 space, convert back to 0-255
        ivec2 uvPSX = ivec2(realUV.x * 64.0, realUV.y * 256.0);
        // ivec2 clut = ivec2(clut.x / 16, clut.y);
        // ivec2 texOffset = ivec2(texPage.x * 64, texPage.y * 256);

        vec4 texColor = texelFetch(vram, uvPSX + texPage, 0);
        int psxColor = internalToPsxColor(texColor);

        //Figure out which of the 4 pixels I am on
        int fullScaleUV_x = int(realUV.x * 256.0);
        int subPixel = fullScaleUV_x % 4;

        int clutIndex = (psxColor >> (4 * subPixel)) & 15;

        vec4 realColor = texelFetch(vram, ivec2((clutIndex + clut.x), clut.y), 0);

        // color.r = texture( vram, realUV ).r;
        // color.a = 1.0;

        color = vec4(ourColor.r, ourColor.g, ourColor.b, 1.0f);
        // color = realColor / 32;
        color.a = 1.0;

        color = realColor;
        color.a = 1.0;

        // if (clutIndex == 0)
        // {
        //     color = vec4(0.0, 0.0, 0.0, 0.0);
        // }

        if (color.rgb == vec3(0.0, 0.0, 0.0)) 
        {
            color.a = 0.0;
        }

        // if (clutIndex == 0) { color.g = (1.0 / 16) * 0; }
        // if (clutIndex == 1) { color.g = (1.0 / 16) * 1; }
        // if (clutIndex == 2) { color.g = (1.0 / 16) * 2; }
        // if (clutIndex == 3) { color.g = (1.0 / 16) * 3; }
        // if (clutIndex == 4) { color.g = (1.0 / 16) * 4; }
        // if (clutIndex == 5) { color.g = (1.0 / 16) * 5; }
        // if (clutIndex == 6) { color.g = (1.0 / 16) * 6; }
        // if (clutIndex == 7) { color.g = (1.0 / 16) * 7; }
        // if (clutIndex == 8) { color.g = (1.0 / 16) * 8; }
        // if (clutIndex == 9) { color.g = (1.0 / 16) * 9; }
        // if (clutIndex == 10) { color.g = (1.0 / 16) * 10; }
        // if (clutIndex == 11) { color.g = (1.0 / 16) * 11; }
        // if (clutIndex == 12) { color.g = (1.0 / 16) * 12; }
        // if (clutIndex == 13) { color.g = (1.0 / 16) * 13; }
        // if (clutIndex == 14) { color.g = (1.0 / 16) * 14; }
        // if (clutIndex == 15) { color.g = (1.0 / 16) * 15; }
    }
    else 
    {
        color = vec4(ourColor.r, ourColor.g, ourColor.b, 1.0f);
        // color = realColor / 32;
        color.a = 1.0;
    }

    // color = vec4(float(clut.x), float(clut.y), 0.0, 1.0);
}