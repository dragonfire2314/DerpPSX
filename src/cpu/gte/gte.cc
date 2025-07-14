#include <cpu/gte/gte.hh>

void GTE::execCommand(uw command)
{
    flag.reg = 0;
    ub opcode = command & 0x3F;
    sf = (command & 0x80000) >> 19;
    lm = (command & 0x400) >> 10;

    // static int count = 0;
    // if (count == 1308) int c = getchar();

    // fprintf(stderr, "count:%i\n", count);
    // fprintf(stderr, "cmd:%i\n", opcode);

    switch(opcode)
    {
    case 0x06:
        NCLIP(command);
        break;
    case 0x13:
        NCDS(command);
        break;
    case 0x2d:
        AVSZ3();
        break;
    case 0x30:
        RTPT(command);
        break;
    default:
        printf("GTE command not found: %x, opcode: %x\n", command, opcode);
        // int c = getchar();;
        // exit(0);
        break;
    }

    // count++;

    // if (count == 2500) 
    // {
    //     int c = getchar();
    // }

    // fprintf(stderr, "flag:%x\n", flag.reg);

    // printf("GTE command: %x, opcode: %x\n", command, opcode);
    // int c = getchar();;
}

uw GTE::read(ub reg)
{
    union highLow
    {
        struct
        {
            ub b_0;
            ub b_1;
            ub b_2;
            ub b_3;
        };
        struct
        {
            uh low;
            uh high;
        };
        struct 
        {
            ub blue : 5;
            ub green : 5;
            ub red : 5;
            uw unused : 17;
        };
        uw reg;
    };

    highLow data;

    switch(reg)
    {
    //vectors array
    case 0:
        data.low = vectors[0].x;
        data.high = vectors[0].y;
        break;
    case 1:
        data.reg = vectors[0].z;
        break;
    case 2:
        data.low = vectors[1].x;
        data.high = vectors[1].y;
        break;
    case 3:
        data.reg = vectors[1].z;
        break;
    case 4:
        data.low = vectors[2].x;
        data.high = vectors[2].y;
        break;
    case 5:
        data.reg = vectors[1].z;
        break;
    case 6:
        data.b_0 = r;
        data.b_1 = g;
        data.b_2 = b;
        data.b_3 = c;
        break;
    case 8:
        data.reg = IR0;
        break;
    case 7:
        data.low = OTZ;
        break;
    //IR_vetor
    case 9:
        data.reg = IR_ARR[1];
        break;
    case 10:
        data.reg = IR_ARR[2];
        break;
    case 11:
        data.reg = IR_ARR[3];
        break;
    //screenCoords
    case 12:
        data.low = ScreenXY[0].x;
        data.high = ScreenXY[0].y;
        break;
    case 13:
        data.low = ScreenXY[1].x;
        data.high = ScreenXY[1].y;
        break;
    case 14:
        data.low = ScreenXY[2].x;
        data.high = ScreenXY[2].y;
        break;
    case 15:
        data.low = ScreenXY[2].x;
        data.high = ScreenXY[2].y;
        break;
    case 16:
        data.reg = ScreenZ[0];
        break;
    case 17:
        data.reg = ScreenZ[1];
        break;
    case 18:
        data.reg = ScreenZ[2];
        break;
    case 19:
        data.reg = ScreenZ[3];
        break;
    case 20:
        data.b_0 = ColorFIFO[0].x;
        data.b_1 = ColorFIFO[0].y;
        data.b_2 = ColorFIFO[0].z;
        break;
    case 21:
        data.b_0 = ColorFIFO[1].x;
        data.b_1 = ColorFIFO[1].y;
        data.b_2 = ColorFIFO[1].z;
        break;
    case 22:
        data.b_0 = ColorFIFO[2].x;
        data.b_1 = ColorFIFO[2].y;
        data.b_2 = ColorFIFO[2].z;
        break;
    case 23:
        //Unused
        break;
    case 24:
        data.reg = MAC0;
        break;
    case 25:
        data.reg = MAC_ARR[0];
        break;
    case 26:
        data.reg = MAC_ARR[1];
        break;
    case 27:
        data.reg = MAC_ARR[2];
        break;
    case 28: 
        // data.red = IR1 / 0x80;
        // data.green = IR2 / 0x80;
        // data.blue = IR3 / 0x80;
        fprintf(stderr, "read IRGB\n");
        // getchar();
        break;
    case 30:
        data.reg = LZCS;
        break;

    case 31:
        data.reg = LZCR;
        break;
    //rotationMatrix
    case 32:
        data.low = rotationMatrix.M11;
        data.high = rotationMatrix.M12;
        break;
    case 33:
        data.low = rotationMatrix.M13;
        data.high = rotationMatrix.M21;
        break;
    case 34:
        data.low = rotationMatrix.M22;
        data.high = rotationMatrix.M23;
        break;
    case 35:
        data.low = rotationMatrix.M31;
        data.high = rotationMatrix.M32;
        break;
    case 36:
        data.low = rotationMatrix.M33;
        break;
    //translationVector
    case 37:
        data.reg = translationVector.x;
        break;
    case 38:
        data.reg = translationVector.y;
        break;
    case 39:
        data.reg = translationVector.z;
        break;
    //lightSourceMatrix
    case 40:
        data.low = lightSourceMatrix.M11;
        data.high = lightSourceMatrix.M12;
        break;
    case 41:
        data.low = lightSourceMatrix.M13;
        data.high = lightSourceMatrix.M21;
        break;
    case 42:
        data.low = lightSourceMatrix.M22;
        data.high = lightSourceMatrix.M23;
        break;
    case 43:
        data.low = lightSourceMatrix.M31;
        data.high = lightSourceMatrix.M32;
        break;
    case 44:
        data.low = lightSourceMatrix.M33;
        break;
    //backgroundColor
    case 45:
        data.reg = backgroundColor.x;
        break;
    case 46:
        data.reg = backgroundColor.y;
        break;
    case 47:
        data.reg = backgroundColor.z;
        break;
    //lightColorSourceMatrix
    case 48:
        data.low = lightColorSourceMatrix.M11;
        data.high = lightColorSourceMatrix.M12;
        break;
    case 49:
        data.low = lightColorSourceMatrix.M13;
        data.high = lightColorSourceMatrix.M21;
        break;
    case 50:
        data.low = lightColorSourceMatrix.M22;
        data.high = lightColorSourceMatrix.M23;
        break;
    case 51:
        data.low = lightColorSourceMatrix.M31;
        data.high = lightColorSourceMatrix.M32;
        break;
    case 52:
        data.low = lightColorSourceMatrix.M33;
        break;
    //farColor
    case 53:
        data.reg = farColor.x;
        break;
    case 54:
        data.reg = farColor.y;
        break;
    case 55:
        data.reg = farColor.z;
        break;


    case 56:
        data.reg = screenOffest.x;
        break;
    case 57:
        data.reg = screenOffest.y;
        break;
    case 58:
        data.reg = H_projectionPlaneDistance;
        break;
    case 59:
        data.reg = DQA;
        break;
    case 60:
        data.reg = DQB;
        break;

    case 61:
        data.reg = ZSF3;
        break;
    case 62:
        data.reg = ZSF4;
        break;
    case 63:
        data.reg = flag.reg;
        break;
    default:
        printf("GTE read, reg not found: %i\n", reg);
        // int c = getchar();
        break;
    }

    // printf("GTE read: %i, data: %x\n", reg, data.reg);
        // int c = getchar();


    return data.reg;
}

void GTE::write(ub reg, uw _data)
{
    

    union highLow
    {
        struct
        {
            ub b_0;
            ub b_1;
            ub b_2;
            ub b_3;
        };
        struct
        {
            uh low;
            uh high;
        };
        struct 
        {
            ub blue : 5;
            ub green : 5;
            ub red : 5;
            uw unused : 17;
        };
        uw reg;
    };

    highLow data;
    data.reg = _data;

    switch(reg)
    {
    //vectors array
    case 0:
        vectors[0].x = data.low;
        vectors[0].y = data.high;
        break;
    case 1:
        vectors[0].z = data.reg;
        break;
    case 2:
        vectors[1].x = data.low;
        vectors[1].y = data.high;
        break;
    case 3:
        vectors[1].z = data.reg;
        break;
    case 4:
        vectors[2].x = data.low;
        vectors[2].y = data.high;
        break;
    case 5:
        vectors[2].z = data.reg;
        break;
    case 6:
        r = data.b_0;
        g = data.b_1;
        b = data.b_2;
        c = data.b_3;
        break;
    case 7:
        break;
    case 8:
        IR0 = data.reg;
        break;
    //IR_vetor
    case 9:
        IR_ARR[1] = data.reg;
        break;
    case 10:
        IR_ARR[2] = data.reg;
        break;
    case 11:
        IR_ARR[3] = data.reg;
        break;
    //screenCoords
    case 12:
        ScreenXY[0].x = data.low;
        ScreenXY[0].y = data.high;
        break;
    case 13:
        ScreenXY[1].x = data.low;
        ScreenXY[1].y = data.high;
        break;
    case 14:
        ScreenXY[2].x = data.low;
        ScreenXY[2].y = data.high;
        break;
    case 15:
        ScreenXY[0] = ScreenXY[1];
        ScreenXY[1] = ScreenXY[2];
        ScreenXY[2].x = data.low;
        ScreenXY[2].y = data.high;
        break;
    case 16:
        ScreenZ[0] = data.reg;
        break;
    case 17:
        ScreenZ[1] = data.reg;
        break;
    case 18:
        ScreenZ[2] = data.reg;
        break;
    case 19:
        ScreenZ[3] = data.reg;
        break;
    case 20:
        ColorFIFO[0].x = data.b_0;
        ColorFIFO[0].y = data.b_1;
        ColorFIFO[0].z = data.b_2;
        break;
    case 21:
        ColorFIFO[1].x = data.b_0;
        ColorFIFO[1].y = data.b_1;
        ColorFIFO[1].z = data.b_2;
        break;
    case 22:
        ColorFIFO[2].x = data.b_0;
        ColorFIFO[2].y = data.b_1;
        ColorFIFO[2].z = data.b_2;
        break;
    case 23:
        //Unused
        break;
    case 24:
        MAC0 = data.reg;
        break;
    case 25:
        MAC_ARR[1] = data.reg;
        break;
    case 26:
        MAC_ARR[2] = data.reg;
        break;
    case 27:
        MAC_ARR[3] = data.reg;
        break;
    case 28:
        IR_ARR[1] = data.red * 0x80;
        IR_ARR[2] = data.green * 0x80;
        IR_ARR[3] = data.blue * 0x80;
        break;
    case 29:
        break;
    case 30:
        LZCS = data.reg;
        break;
    case 31:
        break;
    //rotationMatrix
    case 32:
        rotationMatrix.M11 = data.low;
        rotationMatrix.M12 = data.high;
        // fprintf(stderr, "rotationMatrix.M11: %x\n", data.low);
        // fprintf(stderr, "rotationMatrix.M12: %x\n", data.high);
        break;
    case 33:
        rotationMatrix.M13 = data.low;
        rotationMatrix.M21 = data.high;
        // fprintf(stderr, "rotationMatrix.M13: %x\n", data.low);
        // fprintf(stderr, "rotationMatrix.M21: %x\n", data.high);
        break;
    case 34:
        rotationMatrix.M22 = data.low;
        rotationMatrix.M23 = data.high;
        break;
    case 35:
        rotationMatrix.M31 = data.low;
        rotationMatrix.M32 = data.high;
        break;
    case 36:
        rotationMatrix.M33 = data.reg;
        break;
    //translationVector
    case 37:
        translationVector.x = data.reg;
        break;
    case 38:
        translationVector.y = data.reg;
        break;
    case 39:
        translationVector.z = data.reg;
        break;
    //lightSourceMatrix
    case 40:
        lightSourceMatrix.M11 = data.low;
        lightSourceMatrix.M12 = data.high;
        break;
    case 41:
        lightSourceMatrix.M13 = data.low;
        lightSourceMatrix.M21 = data.high;
        break;
    case 42:
        lightSourceMatrix.M22 = data.low;
        lightSourceMatrix.M23 = data.high;
        break;
    case 43:
        lightSourceMatrix.M31 = data.low;
        lightSourceMatrix.M32 = data.high;
        break;
    case 44:
        lightSourceMatrix.M33 = data.reg;
        break;
    //backgroundColor
    case 45:
        backgroundColor.x = data.reg;
        break;
    case 46:
        backgroundColor.y = data.reg;
        break;
    case 47:
        backgroundColor.z = data.reg;
        break;
    //lightColorSourceMatrix
    case 48:
        lightColorSourceMatrix.M11 = data.low;
        lightColorSourceMatrix.M12 = data.high;
        break;
    case 49:
        lightColorSourceMatrix.M13 = data.low;
        lightColorSourceMatrix.M21 = data.high;
        break;
    case 50:
        lightColorSourceMatrix.M22 = data.low;
        lightColorSourceMatrix.M23 = data.high;
        break;
    case 51:
        lightColorSourceMatrix.M31 = data.low;
        lightColorSourceMatrix.M32 = data.high;
        break;
    case 52:
        lightColorSourceMatrix.M33 = data.reg;
        break;
    //farColor
    case 53:
        farColor.x = data.reg;
        break;
    case 54:
        farColor.y = data.reg;
        break;
    case 55:
        farColor.z = data.reg;
        break;


    case 56:
        screenOffest.x = data.reg;
        break;
    case 57:
        screenOffest.y = data.reg;
        break;
    case 58:
        H_projectionPlaneDistance = data.reg;
        break;
    case 59:
        DQA = data.reg;
        break;
    case 60:
        DQB = data.reg;
        break;

    case 61:
        ZSF3 = data.reg;
        break;
    case 62:
        ZSF4 = data.reg;
        break;
    case 63:
        break;
    default:
        printf("GTE write, reg not found: %i, data: %x\n", reg, data.reg);
        // int c = getchar();;
        break;
    }

    // printf("GTE write: %i, data: %x, data2: %x\n", reg, data.reg, _data);
}