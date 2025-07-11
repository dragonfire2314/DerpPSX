#include <cpu/gte/gte.hh>

// void GTE::MVMVA(uw command)
// {
//     printf("command: %x\n", command);

//     system("PAUSE");
// }

// void GTE::NCLIP(uw command)
// {
//     // MAC0 = ScreenXY[0].x * (ScreenXY[1].y - ScreenXY[2].y) + ScreenXY[1].x * (ScreenXY[2].y - ScreenXY[0].y) + ScreenXY[2].x * (ScreenXY[0].y - ScreenXY[1].y);
// }

// void GTE::NCDS(uw command)
// {
//     // ub sf = (command & 0x80000) >> 19;

//     // // matrixMultVector(vectors[0], lightSourceMatrix);
//     // IR1 = MAC1 = ((lightSourceMatrix.M11 * vectors[0].x) + (lightSourceMatrix.M12 * vectors[0].y) + (lightSourceMatrix.M13 * vectors[0].z)) >> (sf * 12);
//     // IR2 = MAC2 = ((lightSourceMatrix.M21 * vectors[0].x) + (lightSourceMatrix.M22 * vectors[0].y) + (lightSourceMatrix.M23 * vectors[0].z)) >> (sf * 12);
//     // IR3 = MAC3 = ((lightSourceMatrix.M31 * vectors[0].x) + (lightSourceMatrix.M32 * vectors[0].y) + (lightSourceMatrix.M33 * vectors[0].z)) >> (sf * 12);

//     // IR1 = MAC1 = (backgroundColor.x * 0x1000 + (lightColorSourceMatrix.M11 * IR1) + (lightColorSourceMatrix.M12 * IR2) + (lightColorSourceMatrix.M13 * IR3)) >> (sf * 12);
//     // IR2 = MAC2 = (backgroundColor.y * 0x1000 + (lightColorSourceMatrix.M21 * IR1) + (lightColorSourceMatrix.M22 * IR2) + (lightColorSourceMatrix.M23 * IR3)) >> (sf * 12);
//     // IR3 = MAC3 = (backgroundColor.z * 0x1000 + (lightColorSourceMatrix.M31 * IR1) + (lightColorSourceMatrix.M32 * IR2) + (lightColorSourceMatrix.M33 * IR3)) >> (sf * 12);

//     // MAC1 = r * IR1 << 4;
//     // MAC2 = g * IR2 << 4;
//     // MAC3 = b * IR3 << 4;

//     // // MAC1 =  TODO

//     // MAC1 = MAC1 >> (sf * 12);
//     // MAC2 = MAC2 >> (sf * 12);
//     // MAC3 = MAC3 >> (sf * 12);

//     // Vector3<ub> color;
//     // color.x = MAC1 / 16;
//     // color.y = MAC2 / 16;
//     // color.z = MAC3 / 16;
//     // pushColor(color);

//     // IR1 = MAC1;
//     // IR2 = MAC2;
//     // IR3 = MAC3;
// }

// void GTE::setMAC(int index, int64_t value)
// {
//     if (index < 0 || index > 3) 
//     {
//         printf("BAD MAC INDEX only 0-3 allowed\n");
//         system("PASUE");
//     }

//     // int64_t fixed_value = value & ;

//     if (index == 0) 
//     {
//         MAC0 = value;
//         return;
//     }
    
//     //Check and set overflow bit
//     if (value >= (1LL << (43))) 
//     {
//         if (index == 1) flag.MAC1_pos_overflow = 1;
//         if (index == 2) flag.MAC2_pos_overflow = 1;
//         if (index == 3) flag.MAC3_pos_overflow = 1;
//     }
//     else if (value < -(1LL << (43))) 
//     {
//         if (index == 1) flag.MAC1_neg_overflow = 1;
//         if (index == 2) flag.MAC2_neg_overflow = 1;
//         if (index == 3) flag.MAC3_neg_overflow = 1;
//     }

//     MAC_ARR[index - 1] = value;
// }

// void GTE::setIR(int index, sh value)
// {

// }

// void GTE::pushZ(uh newZ)
// {
//     ScreenZ[0] = ScreenZ[1];
//     ScreenZ[1] = ScreenZ[2];
//     ScreenZ[2] = ScreenZ[3];
//     ScreenZ[3] = newZ;
// }

// void GTE::pushXY(uh newX, uh newY)
// {
//     ScreenXY[0] = ScreenXY[1];
//     ScreenXY[1] = ScreenXY[2];
//     ScreenXY[2] = Vector3<sh>(newX, newY, 0);
// }

// void GTE::pushColor(Vector3<ub> new_color) 
// {
//     ColorFIFO[0] = ColorFIFO[1];
//     ColorFIFO[1] = ColorFIFO[2];
//     ColorFIFO[2] = new_color; 
// }

// void GTE::matrixMultVectorAddVector(Vector3<sh> vecAdd, Vector3<sh> vecMult, Matrix3x3 &mat, ub sf)
// {
//     // IR1 = MAC1 = (vecAdd.x + (mat.M11 * vec.x) + (mat.M12 * vec.y) + (mat.M13 * vec.z)) >> (sf * 12);
//     // IR2 = MAC2 = (vecAdd.y + (mat.M21 * vec.x) + (mat.M22 * vec.y) + (mat.M23 * vec.z)) >> (sf * 12);
//     // IR3 = MAC3 = (vecAdd.z + (mat.M31 * vec.x) + (mat.M32 * vec.y) + (mat.M33 * vec.z)) >> (sf * 12);
// }

// template<int T>
// int64_t GTE::checkMacOverflow(int64_t val)
// {
//     static_assert(T >= 1 && T <= 3);

//     uint64_t ret = val;

//     if (val >= (1LL << (44 - 1))) printf("Overflow mac\n");
//     if (val < -(1LL << (44 - 1))) printf("Underflow mac\n");

//     MAC_ARR[T] = val >> 12;

//     return ret;
// }

// template<int T>
// int32_t GTE::setIR(int32_t val)
// {
//     static_assert(T >= 1 && T <= 3);

//     IR_ARR[T] = val;

//     return val;
// }

// void GTE::RTPS(Vector3<sh> vec, ub sf)
// {
//     uint64_t result1 = checkMacOverflow<1>((translationVector.x * 0x1000) + (rotationMatrix.M11 * vec.x));
//     result1 = checkMacOverflow<1>(result1 + rotationMatrix.M12 * vec.y);
//     result1 = checkMacOverflow<1>(result1 + rotationMatrix.M13 * vec.z);

//     uint64_t result2 = checkMacOverflow<1>((translationVector.y * 0x1000) + (rotationMatrix.M12 * vec.x));
//     result2 = checkMacOverflow<1>(result1 + rotationMatrix.M22 * vec.y);
//     result2 = checkMacOverflow<1>(result1 + rotationMatrix.M23 * vec.z);

//     uint64_t result3 = checkMacOverflow<1>((translationVector.z * 0x1000) + (rotationMatrix.M13 * vec.x));
//     result3 = checkMacOverflow<1>(result1 + rotationMatrix.M23 * vec.y);
//     result3 = checkMacOverflow<1>(result1 + rotationMatrix.M33 * vec.z);

//     uh z = MAC_ARR[3];
//     if (z == 0)
//         z = 1;

//     setIR<1>(result1);
//     setIR<2>(result2);
//     setIR<3>(result3);

//     pushZ(z);

//     // MAC0 = (((H_projectionPlaneDistance*0x20000/z)+1)/2)*IR1+screenOffest.x;
//     // uh newX = MAC0 / 0x10000;

//     // MAC0 = (((H_projectionPlaneDistance*0x20000/z)+1)/2)*IR2+screenOffest.y;
//     // uh newY = MAC0 / 0x10000;

//     // MAC0 = (((H_projectionPlaneDistance*0x20000/z)+1)/2)*DQA+DQB;
//     // IR0 = MAC0 / 0x1000;

//     // pushXY(newX, newY);
// }

// void GTE::RTPT(uw command)
// {
//     ub sf = (command & 0x80000) >> 19;
//     RTPS(vectors[0], sf);
//     RTPS(vectors[1], sf);
//     RTPS(vectors[2], sf);
// }

// template <>
// int64_t GTE::setMac<0>(int64_t value) {
//     checkOverflow<32>(value, Flag::MAC0_OVERFLOW_POSITIVE, Flag::MAC0_OVERFLOW_NEGATIVE);
//     mac[0] = value;
//     return value;
// }

// template <int i>
// int64_t GTE::setMac(int64_t value) {
//     static_assert(i >= 1 && i <= 3, "Invalid mac for GTE::setMac");

//     if (i == 1) {
//         checkOverflow<44>(value, Flag::MAC1_OVERFLOW_POSITIVE, Flag::MAC1_OVERFLOW_NEGATIVE);
//     } else if (i == 2) {
//         checkOverflow<44>(value, Flag::MAC2_OVERFLOW_POSITIVE, Flag::MAC2_OVERFLOW_NEGATIVE);
//     } else if (i == 3) {
//         checkOverflow<44>(value, Flag::MAC3_OVERFLOW_POSITIVE, Flag::MAC3_OVERFLOW_NEGATIVE);
//     }

//     if (sf) value >>= 12;

//     mac[i] = value;
//     return value;
// }

// template <int i>
// void GTE::setIr(int32_t value, bool lm) {
//     static_assert(i >= 1 && i <= 3, "Invalid ir for GTE::setIr");

//     uint32_t saturatedBits = 0;
//     if (i == 1) {
//         saturatedBits = Flag::IR1_SATURATED;
//     } else if (i == 2) {
//         saturatedBits = Flag::IR2_SATURATED;
//     } else if (i == 3) {
//         saturatedBits = Flag::IR3_SATURATED;
//     }

//     ir[i] = clip(value, 0x7fff, lm ? 0 : -0x8000, saturatedBits);
// }

// template <int i>
// void GTE::setMacAndIr(int64_t value, bool lm) {
//     setIr<i>(setMac<i>(value), lm);
// }

// int64_t GTE::multiplyMatrixByVectorRTP(Matrix m, Vector<int16_t> v, Vector<int32_t> tr) {
//     Vector<int64_t> result;

//     result.x = O(1, O(1, O(1, ((int64_t)tr.x << 12) + m[0][0] * v.x) + m[0][1] * v.y) + m[0][2] * v.z);
//     result.y = O(2, O(2, O(2, ((int64_t)tr.y << 12) + m[1][0] * v.x) + m[1][1] * v.y) + m[1][2] * v.z);
//     result.z = O(3, O(3, O(3, ((int64_t)tr.z << 12) + m[2][0] * v.x) + m[2][1] * v.y) + m[2][2] * v.z);

//     setMacAndIr<1>(result.x, lm);
//     setMacAndIr<2>(result.y, lm);
//     setMac<3>(result.z);

//     // RTP calculates IR3 saturation flag as if lm bit was always false
//     clip(result.z >> 12, 0x7fff, -0x8000, Flag::IR3_SATURATED);

//     // But calculation itself respects lm bit
//     ir[3] = clip(mac[3], 0x7fff, lm ? 0 : -0x8000);

//     return result.z;
// }



// void GTE::rtps(int n, bool setMAC0) {
//     int64_t mac3 = multiplyMatrixByVectorRTP(rotation, v[n], translation);

//     pushScreenZ((int32_t)(mac3 >> 12));
//     int64_t h_s3z = divideUNR(h, s[3].z);

//     int16_t ir1 = ir[1];
//     if (widescreenHack) ir1 = (int16_t)((int32_t)ir1 * 3 / 4);

//     int32_t x = setMac<0>(h_s3z * ir1 + of[0]) >> 16;
//     int32_t y = setMac<0>(h_s3z * ir[2] + of[1]) >> 16;
//     pushScreenXY(x, y);

//     if (setMAC0) {
//         int64_t mac0 = setMac<0>(h_s3z * dqa + dqb);
//         ir[0] = clip(mac0 >> 12, 0x1000, 0x0000, Flag::IR0_SATURATED);
//     }
// }

// /**
//  * Same as RTPS, but repeated for vector 0, 1 and 2
//  */
// void GTE::rtpt() {
//     rtps(0, false);
//     rtps(1, false);
//     rtps(2, true);
// }