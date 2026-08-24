//
// Created by admin on 06.01.2024.
//

#include "../game/RW/common.h"
#include "../main.h"
#include "Vector.h"

class CompressedVector {
public:
    int16 x;
    int16 y;
    int16 z;
};
VALIDATE_SIZE(CompressedVector, 0x6);

CVector UncompressVector(const CompressedVector& compressedVec);
CompressedVector CompressVector(const CVector& vec);

float UncompressUnitFloat(int16 val);

CVector UncompressUnitVector(const CompressedVector& compressedVec);
CompressedVector CompressUnitVector(const CVector& vec);

CVector UncompressLargeVector(const CompressedVector& compressedVec);
CompressedVector CompressLargeVector(const CVector& vec);

CVector UncompressFxVector(const CompressedVector& compressedVec);
CVector CompressFxVector(const CompressedVector& compressedVec);