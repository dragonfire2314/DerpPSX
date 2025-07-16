#include <gpu.hh>

#include <logging.hh>
#include <core.hh>

GPU::CommandState* GPU::state;

int GPU::get_horizontal_timings()
{
	if (status.hRes2 == 1) { //368
		return 368;
	}
	else {
		switch (status.hRes1)
		{
		case 0: return 256;
		case 1: return 320;
		case 2: return 512;
		case 3: return 640;
		default:
			printf("\033[1;31m%s\033[0m\n", "Bad resolution.");
            return -1;
			break;
		}
	}
}

void GPU::gpu_init()
{
	// SolidShader = gpu_LoadShaders("vertex.v", "pixel.p");
	// TextureShader = gpu_LoadShaders("TextureVertex.v", "TexturePixel.p");
}

void GPU::gpu_clear()
{
	// objects_index = 0;
}

void GPU::gpu_render()
{
	// for (int i = 0; i < objects_index; i++) 
	// {
	// 	objects[i].setUp();
	// 	objects[i].render(TextureShader, SolidShader);
	// }
}

void GPU::gpu_store_frame(void *image)
{
	//Income image is made with PS_Width and PS_Height at 16 bits per pixel

	if (status.displayEnable == 1) //Display off
	{
		printf("display disabbled\n");
		int c = getchar();;
		return;
	}

	// if (drawingAreaBotRightX - drawingAreaTopLeftX + 1 != PS_Width) 
	// {
	// 	printf("drawingAreaBotRightX: %i, drawingAreaTopLeftX: %i, PS_Width: %i\n", drawingAreaBotRightX, drawingAreaTopLeftX, PS_Width);
	// 	printf("The day I feared has come, the GPU is using a drawing window that is differnt than the resolution X\n");
	// 	int c = getchar();;
	// }

	// if (drawingAreaBotRightY - drawingAreaTopLeftY + 1 != PS_Height) 
	// {
	// 	printf("drawingAreaBotRightY: %i, drawingAreaTopLeftY: %i, PS_Height: %i\n", drawingAreaBotRightY, drawingAreaTopLeftY, PS_Height);
	// 	printf("The day I feared has come, the GPU is using a drawing window that is differnt than the resolution Y\n");
	// 	int c = getchar();;
	// }

	//Copy image to VRAM using drawingAreaBotRightX, Y and drawingAreaBotRightX, Y
	uh* src_ptr = (uh*)(image);
	// uh mask_and = 0;
	// uh mask_or = 0;
	uh destY = drawingAreaTopLeftX;
	uh destX = drawingAreaTopLeftY;
	uh limitX = drawingAreaBotRightX - drawingAreaTopLeftX + 1;
	uh limitY = drawingAreaBotRightY - drawingAreaTopLeftY + 1;

	// printf("destX: %i, destY: %i, limitX: %i, limitY: %i\n", destY, destX, limitX, limitY);

	for (uw row = 0; row < limitY;)
	{
		uh* dst_row_ptr = &(VRAM)[((destY + row++) % 512) * 1024];
		for (uw col = 0; col < limitX;)
		{
			uh* pixel_ptr = &dst_row_ptr[(destX + col++) % 1024];
			// if (((*pixel_ptr) & mask_and) == 0)
			// 	*pixel_ptr = *(src_ptr++) | mask_or;
			*pixel_ptr = *(src_ptr++);
		}
	}
}

void GPU::Idle::dispatch(uw data, GPU* gpu)
{
	uw command_byte = ((data) & 0xFF000000) >> 24;

	CommandState* new_state = nullptr;
	int c;

	switch (command_byte)
	{
	case 0x00: //NOP
		break;
	case 0x01: //Cache clear
		break;
	case 0x28: //Monochrome four-point opaque polygon
		// printf("[GPU] 0x28 Monochrome four-point opaque polygon\n");
		// commandState = CommandState::DRAW_MONOCHROME_QUAD;
		new_state = new GPU::Draw_Monocrome_Quad();
		break;
	case 0x2C:
		// printf("[GPU] 0x2C Textured four-point opaque polygon\n");
		// isCommand = true;
		new_state = new GPU::Draw_Textured_Quad();
		break;
	case 0x30:
		// printf("[GPU] 0x30 Shaded three-point opaque polygon\n");
		// commandState = CommandState::DRAW_SHADED_TRIANGLE;
		new_state = new GPU::Draw_Shaded_Triangle();
		break;
	case 0x38:
		// printf("[GPU] 0x38 Shaded four-point opaque polygon\n");
		// commandState = CommandState::DRAW_SHADED_QUAD;
		new_state = new GPU::Draw_Shaded_Quad();
		break;
	case 0x78:
		// printf("[GPU] 0x78 Monochrome Rectangle (16x16) (opaque)\n");
		// commandState = CommandState::DRAW_SHADED_QUAD;
		new_state = new GPU::Draw_Monochrome_Rectangle_Fixed();
		break;
	case 0xa0:
		printf("[GPU] 0xA0 Copy Rectangle\n");
		// commandState = CommandState::COPY_RECTANGLE_CPU_TO_VRAM;
		new_state = new GPU::CopyRectangle_CPU_to_VRAM();
		break;
	case 0xc0:
		// printf("[GPU] 0xC0 Copy Rectangle\n");
		// commandState = CommandState::COPY_RECTANGLE_CPU_TO_VRAM;
		new_state = new GPU::CopyRectangle_VRAM_TO_CPU();
		break;
	case 0xe1:
		gpu->texPage._reg = data;
		gpu->status.texPageXBase = gpu->texPage.texPageXBase;
		gpu->status.texPageYBase = gpu->texPage.texPageYBase;
		// if (gpu->status.semiTranparent != gpu->texPage.semiTranparent) 
		// {
		// 	printf("semiTranparent\n");
		// 	int c = getchar();;
		// }
		gpu->status.semiTranparent = gpu->texPage.semiTranparent;
		// if (gpu->status.texPageColor != gpu->texPage.texPageColor) 
		// {
		// 	printf("HHMMMM\n");
		// 	int c = getchar();;
		// }
		gpu->status.texPageColor = gpu->texPage.texPageColor;
		gpu->status.dither = gpu->texPage.dither;
		gpu->status.canDraw = gpu->texPage.canDraw;
		gpu->status.texDisable = gpu->texPage.texDisable;
		break;
	case 0xe2:
		gpu->texWindowMaskX = (data & 0x1F) >> 0;
		gpu->texWindowMaskY = (data & 0x3E0) >> 5;
		gpu->texWindowOffsetX = (data & 0x7C00) >> 10;
		gpu->texWindowOffsetY = (data & 0xF8000) >> 15;
		break;
	case 0xe3: //Set Drawing Area top left (X1,Y1)
		gpu->drawingAreaTopLeftX = (data & 0x3FF);
		gpu->drawingAreaTopLeftY = (data & 0x7FC00) >> 10;
		break;
	case 0xe4:
		gpu->drawingAreaBotRightX = (data & 0x3FF);
		gpu->drawingAreaBotRightY = (data & 0x7FC00) >> 10;
		break;
	case 0xe5:
		gpu->drawingOffsetX = (data & 0x7FF);
		gpu->drawingOffsetY = (data & 0x3FF800) >> 11;
		break;
	case 0xe6:
		gpu->setWhileDrawing = data & 0x1;
		gpu->checkBeforeDrawing = (data & 0x2) >> 1;
		break;
	case 0x68:
		// printf("[GPU] 0x68 Monochrome Rectangle (1x1)\n");
		new_state = new GPU::Monochrome_Rectangle_1x1();
		break;
	default:
		printf("[GPU COMMAND] Command not found:%x\n", (data));
		int c = getchar();;
		// for (uw i = 0; i < size; i++) 
		// {
		// 	printf("%x ", (command[i]));
		// }
		// printf("\n");
		return;
		break;
	}

	if (new_state) 
	{
		chageState(new_state);
		state->dispatch(data, gpu);
	}
}

void GPU::Draw_Monochrome_Rectangle_Fixed::dispatch(uw data, GPU* gpu)
{
	if (pass == 1)
	{
		command = ((data) & 0xFF000000) >> 24;

		v1.setRGB(data);
		v2.setRGB(data);
		v3.setRGB(data);
		v4.setRGB(data);
	}
	if (pass == 2) 
	{
		x1 = (data & 0xFF);
		y1 = (data & 0xFF00) >> 16;

		v1.setXY(x1, y1);
		v2.setXY(x1 + 16, y1);
		v3.setXY(x1, y1 + 16);
		v4.setXY(x1 + 16, y1 + 16);

		createTriangles();
		//Send to renderer
		gpu->core->getRenderer()->addPolygon(tris);

		GPU::chageState(new Idle());
	}

	pass++;
}

void GPU::Draw_Monocrome_Quad::dispatch(uw data, GPU* gpu)
{
	if (pass == 1)
	{
		v1.setRGB(data);
		v2.setRGB(data);
		v3.setRGB(data);
		v4.setRGB(data);
	}
	if (pass == 2) 
	{
		v1.setXY(data);
	}
	if (pass == 3) 
	{
		v2.setXY(data);
	}
	if (pass == 4) 
	{
		v3.setXY(data);
	}
	if (pass == 5) 
	{
		v4.setXY(data);

		createTriangles();
		//Send to renderer
		gpu->core->getRenderer()->addPolygon(tris);

		GPU::chageState(new Idle());
	}

	pass++;
}

void GPU::Draw_Shaded_Quad::dispatch(uw data, GPU* gpu)
{
	if (pass == 1)
	{
		v1.setRGB(data);
	}
	if (pass == 2) 
	{
		v1.setXY(data);
	}
	if (pass == 3) 
	{
		v2.setRGB(data);
	}
	if (pass == 4) 
	{
		v2.setXY(data);
	}
	if (pass == 5) 
	{
		v3.setRGB(data);
	}
	if (pass == 6) 
	{
		v3.setXY(data);
	}
	if (pass == 7) 
	{
		v4.setRGB(data);
	}
	if (pass == 8) 
	{
		v4.setXY(data);

		createTriangles();
		//Send to renderer
		gpu->core->getRenderer()->addPolygon(tris);

		GPU::chageState(new Idle());
	}

	pass++;
}

void GPU::Draw_Shaded_Triangle::dispatch(uw data, GPU* gpu)
{
	if (pass == 1)
	{
		v1.setRGB(data);
	}
	if (pass == 2) 
	{
		v1.setXY(data);
	}
	if (pass == 3) 
	{
		v2.setRGB(data);
	}
	if (pass == 4) 
	{
		v2.setXY(data);
	}
	if (pass == 5) 
	{
		v3.setRGB(data);
	}
	if (pass == 6) 
	{
		v3.setXY(data);

		createTriangle();
		//Send to renderer
		gpu->core->getRenderer()->addPolygon(tris);

		GPU::chageState(new Idle());
	}

	pass++;
}

void GPU::Draw_Textured_Quad::dispatch(uw data, GPU* gpu)
{
	if (pass == 1)
	{
		//None
		v1.setRGB(0x00000000);
		v2.setRGB(0x00000000);
		v3.setRGB(0x00000000);
		v4.setRGB(0x00000000);
	}
	else if (pass == 2)
	{
		//v1
		v1.setXY(data);
	}
	else if (pass == 3)
	{
		// polygon.setPalette((data & 0xFFFF0000) >> 16);
		setClut(data);
		v1.setTexCoord(data);
	}
	else if (pass == 4)
	{
		//v1
		v2.setXY(data);
	}
	else if (pass == 5)
	{
		// polygon.setPage((data & 0xFFFF0000) >> 16);
		setTexPage(data);
		v2.setTexCoord(data);
	}
	else if (pass == 6)
	{
		//v1
		v3.setXY(data);
	}
	else if (pass == 7)
	{
		v3.setTexCoord(data);
	}
	else if (pass == 8)
	{
		//v4
		v4.setXY(data);
	}
	else if (pass == 9)
	{
		v4.setTexCoord(data);

		createTriangles();
		//Send to renderer
		gpu->core->getRenderer()->addPolygon(tris);

		GPU::chageState(new Idle());

		//Make texture
		// objects[objects_index].textureData = new ub[256 * 256 * 4];
		//Get clut address
		// uh clutXloc = (palette & 0x3F) * 16;
		// uh clutYloc = (palette & 0x7FC0) >> 6;

		// uh texPageX = (page & 0xF) * 64;
		// uh texPageY = (page & 0x10) >> 4;

		// uw countX = 0;
		// uw countY = 0;

		// uh* clut = (VRAM + (clutYloc * 1024) + (clutXloc));

		//Copy 256 * 256 VRAM Block
		//memset(objects[objects_index].textureData, 255, sizeof(ub) * 256 * 256 * 3);
		// const uh mask_and = 0;
		// const uh mask_or = 0;

		// uw src_x = texPageX;
		// uw src_y = texPageY;
		// uw dst_x = 0;
		// uw dst_y = 0;
		// uw width = 64;
		// uw height = 64;

		// for (uw y = 0; y < 256; y++) 
		// {
		// 	for (uw x = 0; x < 64; x++)
		// 	{
		// 		//Add texpageY to y
		// 		uh pixel = VRAM[(y * 1024) + x + texPageX];
		// 		ub p1 = (pixel & 0xF);
		// 		ub p2 = (pixel & 0xF0) >> 4;
		// 		ub p3 = (pixel & 0xF00) >> 8;
		// 		ub p4 = (pixel & 0xF000) >> 12;

		// 		uw index = ((y * 1024) + (x * 16));

		// 		objects[objects_index].textureData[index + 3] = ((clut[p1] & 0x1F)) * 8;
		// 		objects[objects_index].textureData[index + 2] = ((clut[p1] & 0x3E0) >> 5) * 8;
		// 		objects[objects_index].textureData[index + 1] = ((clut[p1] & 0x7C00) >> 10) * 8;
		// 		if (clut[p1] == 0x0) objects[objects_index].textureData[index] = 0; //Transparent
		// 		else objects[objects_index].textureData[index] = 255; //Opeque
				
														 
		// 		objects[objects_index].textureData[index + 7] = ((clut[p2] & 0x1F)) * 8;
		// 		objects[objects_index].textureData[index + 6] = ((clut[p2] & 0x3E0) >> 5) * 8;
		// 		objects[objects_index].textureData[index + 5] = ((clut[p2] & 0x7C00) >> 10) * 8;
		// 		if (clut[p2] == 0x0) objects[objects_index].textureData[index + 4] = 0; //Transparent
		// 		else objects[objects_index].textureData[index + 4] = 255; //Opeque
														 
		// 		objects[objects_index].textureData[index + 11] = ((clut[p3] & 0x1F)) * 8;
		// 		objects[objects_index].textureData[index + 10] = ((clut[p3] & 0x3E0) >> 5) * 8;
		// 		objects[objects_index].textureData[index + 9] = ((clut[p3] & 0x7C00) >> 10) * 8;
		// 		if (clut[p3] == 0x0) objects[objects_index].textureData[index + 8] = 0; //Transparent
		// 		else objects[objects_index].textureData[index + 8] = 255; //Opeque
														 
		// 		objects[objects_index].textureData[index + 15] = ((clut[p4] & 0x1F)) * 8;
		// 		objects[objects_index].textureData[index + 14] = ((clut[p4] & 0x3E0) >> 5) * 8;
		// 		objects[objects_index].textureData[index + 13] = ((clut[p4] & 0x7C00) >> 10) * 8;
		// 		if (clut[p4] == 0x0) objects[objects_index].textureData[index + 12] = 0; //Transparent
		// 		else objects[objects_index].textureData[index + 12] = 255; //Opeque
		// 	}
		// }
	}

	pass++;
}

void GPU::CopyRectangle_CPU_to_VRAM::dispatch(uw data, GPU* gpu)
{
	// LOG::fatal(LOG::GPU, "CopyRectangle_CPU_to_VRAM called, im not sure this code works");
	
	if (pass == 2) 
	{
		destX = (data & 0xFFFF);
		destY = (data & 0xFFFF0000) >> 16;
	}
	if (pass == 3) 
	{
		width = (data & 0xFFFF);
		height = (data & 0xFFFF0000) >> 16;

		num_pixels = width * height;
		tran_data = new uw[num_pixels];
		outBuffer = new ub[num_pixels * 2];
	}
	if (pass >= 4) 
	{
		tran_data[pass - 4] = data;

		//Exit condition
		if (((width * height / 2) + 1) == pass-2) {

			uh* src_ptr = (uh*)(tran_data);
			uh mask_and = 0;
			uh mask_or = 0;

			for (uw row = 0; row < height;)
			{
				uh* dst_row_ptr = &(gpu->VRAM)[((destY + row++) % 512) * 1024];
				for (uw col = 0; col < width;)
				{
					// TODO: Handle unaligned reads...
					uh* pixel_ptr = &dst_row_ptr[(destX + col++) % 1024];
					if (((*pixel_ptr) & mask_and) == 0)
						*pixel_ptr = *(src_ptr++) | mask_or;
				}
			}

			delete tran_data;
			delete outBuffer;

			GPU::chageState(new Idle());
		}
	}

	// tran_data[count - 2] = command[2];

	

	pass++;

}

void GPU::CopyRectangle_VRAM_TO_CPU::dispatch(uw data, GPU* gpu)
{
	if (pass == 2) 
	{
		sourceX = (data & 0xFFFF);
		sourceY = (data & 0xFFFF0000) >> 16;
	}
	if (pass == 3) 
	{
		width = (data & 0xFFFF);
		height = (data & 0xFFFF0000) >> 16;

		num_pixels = width * height;
	}

	pass++;

}

uh GPU::CopyRectangle_VRAM_TO_CPU::read(GPU* gpu)
{
	//Exit condition
	if (((width * height / 2) + 1) == pass-2) {
		GPU::chageState(new Idle());
	}

	pass++;

	return gpu->VRAM[1024 * sourceY + sourceX];
}

void GPU::Monochrome_Rectangle_1x1::dispatch(uw data, GPU* gpu)
{
	static uw color;
	if (pass == 1)
	{
		color = data;
		v1.setRGB(data);
	}
	if (pass == 2) 
	{
		v1.setXY(data);
		
		//Place pixel in vram
		// gpu->VRAM[v1.pos.y * 1024 + v1.pos.x] = (v1.color.r) | (v1.color.b);
		gpu->VRAM[v1.pos.y * 1024 + v1.pos.x] = (((v1.color.b & 0xFF) / 4) << 10) | (((v1.color.g & 0xFF) / 4) << 5) | ((v1.color.r & 0xFF) / 4);
		// gpu->VRAM[v1.pos.y * 1024 + v1.pos.x] = truncateColor24to15(color);

		GPU::chageState(new Idle());
	}	

	pass++;
}

uw GPU::gpuReadStat()
{
	return status.reg;
}

void GPU::gpu0Command(uw command)
{
	// fprintf(stderr, "[GPU0] command: %x\n", command);

	state->dispatch(command, this);
}

void GPU::gpu0Command(uw* command, uw size)
{
	// fprintf(stderr, "[GPU0] command: %x, size: %x\n", *command, size);

	GPU::chageState(new Idle());

	for (uw i = 0; i < size; i++)
		state->dispatch(command[i], this);
}

void GPU::gpu1Command(uw command)
{
	// std::cout << "[GPU1] Command: " << std::hex << command << std::endl;

	switch ((command & 0xFF000000) >> 24)
	{
	case 0x00:
		break;
	case 0x01: //Resets GPU
		break;
	case 0x02: //Acknowledge gpu intrupt
		status.interuptRequest = 0;
		break;
	case 0x03: //Display enable
		status.displayEnable = (command & 0x1);
		break;
	case 0x05:
		displayAreaStartX = (command & 0x3FF);
		displayAreaStartY = (command & 0x7FE00) >> 10;
		break;
	case 0x06:
		break;
	case 0x07:
		break;
	case 0x08:
		status.hRes1 = (command & 0x3);
		status.vRes = (command & 0x4) >> 2;
		status.videoMode = (command & 0x8) >> 3;
		status.colorDepth = (command & 0x10) >> 4;
		status.verticalInterlace = (command & 0x20) >> 5;
		status.hRes2 = (command & 0x40) >> 6;
		status.reverseFlag = (command & 0x80) >> 7;
		break;
	case 0x04:
		status.DMA_dir = command & 0x3;
		break;
	case 0x10:
		switch(command & 0xFFFFFF)
		{
		case 0x07:
			GPU_READ_RESPONSE = 0;
			break;
		default:
			GPU_READ_RESPONSE = 0;
			fprintf(stderr, "GP1 command 10 sub command not found: %x", command & 0xFFFFFF);
			int c = getchar();
			break;
		}
		break;
	default:
		fprintf(stderr, "GP1 command not found: %x", (command & 0xFF000000) >> 24);
		int c = getchar();
	}

	//Set screen resolution
	if (status.hRes2 == 0) {
		if (status.hRes1 == 0) PS_Width = 256;
		if (status.hRes1 == 1) PS_Width = 320;
		if (status.hRes1 == 2) PS_Width = 512;
		if (status.hRes1 == 3) PS_Width = 640;
	}
	else PS_Width = 368;
	if (status.vRes == 0) PS_Height = 240;
	else {
		if (status.verticalInterlace == 1) PS_Height = 480;
	}
}

uh GPU::gpuRead()
{
	// return 0;
	return state->read(this);
}
