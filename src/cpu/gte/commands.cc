#include <cpu/gte/gte.hh>

uint64_t extend_sign(uint64_t n) {
    const int bit_size = 44;
    static_assert(bit_size > 0 && bit_size < 63, "bit_size out of range");

    uint64_t mask = ((1LL << (bit_size - 1)) - 1);
    bool sign = (n & (1LL << (bit_size - 1))) != 0;

    uint64_t val = n & mask;
    if (sign) val |= ~mask;
    return val;
}

void GTE::MVMVA(uw command)
{
    // printf("command: %x\n", command);

    int c = getchar();
}

void GTE::NCLIP(uw command)
{
    setMAC0((int64_t)ScreenXY[0].x * (ScreenXY[1].y - ScreenXY[2].y) + ScreenXY[1].x * (ScreenXY[2].y - ScreenXY[0].y) + ScreenXY[2].x * (ScreenXY[0].y - ScreenXY[1].y));
    // fprintf(stderr, "mac0:%x\n", MAC0);
}

void GTE::NCDS(uw command)
{
    int64_t res1 = setMAC(1, setMAC(1, setMAC(1, (int64_t)lightSourceMatrix.M11*vectors[0].x) + lightSourceMatrix.M12*vectors[0].y) + lightSourceMatrix.M13*vectors[0].z);
    int64_t res2 = setMAC(2, setMAC(2, setMAC(2, (int64_t)lightSourceMatrix.M21*vectors[0].x) + lightSourceMatrix.M22*vectors[0].y) + lightSourceMatrix.M23*vectors[0].z);
    int64_t res3 = setMAC(3, setMAC(3, setMAC(3, (int64_t)lightSourceMatrix.M31*vectors[0].x) + lightSourceMatrix.M32*vectors[0].y) + lightSourceMatrix.M33*vectors[0].z);

    MAC_ARR[1] = res1 >> 12;
    MAC_ARR[2] = res2 >> 12;
    MAC_ARR[3] = res3 >> 12;

    setIR(1, MAC_ARR[1], lm);
    setIR(2, MAC_ARR[2], lm);
    setIR(3, MAC_ARR[3], lm);

    // fprintf(stderr, "\nncds 1\n\n");
    // fprintf(stderr, "mac1: %lx\n", MAC_ARR[1]);
    // fprintf(stderr, "mac2: %lx\n", MAC_ARR[2]);
    // fprintf(stderr, "mac3: %lx\n", MAC_ARR[3]);

    // fprintf(stderr, "ir1: %x\n", IR_ARR[1]);
    // fprintf(stderr, "ir2: %x\n", IR_ARR[2]);
    // fprintf(stderr, "ir3: %x\n", IR_ARR[3]);

    res1 = setMAC(1, setMAC(1, setMAC(1, ((int64_t)backgroundColor.x << 12) + lightColorSourceMatrix.M11*IR_ARR[1]) + lightColorSourceMatrix.M12*IR_ARR[2]) + lightColorSourceMatrix.M13*IR_ARR[3]);
    res2 = setMAC(2, setMAC(2, setMAC(2, ((int64_t)backgroundColor.y << 12) + lightColorSourceMatrix.M21*IR_ARR[1]) + lightColorSourceMatrix.M22*IR_ARR[2]) + lightColorSourceMatrix.M23*IR_ARR[3]);
    res3 = setMAC(3, setMAC(3, setMAC(3, ((int64_t)backgroundColor.z << 12) + lightColorSourceMatrix.M31*IR_ARR[1]) + lightColorSourceMatrix.M32*IR_ARR[2]) + lightColorSourceMatrix.M33*IR_ARR[3]);

    MAC_ARR[1] = res1 >> 12;
    MAC_ARR[2] = res2 >> 12;
    MAC_ARR[3] = res3 >> 12;

    setIR(1, MAC_ARR[1], lm);
    setIR(2, MAC_ARR[2], lm);
    setIR(3, MAC_ARR[3], lm);

    // fprintf(stderr, "\nncds 2\n\n");
    // fprintf(stderr, "mac1: %lx\n", MAC_ARR[1]);
    // fprintf(stderr, "mac2: %lx\n", MAC_ARR[2]);
    // fprintf(stderr, "mac3: %lx\n", MAC_ARR[3]);

    // fprintf(stderr, "ir1: %x\n", IR_ARR[1]);
    // fprintf(stderr, "ir2: %x\n", IR_ARR[2]);
    // fprintf(stderr, "ir3: %x\n", IR_ARR[3]);

    // setMAC(1, (r * IR_ARR[1]) << 4);
    // setMAC(2, (g * IR_ARR[2]) << 4);
    // setMAC(3, (b * IR_ARR[3]) << 4);

    // res1 = setMAC(1, MAC_ARR[1] + (farColor.x - MAC_ARR[1]) * IR0);
    // res2 = setMAC(2, MAC_ARR[2] + (farColor.y - MAC_ARR[2]) * IR0);
    // res3 = setMAC(3, MAC_ARR[3] + (farColor.z - MAC_ARR[3]) * IR0);

    // MAC_ARR[1] = res1 >> 12;
    // MAC_ARR[2] = res2 >> 12;
    // MAC_ARR[3] = res3 >> 12;

    // Vector3<ub> color;
    // color.x = MAC_ARR[1] >> 4;
    // color.y = MAC_ARR[2] >> 4;
    // color.z = MAC_ARR[3] >> 4;
    // pushColor(color);

    // setIR(1, MAC_ARR[1]);
    // setIR(2, MAC_ARR[2]);
    // setIR(3, MAC_ARR[3]);

    sh OLD_IR_ARR[4];
    OLD_IR_ARR[0] = IR_ARR[0];
    OLD_IR_ARR[1] = IR_ARR[1];
    OLD_IR_ARR[2] = IR_ARR[2];
    OLD_IR_ARR[3] = IR_ARR[3];

    res1 = setMAC(1, ((int64_t)farColor.x << 12) - ((r << 4) * IR_ARR[1])); 
    res2 = setMAC(2, ((int64_t)farColor.y << 12) - ((g << 4) * IR_ARR[2])); 
    res3 = setMAC(3, ((int64_t)farColor.z << 12) - ((b << 4) * IR_ARR[3])); 

    // fprintf(stderr, "\ntest 1\n\n");
    // fprintf(stderr, "farcolor x: %lx\n", ((int64_t)farColor.x << 12));
    // fprintf(stderr, "r: %x\n", (r << 4));
    // fprintf(stderr, "ir1: %x\n", IR_ARR[1]);
    // fprintf(stderr, "resuult: %lx\n", ((int64_t)farColor.x << 12) - ((r << 4) * IR_ARR[1]));
    
    if (sf) 
    {
        MAC_ARR[1] = res1 >> 12;
        MAC_ARR[2] = res2 >> 12;
        MAC_ARR[3] = res3 >> 12;
    }

    setIR(1, MAC_ARR[1]);
    setIR(2, MAC_ARR[2]);
    setIR(3, MAC_ARR[3]);

    // fprintf(stderr, "\nncds 3\n\n");
    // fprintf(stderr, "mac1: %x\n", MAC_ARR[1]);
    // fprintf(stderr, "mac2: %x\n", MAC_ARR[2]);
    // fprintf(stderr, "mac3: %x\n", MAC_ARR[3]);

    // fprintf(stderr, "ir1: %x\n", IR_ARR[1]);
    // fprintf(stderr, "ir2: %x\n", IR_ARR[2]);
    // fprintf(stderr, "ir3: %x\n", IR_ARR[3]);

    res1 = setMAC(1, ((r << 4) * OLD_IR_ARR[1]) + IR0 * IR_ARR[1]); 
    res2 = setMAC(2, ((g << 4) * OLD_IR_ARR[2]) + IR0 * IR_ARR[2]); 
    res3 = setMAC(3, ((b << 4) * OLD_IR_ARR[3]) + IR0 * IR_ARR[3]); 

    if (sf) 
    {
        MAC_ARR[1] = res1 >> 12;
        MAC_ARR[2] = res2 >> 12;
        MAC_ARR[3] = res3 >> 12;
    }

    setIR(1, MAC_ARR[1], lm);
    setIR(2, MAC_ARR[2], lm);
    setIR(3, MAC_ARR[3], lm);


    
    pushColor(MAC_ARR[1] >> 4, MAC_ARR[2] >> 4, MAC_ARR[3] >> 4);


    // fprintf(stderr, "\nncds 4\n\n");
    // fprintf(stderr, "mac1: %lx\n", MAC_ARR[1]);
    // fprintf(stderr, "mac2: %lx\n", MAC_ARR[2]);
    // fprintf(stderr, "mac3: %lx\n", MAC_ARR[3]);

    // fprintf(stderr, "ir1: %x\n", IR_ARR[1]);
    // fprintf(stderr, "ir2: %x\n", IR_ARR[2]);
    // fprintf(stderr, "ir3: %x\n", IR_ARR[3]);
}

int64_t GTE::setMAC(int index, int64_t value)
{
    uint32_t overflowFlag = 0;
    uint32_t underflowFlag = 0;

    if (1 == index)
    {
        overflowFlag = FLAG_BITS::MAC1_OVERFLOW_FLAG;
        underflowFlag = FLAG_BITS::MAC1_UNDERFLOW_FLAG;
    }
    if (2 == index)
    {
        overflowFlag = FLAG_BITS::MAC2_OVERFLOW_FLAG;
        underflowFlag = FLAG_BITS::MAC2_UNDERFLOW_FLAG;
    }
    if (3 == index)
    {
        overflowFlag = FLAG_BITS::MAC3_OVERFLOW_FLAG;
        underflowFlag = FLAG_BITS::MAC3_UNDERFLOW_FLAG;
    }

    if (value >= (1LL << (44 - 1))) flag.reg |= overflowFlag;
    if (value < -(1LL << (44 - 1))) flag.reg |= underflowFlag;

    //Check for overflow/underflow

    //SetMAC
    MAC_ARR[index] = value;

    // fprintf(stderr, "mac%x, %lx\n", index, value);

    return extend_sign(value);
}

int64_t GTE::setMAC0(int64_t value)
{
    // fprintf(stderr, "mac0:, %lx\n", value);
    if (value >= (1LL << (32 - 1))) flag.reg |= FLAG_BITS::MAC0_OVERFLOW_FLAG;
    if (value < -(1LL << (32 - 1))) flag.reg |= FLAG_BITS::MAC0_UNDERFLOW_FLAG;

    MAC0 = value;
    return value;
}

void GTE::setIR(int index, int64_t value, bool lm)
{
    uint32_t saturatedFlag = 0;
    if (1 == index)
    {
        saturatedFlag = FLAG_BITS::IR1_SATURATED;
    }
    if (2 == index)
    {
        saturatedFlag = FLAG_BITS::IR2_SATURATED;
    }
    if (3 == index)
    {
        saturatedFlag = FLAG_BITS::IR3_SATURATED;
    }

    // fprintf(stderr, "setIR%i: %lx\n", index, value);
    if (value > 0x7FFF)
    {
        value = 0x7FFF;
        flag.reg |= saturatedFlag;
    }
    // fprintf(stderr, "lm: %x\n", lm);
    if (value < (lm ? 0 : -0x8000))
    {
        value = (lm ? 0 : -0x8000);
        flag.reg |= saturatedFlag;
    }

    // fprintf(stderr, "setIR%i to: %lx\n", index, value);

    IR_ARR[index] = value;
}

void GTE::pushZ(uh newZ)
{
    ScreenZ[0] = ScreenZ[1];
    ScreenZ[1] = ScreenZ[2];
    ScreenZ[2] = ScreenZ[3];
    ScreenZ[3] = newZ;
}

void GTE::pushXY(uh newX, uh newY)
{
    ScreenXY[0] = ScreenXY[1];
    ScreenXY[1] = ScreenXY[2];
    ScreenXY[2] = Vector3<sh>(newX, newY, 0);
}

void GTE::pushColor(int32_t r, int32_t g, int32_t b)
{
    if (r > 0xff) { flag.reg |= FLAG_BITS::COLOR_R_FIFO_SATURATED; r = 0xff; }
    if (r < 0x00) { flag.reg |= FLAG_BITS::COLOR_R_FIFO_SATURATED; r = 0x00; }

    if (g > 0xff) { flag.reg |= FLAG_BITS::COLOR_G_FIFO_SATURATED; r = 0xff; }
    if (g < 0x00) { flag.reg |= FLAG_BITS::COLOR_G_FIFO_SATURATED; r = 0x00; }

    if (b > 0xff) { flag.reg |= FLAG_BITS::COLOR_B_FIFO_SATURATED; r = 0xff; }
    if (b < 0x00) { flag.reg |= FLAG_BITS::COLOR_B_FIFO_SATURATED; r = 0x00; }

    pushColor(Vector3<ub>{r, g, b});
}

void GTE::pushColor(Vector3<ub> new_color) 
{
    ColorFIFO[0] = ColorFIFO[1];
    ColorFIFO[1] = ColorFIFO[2];
    ColorFIFO[2] = new_color; 
}

void GTE::AVSZ3()
{
    setMAC0((int64_t)ZSF3*(ScreenZ[1] + ScreenZ[2] + ScreenZ[3]));
    OTZ = (MAC0 >> 12);
    if((MAC0 >> 12) > 0xFFFF) {
        //Set flag
        flag.reg |= FLAG_BITS::OTZ_SATURATED;
        OTZ = 0xFFFF;
    }
    if ((MAC0 >> 12) < 0x0) {
        //Set flag
        flag.reg |= FLAG_BITS::OTZ_SATURATED;
        OTZ = 0x0;
    }

    // fprintf(stderr, "otz: %x\n", OTZ);

}

void GTE::RTPS(Vector3<sh> vec)
{
    // fprintf(stderr, "\n\n\n");

    // fprintf(stderr, "vec: x:%x, y:%x, z:%x\n", vec.x, vec.y, vec.z);
    // fprintf(stderr, "sf: %x\n", sf);
    // fprintf(stderr, "\n");
    // fprintf(stderr, "Tvec: x:%x, y:%x, z:%x\n", translationVector.x, translationVector.y, translationVector.z);
    // fprintf(stderr, "\n");
    // fprintf(stderr, "rtMat: M11:%x, M12:%x, M13:%x\n", rotationMatrix.M11, rotationMatrix.M12, rotationMatrix.M13);
    // fprintf(stderr, "rtMat: M21:%x, M22:%x, M23:%x\n", rotationMatrix.M21, rotationMatrix.M22, rotationMatrix.M23);
    // fprintf(stderr, "rtMat: M31:%x, M32:%x, M33:%x\n", rotationMatrix.M31, rotationMatrix.M32, rotationMatrix.M33);
    // fprintf(stderr, "\n");

    // translationVector is mutiplied by 0x1000 to bit shift << 12, to align fixed points
    int64_t res1 = setMAC(1, setMAC(1, setMAC(1, ((int64_t)translationVector.x << 12) + rotationMatrix.M11*vec.x) + rotationMatrix.M12*vec.y) + rotationMatrix.M13*vec.z);
    int64_t res2 = setMAC(2, setMAC(2, setMAC(2, ((int64_t)translationVector.y << 12) + rotationMatrix.M21*vec.x) + rotationMatrix.M22*vec.y) + rotationMatrix.M23*vec.z);
    int64_t res3 = setMAC(3, setMAC(3, setMAC(3, ((int64_t)translationVector.z << 12) + rotationMatrix.M31*vec.x) + rotationMatrix.M32*vec.y) + rotationMatrix.M33*vec.z);
// 
    // fprintf(stderr, "res1: %lx\n", res1);
    // fprintf(stderr, "res2: %lx\n", res2);
    // fprintf(stderr, "res3: %lx\n", res3);

    MAC_ARR[1] = res1 >> 12;
    MAC_ARR[2] = res2 >> 12;
    MAC_ARR[3] = res3 >> 12;

    // fprintf(stderr, "mac1: %lx\n", MAC_ARR[1]);
    // fprintf(stderr, "mac2: %lx\n", MAC_ARR[2]);
    // fprintf(stderr, "mac3: %lx\n", MAC_ARR[3]);

    setIR(1, MAC_ARR[1], lm);
    setIR(2, MAC_ARR[2], lm);
    // setIR(3, MAC_ARR[3], lm);
    IR_ARR[3] = MAC_ARR[3];
    if (MAC_ARR[3] > 0x7FFF || MAC_ARR[3] < -0x8000)
    {
        flag.reg |= FLAG_BITS::IR3_SATURATED;
    }
    if (MAC_ARR[3] > 0x7FFF)
    {
        IR_ARR[3] = 0x7FFF;
    }
    if (MAC_ARR[3] < (lm ? 0 : -0x8000))
    {
        IR_ARR[3] = (lm ? 0 : -0x8000);
    }
    


    // fprintf(stderr, "ir1: %x\n", IR_ARR[1]);
    // fprintf(stderr, "ir2: %x\n", IR_ARR[2]);
    // fprintf(stderr, "ir3: %x\n", IR_ARR[3]);

    //TODO - Saturation details, read docs for function

    // fprintf(stderr, "res1: %lx\n", res1);

    pushZ(MAC_ARR[3] >> ((1-sf)*12));
    // fprintf(stderr, "z3: %x\n", ScreenZ[3]);

    int64_t h_sz3 = ((((int64_t)H_projectionPlaneDistance*0x20000)/ScreenZ[3])+1)/2;
    // fprintf(stderr, "h_sz3: %lx\n", h_sz3);

    int64_t screenX = setMAC0(h_sz3 * IR_ARR[1] + screenOffest.x) >> 16;
    int64_t screenY = setMAC0(h_sz3 * IR_ARR[2] + screenOffest.y) >> 16;
    // fprintf(stderr, "x: %lx, y: %lx\n", screenX, screenY);
    pushXY(screenX, screenY);

    int64_t ir0_temp = setMAC0(h_sz3 * DQA + DQB) >> 12;
    // fprintf(stderr, "ir0_temp: %lx\n", h_sz3 * DQA + DQB);
    IR0 = ir0_temp;
    if ((int32_t)ir0_temp > 0x1000) 
    {
        IR0 = 0x1000; 
        flag.reg |= FLAG_BITS::IR0_SATURATED;
    }
    if ((int32_t)ir0_temp < 0) 
    {
        IR0 = 0x0; 
        flag.reg |= FLAG_BITS::IR0_SATURATED;
    }

    // fprintf(stderr, "MAC0: %x, MAC1: %lx, MAC2: %lx, MAC3: %lx\n", MAC0, MAC_ARR[1], MAC_ARR[2], MAC_ARR[3]);
    // fprintf(stderr, "IR0: %x, IR1: %x, IR2: %x, IR3: %x\n", IR0, IR_ARR[1], IR_ARR[2], IR_ARR[3]);
}

void GTE::RTPT(uw command)
{
    RTPS(vectors[0]);
    RTPS(vectors[1]);
    RTPS(vectors[2]);
}