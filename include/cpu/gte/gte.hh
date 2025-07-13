#ifndef GTE_hpp 
#define GTE_hpp

#include <global.hh>
#include <cpu/gte/math.hh>

class GTE 
{
private:
    //Data Registers
    Vector3<sh> vectors[3];
    ub r, g, b, c;
    sh OTZ; //Average z
    sh IR0; //16bit Accumulator (Interpolate)
    sh IR_ARR[4];
    Vector3<sh> ScreenXY[3];
    uh ScreenZ[4];
    Vector3<ub> ColorFIFO[4];

    sw MAC0;
    int32_t MAC_ARR[4];
    uh IRGB;
    uh ORGB;
    sw LZCS;
    sw LZCR;
    
    //Control Registers
    Matrix3x3 rotationMatrix;
    Vector3<sw> translationVector;
    Matrix3x3 lightSourceMatrix;
    Vector3<sw> backgroundColor;
    Matrix3x3 lightColorSourceMatrix;
    Vector3<sw> farColor;
    Vector3<sw> screenOffest;
    uh H_projectionPlaneDistance;
    sh DQA;
    sw DQB;
    sh ZSF3;
    sh ZSF4;

    enum FLAG_BITS 
    {
        MAC1_OVERFLOW_FLAG = 0x40000000,
        MAC2_OVERFLOW_FLAG = 0x20000000,
        MAC3_OVERFLOW_FLAG = 0x10000000,
        MAC1_UNDERFLOW_FLAG = 0x8000000,
        MAC2_UNDERFLOW_FLAG = 0x4000000,
        MAC3_UNDERFLOW_FLAG = 0x2000000,

        IR1_SATURATED = 0x1000000,
        IR2_SATURATED = 0x800000,
        IR3_SATURATED = 0x400000,

        COLOR_R_FIFO_SATURATED = 0x200000,
        COLOR_G_FIFO_SATURATED = 0x100000,
        COLOR_B_FIFO_SATURATED = 0x80000,

        OTZ_SATURATED = 0x40000,

        MAC0_OVERFLOW_FLAG = 0x10000,
        MAC0_UNDERFLOW_FLAG = 0x8000,

        IR0_SATURATED = 0x1000,
    };

    union FLAG
    {
        struct
        {
            uh NotUsed : 12;
            ub IR0_sat : 1;
            ub SY2_sat : 1;
            ub SX2_sat : 1;
            ub MAC0_neg_overflow : 1;
            ub MAC0_pos_overflow : 1;
            ub div_overflow : 1;
            ub SZ3_OTZ_sat : 1;
            ub color_fifo_R_sat : 1;
            ub color_fifo_G_sat : 1;
            ub color_fifo_B_sat : 1;
            ub IR1_sat : 1;
            ub IR2_sat : 1;
            ub IR3_sat : 1;
            ub MAC3_neg_overflow : 1;
            ub MAC2_neg_overflow : 1;
            ub MAC1_neg_overflow : 1;
            ub MAC3_pos_overflow : 1;
            ub MAC2_pos_overflow : 1;
            ub MAC1_pos_overflow : 1;
            ub error : 1;
        };
        uw reg;
    };

    FLAG flag;
    
    bool sf;
    bool lm;

    void pushZ(uh newZ);
    void pushXY(uh newX, uh newY);
    void pushColor(Vector3<ub> new_color);
    void pushColor(int32_t r, int32_t g, int32_t b);

    void NCLIP(uw command);
    void NCDS(uw command);
    void MVMVA(uw command);
    void RTPS(Vector3<sh> vec);
    void RTPT(uw command);
    void AVSZ3();
    
    int64_t setMAC(int index, int64_t value);
    int64_t setMAC0(int64_t value);
    void setIR(int index, int64_t value, bool lm = false);
public:
    uw read(ub reg);
    void write(ub reg, uw _data);
    void execCommand(uw command);







};

#endif