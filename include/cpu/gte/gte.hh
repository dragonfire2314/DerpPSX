#ifndef GTE_hpp 
#define GTE_hpp

#include <global.h>
#include <cpu/gte/math.hh>

class GTE 
{
private:
    //Data Registers
    Vector3<sh> vectors[3];
    ub r, g, b, c;
    uh OTZ; //Average z
    uh IR0; //16bit Accumulator (Interpolate)
    uh IR_ARR[4];
    Vector3<sh> ScreenXY[3];
    uh ScreenZ[4];
    Vector3<ub> ColorFIFO[4];

    sw MAC0;
    int64_t MAC_ARR[4];
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
    
    // union highLow
    // {
    //     struct
    //     {
    //         ub b_0;
    //         ub b_1;
    //         ub b_2;
    //         ub b_3;
    //     };
    //     struct
    //     {
    //         uh low;
    //         uh high;
    //     };
    //     uw reg;
    // };

    void pushZ(uh newZ);
    void pushXY(uh newX, uh newY);
    void pushColor(Vector3<ub> new_color);

    template<int T>
    int64_t checkMacOverflow(int64_t val);

    template<int T>
    int32_t setIR(int32_t val);

    void NCLIP(uw command);
    void NCDS(uw command);
    void MVMVA(uw command);
    void RTPS(Vector3<sh> vec, ub sf);
    void RTPT(uw command);
    
    void setMAC(int index, int64_t value);
    void setIR(int index, sh value);

    void matrixMultVectorAddVector(Vector3<sh> vecAdd, Vector3<sh> vecMult, Matrix3x3 &mat, ub sf);
public:
    uw read(ub reg);
    void write(ub reg, uw _data);
    void execCommand(uw command);







};

#endif