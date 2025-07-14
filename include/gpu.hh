#ifndef GPU_HPP
#define GPU_HPP

#include <global.hh>
#include <stdlib.h>     /* srand, rand */
#include <fstream>
#include <iomanip>

class Core;

//Vertex Stuct
struct GPU_Vert
{
	void setXY(uw width, uw height, uh x, uh y) 
	{
		if (x == 0) f_x = -1.0f;
		f_x = (float)(((((float)x - 0)/(width - 0)) * 2.0f) - 1);
	
		if (y == 0) f_y = -1.0f;
		f_y = (float)(((((float)y - 0) / (height - 0)) * 2.0f) - 1);
	}
	void setXY(uw width, uw height, uw vertCom) 
	{
		uw x, y;
		x = (vertCom & 0xFFFF);
		y = (vertCom & 0xFFFF0000) >> 16;
		if (x == 0) f_x = -1.0f;
		f_x = (float)(((((float)x - 0)/(width - 0)) * 2.0f) - 1);
	
		if (y == 0) f_y = -1.0f;
		f_y = (float)(((((float)y - 0) / (height - 0)) * 2.0f) - 1);
	}
	void setRGB(uw colorCom) 
	{
		r = (colorCom & 0xFF) / 255.0f;
		g = ((colorCom & 0xFF00) >> 8) / 255.0f;
		b = ((colorCom & 0xFF0000) >> 16) / 255.0f;
	}
	void setTexCoord(uw texCom) 
	{
		texCoordX = (texCom & 0xFF) / 255.0f;
		texCoordY = ((texCom & 0xFF00) >> 8) / 255.0f;
	}
	float r, g, b;
	float f_x, f_y;
	float texCoordX, texCoordY;
};

struct GPU_Polygon
{
	std::vector<GPU_Vert> verts;

	void setPage(uh _page)
	{
		page = _page;
	}
	void setPalette(uh _palette)
	{
		palette = _palette;
	}

	int page = 0;
	int palette = 0;
};

struct Vec2
{
	uh x, y;
	// Vec2(uh _x, _y) ;
};

struct Vec3 
{
	union 
	{
		uh r, x;
	};
	union 
	{
		uh g, y;
	};
	union 
	{
		uh b, z;
	};
};

struct Vertex 
{
	Vec2 pos;
	Vec3 color;
	Vec2 texCoord;


	void setTexCoord(uw texCom) 
	{
		texCoord.x = (texCom & 0xFF);
		texCoord.y = ((texCom & 0xFF00) >> 8);
	}
	void setRGB(uw colorCom) 
	{
		color.r = (colorCom & 0xFF);
		color.g = ((colorCom & 0xFF00) >> 8);
		color.b = ((colorCom & 0xFF0000) >> 16);
	}
	void setXY(uw vertCom) 
	{
		uw x, y;
		x = (vertCom & 0xFFFF);
		y = (vertCom & 0xFFFF0000) >> 16;

		pos.x = x;
		pos.y = y;
	}
	void setXY(uh x, uh y)
	{
		pos.x = x;
		pos.y = y;
	}
};

struct Triangle
{
	Vertex v1, v2, v3;

	Triangle(Vertex _v1, Vertex _v2, Vertex _v3, uh _clutXloc, uh _clutYloc, uh _texPageX, ub _texPageY) 
	: v1(_v1), v2(_v2), v3(_v3), clutXloc(_clutXloc), clutYloc(_clutYloc), texPageX(_texPageX), texPageY(_texPageY) {}

	uh clutXloc;
	uh clutYloc;
	uh texPageX;
	ub texPageY;
};

class GPU
{
public:
	//GPU STAT
	// ub texPageXBase = 0;
	// ub texPageYBase = 0;
	// ub semiTranparent = 0;
	// ub texPageColor = 0;
	// ub dither = 0;
	// ub canDraw = 0;
	// ub maskBit = 0;
	// ub isDrawing = 0;
	// ub interlaceField = 0;
	// ub reverseFlag = 0;
	// ub texDisable = 0;
	// ub hRes1 = 0;
	// ub hRes2 = 0;
	// ub vRes = 0;
	// ub videoMode = 0;
	// ub colorDepth = 0;
	// ub verticalInterlace = 0;
	// ub displayEnable = 0;
	// ub interuptRequest = 0;
	// ub dmaDataRequest = 0;
	// ub readyReceiveCMD_word = 1;
	// ub readySend_cpu = 1;
	// ub readyReceiveDAM_block = 1;
	// ub DMA_dir = 0;
	// ub evenOrOdd = 0;

	union Status {
		struct {
			ub texPageXBase : 4;
			ub texPageYBase : 1;
			ub semiTranparent : 2;
			ub texPageColor : 2;
			ub dither : 1;
			ub canDraw : 1;
			ub maskBit : 1;
			ub isDrawing : 1;
			ub interlaceField : 1;
			ub reverseFlag : 1;
			ub texDisable : 1;
			ub hRes1 : 2;
			ub hRes2 : 1;
			ub vRes : 1;
			ub videoMode : 1;
			ub colorDepth : 1;
			ub verticalInterlace : 1;
			ub displayEnable : 1;
			ub interuptRequest : 1;
			ub dmaDataRequest : 1;
			ub readyReceiveCMD_word : 1;
			ub readySend_cpu : 1;
			ub readyReceiveDAM_block : 1;
			ub DMA_dir : 2;
			ub evenOrOdd : 1;
		};
		uw reg;
	};

	union Texpage {
		struct {
			ub texPageXBase : 4;
			ub texPageYBase : 1;
			ub semiTranparent : 2;
			ub texPageColor : 2;
			ub dither : 1;
			ub canDraw : 1;
			ub texDisable : 1;
			ub texXFLip : 1;
			ub texYFLip : 1;
		};
		uw _reg;
	};

	Texpage texPage;
	Status status; 

	uh drawingAreaTopLeftX;
	uh drawingAreaTopLeftY;
	uh drawingAreaBotRightX;
	uh drawingAreaBotRightY;
	uh drawingOffsetX;
	uh drawingOffsetY;

	ub texWindowMaskX;
	ub texWindowMaskY;
	ub texWindowOffsetX;
	ub texWindowOffsetY;
	
	ub setWhileDrawing;
	ub checkBeforeDrawing;

	uh displayAreaStartX; // Top Left
	uh displayAreaStartY; // Top Left
	uh displayAreaWidth;
	uh displayAreaHeight;

	enum POLY_TYPE { TRIANGLE, QUAD };
	enum COLOR_TYPE { TEXTURED, SOLID };


	uw PS_Width = 640;
	uw PS_Height = 240;

	bool isCommand = false;
	void(GPU::*commandFunc)(uw*, uw);


    Core* core;

	class CommandState 
	{
	public:
		virtual void dispatch(uw data, GPU* gpu) = 0;
		virtual uh read(GPU* gpu) { return 0; };
		int pass = 1;

		Vertex v1, v2, v3, v4;
		std::vector<Triangle> tris;
		void createTriangles()
		{
			if (v4.pos.x == 0 && v4.pos.y == 0) 
			{
				printf("Wrong create TYriangle/s functions used?\n");
				int c = getchar();;
			}
			tris.push_back(Triangle(v1, v2, v3, clutXloc, clutYloc, texPageX, texPageY));
			tris.push_back(Triangle(v2, v4, v3, clutXloc, clutYloc, texPageX, texPageY));
		}
		void createTriangle()
		{
			tris.push_back(Triangle(v1, v2, v3, clutXloc, clutYloc, texPageX, texPageY));
		}
		void setClut(uw data)
		{
			uh clutData = (data & 0xFFFF0000) >> 16;
			clutXloc = (clutData & 0x3F) * 16;
			clutYloc = (clutData & 0x7FC0) >> 6;
		}
		void setTexPage(uw data)
		{
			uh texData = (data & 0xFFFF0000) >> 16;
			texPageX = (texData & 0xF) * 64;
			texPageY = ((texData & 0x10) >> 4) * 256;
		}
		uh truncateColor24to15 (uw color)
		{
			// 8-bit 0-255
			// 5-bit 0-63
			auto truncate8_5 = [](ub value) { return (ub)(0 + (value - 0) * (63 - 0) / (255 - 0)); };

			return (((truncate8_5((color & 0xFF0000) >> 16)) & 0x7C00) << 10) | (((truncate8_5((color & 0xFF00) >> 8)) & 0x3E0) << 5) | (truncate8_5((color & 0xFF)) & 0x1F);
		}

		uh clutXloc;
		uh clutYloc;
		uh texPageX;
		ub texPageY;

		// ub clutBitSize;

		ub command;
	};

	class Idle : public CommandState 
	{
		void dispatch(uw data, GPU* gpu);
	};

	class Draw_Monocrome_Quad : public CommandState 
	{
		void dispatch(uw data, GPU* gpu);
	};

	class Draw_Shaded_Quad : public CommandState 
	{
		void dispatch(uw data, GPU* gpu);
	};

	class Draw_Monochrome_Rectangle_Fixed : public CommandState 
	{
		void dispatch(uw data, GPU* gpu);

		uh x1, y1;
		uh width, height;
	};

	class Draw_Shaded_Triangle : public CommandState 
	{
		void dispatch(uw data, GPU* gpu);
	};

	class Draw_Textured_Quad : public CommandState 
	{
		void dispatch(uw data, GPU* gpu);
	};
	
	class CopyRectangle_CPU_to_VRAM : public CommandState 
	{
		void dispatch(uw data, GPU* gpu);

		uh destX;
		uh destY;
		uh width;
		uh height;
		uw xPos;
		uw yPos;
		uw num_pixels;
		uw* tran_data;
		ub* outBuffer;
	};

	class CopyRectangle_VRAM_TO_CPU : public CommandState 
	{
		void dispatch(uw data, GPU* gpu);
		uh read(GPU* gpu);

		uh sourceX;
		uh sourceY;
		uh width;
		uh height;
		uw xPos;
		uw yPos;
		uw num_pixels;
		uw* tran_data;
		ub* outBuffer;
	};

	class Monochrome_Rectangle_1x1 : public CommandState 
	{
		void dispatch(uw data, GPU* gpu);
	};



	static CommandState* state;

	static void chageState(CommandState* new_state) 
	{
		delete state;
		state = new_state;
	}

	uint32_t GPU_READ_RESPONSE = 0;

	// enum class CommandID
	// {
	// 	IDLE = 0x00,
	// 	COPY_RECTANGLE_CPU_TO_VRAM = 0xA0,
	// 	DRAW_MONOCHROME_QUAD = 0x28,
	// 	DRAW_TEXTURED_QUAD = 0x2C,
	// 	DRAW_SHADED_TRIANGLE = 0x30,
	// 	DRAW_SHADED_QUAD = 0x38
	// };
public:
	//1mb of VRAM
	uh* VRAM;

	std::fstream outFile;

	GPU(Core* _core) {
		VRAM = new uh[1024 * 512];

		srand(0);

		uh* temp = (uh*)VRAM;

		for (int i = 0; i < 0xFFFFF / 2; i++) {
			temp[i] = 0;
		}

		outFile.open("out.txt", std::ios::out | std::ios::binary);

        core = _core;

		state = new Idle();

		status.reg = 0x1C000000;
	}
	~GPU() {}

	void gpu_mod() 
	{
		uh* temp = (uh*)VRAM;

		for (int i = 0; i < 0xFFFFF / 2; i++) {
			temp[i] = rand() % 0xFFFF;
		}
	}

	int get_horizontal_timings();

	void gpu_init();
	void gpu_clear();
	void gpu_render();

	void gpu_store_frame(void *image);

	// GLuint gpu_LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

	uw gpuReadStat();

	uh gpuRead();

	void gpu0Command(uw* command, uw size);
	void gpu0Command(uw command);
	void gpu1Command(uw command);
};

#endif