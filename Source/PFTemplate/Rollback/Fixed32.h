#pragma once
#include "CoreMinimal.h"
#include <stdint.h>

/*  Q32.32 64-bit fixed point  */
USTRUCT(BlueprintType)
struct FIXED_32
{
    GENERATED_BODY()
    
    int64_t v = 0;  // raw 32.32 value

    // construction / conversion
    FIXED_32() = default;
    FIXED_32(float f) { v = (int64_t)(f * (double)(1ULL << 32)); }
    
    static FIXED_32 FromUnits(int32_t whole, uint32_t frac);
    int32_t GetWhole() const;
    uint32_t GetFrac() const;

    // operator overloads
    FIXED_32 operator+(FIXED_32 o) const { return FIXED_32(v + o.v); }
    FIXED_32& operator+=(const FIXED_32& o)
    {
        v += o.v;
        return *this;
    }
    FIXED_32 operator-(FIXED_32 o) const { return FIXED_32(v - o.v); }
    FIXED_32& operator-=(const FIXED_32& o)
    {
        v -= o.v;
        return *this;
    }
    FIXED_32 operator-() const { return FIXED_32(-v); }
    FIXED_32 operator*(FIXED_32 o) const;
    FIXED_32& operator*=(const FIXED_32& o)
    {
        *this = *this * o;
        return *this;
    }
    FIXED_32 operator/(FIXED_32 o) const;
    FIXED_32& operator/=(const FIXED_32& o)
    {
        *this = *this / o;
        return *this;
    }

    //comparison
    bool operator==(FIXED_32 o) const { return v == o.v; }
    bool operator!=(FIXED_32 o) const { return v != o.v; }
    bool operator< (FIXED_32 o) const { return v <  o.v; }
    bool operator> (FIXED_32 o) const { return v >  o.v; }
    bool operator<=(FIXED_32 o) const { return v <= o.v; }
    bool operator>=(FIXED_32 o) const { return v >= o.v; }

    FIXED_32 Sqrt() const;
    FIXED_32 Abs() const { return v < 0 ? FIXED_32(-v) : *this; }
    static FIXED_32 Min(FIXED_32 a, FIXED_32 b) { return a < b ? a : b; }
    static FIXED_32 Max(FIXED_32 a, FIXED_32 b) { return a > b ? a : b; }
    FIXED_32 Clamp(FIXED_32 min, FIXED_32 max) const { return Max(min, Min(max, *this));}

    static constexpr FIXED_32 Pi()       { return FIXED_32(0x3243F6A89LL); } // pi
    static constexpr FIXED_32 TWO_Pi()   { return FIXED_32(0x6487ED511LL); } // 2*pi
    static constexpr FIXED_32 HALF_Pi()  { return FIXED_32(0x1921FB544LL); } // pi/2
    static constexpr FIXED_32 INV_TWO_Pi(){ return FIXED_32(0x28BE60DCLL); }  // 1/(2*pi)

    FIXED_32 Sin() const;
    FIXED_32 Cos() const;
    
    friend uint32 GetTypeHash(FIXED_32 F) { return (uint32)(F.v ^ (F.v >> 32)); }

    bool NetSerialize(FArchive& Ar, class UPackageMap*, bool& bOutSuccess)
    {
        Ar.Serialize(&v, sizeof(v));
        bOutSuccess = true;
        return true;
    }

private:
    FIXED_32 MulHigh(FIXED_32 b) const;
    static FIXED_32 poly5(FIXED_32 x2);
};

FORCEINLINE float FixedToFloat(FIXED_32 f)   { return (float)(f.v / 4294967296.0); }
FORCEINLINE FIXED_32 FloatToFixed(float fl)  { return FIXED_32(fl); }