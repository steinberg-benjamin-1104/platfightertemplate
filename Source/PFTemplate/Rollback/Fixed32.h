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
    explicit FIXED_32(int32_t whole) { v = (int64_t)whole << 32; }
    FIXED_32(float f) { v = (int64_t)(f * (double)(1ULL << 32)); }

    // factory for raw bits
    static FIXED_32 FromRaw(int64_t raw) { return FIXED_32(raw); }

    static FIXED_32 FromUnits(int32_t whole, uint32_t frac);
    int32_t GetWhole() const;
    uint32_t GetFrac() const;

    // operator overloads
    // addition
    FIXED_32 operator+(FIXED_32 rhs) const { return FIXED_32(v + rhs.v); }
    FIXED_32& operator+=(const FIXED_32& rhs) { v += rhs.v; return *this; }

    // subtraction
    FIXED_32 operator-(FIXED_32 rhs) const { return FIXED_32(v - rhs.v); }
    FIXED_32& operator-=(const FIXED_32& rhs) { v -= rhs.v; return *this; }
    FIXED_32 operator-() const { return FIXED_32(-v); }

    // multiplication
    FIXED_32 operator*(FIXED_32 rhs) const;
    FIXED_32& operator*=(const FIXED_32& rhs) { *this = *this * rhs; return *this; }

    // division
    FIXED_32 operator/(FIXED_32 rhs) const;
    FIXED_32& operator/=(const FIXED_32& rhs) { *this = *this / rhs; return *this; }

    // comparison
    bool operator==(FIXED_32 o) const { return v == o.v; }
    bool operator!=(FIXED_32 o) const { return v != o.v; }
    bool operator< (FIXED_32 o) const { return v <  o.v; }
    bool operator> (FIXED_32 o) const { return v >  o.v; }
    bool operator<=(FIXED_32 o) const { return v <= o.v; }
    bool operator>=(FIXED_32 o) const { return v >= o.v; }

    FIXED_32 Sqrt() const;
    FIXED_32 Abs() const { return v < 0 ? FIXED_32(-v) : *this; }
    int32 Sign() const { return (v > 0) ? 1 : -1; }

    static constexpr FIXED_32 Pi()        { return FIXED_32(0x3243F6A89LL); }
    static constexpr FIXED_32 TWO_Pi()    { return FIXED_32(0x6487ED511LL); }
    static constexpr FIXED_32 HALF_Pi()   { return FIXED_32(0x1921FB544LL); }
    static constexpr FIXED_32 INV_TWO_Pi(){ return FIXED_32(0x28BE60DCLL); }

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
    explicit FIXED_32(int64_t raw) : v(raw) {}
    FIXED_32 MulHigh(FIXED_32 b) const;
    static FIXED_32 poly5(FIXED_32 x2);
};

// safe int32 multipliers
FORCEINLINE FIXED_32 operator*(const FIXED_32& lhs, int32 rhs)
{
    return FIXED_32::FromRaw(lhs.v * rhs);
}
FORCEINLINE FIXED_32 operator*(int32 lhs, const FIXED_32& rhs)
{
    return rhs * lhs;
}

// helpers
FORCEINLINE float  FixedToFloat(FIXED_32 f)   { return (float)(f.v / 4294967296.0); }
FORCEINLINE FIXED_32 FloatToFixed(float fl)   { return FIXED_32(fl); }
FORCEINLINE FIXED_32 FixedMin(FIXED_32 a, FIXED_32 b) { return (a < b) ? a : b; }
FORCEINLINE FIXED_32 FixedMax(FIXED_32 a, FIXED_32 b) { return (a > b) ? a : b; }
FORCEINLINE FIXED_32 FixedClamp(FIXED_32 v, FIXED_32 min, FIXED_32 max)
{
    return FixedMax(min, FixedMin(max, v));
}