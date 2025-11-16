#include "FixedVector2D.h""
#include "Logging/LogMacros.h"

FFixedVector2D FFixedVector2D::FromFloatXY(float x, float z)
{
    return { FIXED_32((int64_t)(x * (float)(1LL << 32))),
             FIXED_32((int64_t)(z * (float)(1LL << 32))) };
}

void FFixedVector2D::ToFloatXY(float& OutX, float& OutZ) const
{
    const float inv = 1.0f / (float)(1LL << 32);
    OutX = (float)X.v * inv;
    OutZ = (float)Z.v * inv;
}

FFixedVector2D FFixedVector2D::GetSafeNormal() const
{
    FIXED_32 len = Length();
    return len.v == 0 ? FFixedVector2D{} : *this / len;
}