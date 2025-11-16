#include "FixedVector2D.h""
#include "Logging/LogMacros.h"

FFixedVector2D FFixedVector2D::GetSafeNormal() const
{
    FIXED_32 len = Length();
    return len.v == 0 ? FFixedVector2D{} : *this / len;
}