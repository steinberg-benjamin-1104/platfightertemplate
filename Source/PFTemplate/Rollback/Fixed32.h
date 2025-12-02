#pragma once
#ifdef FixedToFloat
#undef FixedToFloat
#endif

#ifdef FloatToFixed
#undef FloatToFixed
#endif

#include "CoreMinimal.h"
#include <stdint.h>

struct FFixed_32
{
    int64_t v = 0;  // raw 32.32 value

    // construction / conversion
    FFixed_32() = default;
    explicit FFixed_32(int32_t whole) { v = (int64_t)whole << 32; }
    FFixed_32(float f) { v = (int64_t)(f * (double)(1ULL << 32)); }
    explicit FFixed_32(int64_t Raw) : v(Raw) {}

    // factory for raw bits
    static FFixed_32 FromRaw(int64_t raw) { return FFixed_32(raw); }

    static FFixed_32 FromUnits(int32_t whole, uint32_t frac);
    int32_t GetWhole() const;
    uint32_t GetFrac() const;

    // operator overloads
    // addition
    FFixed_32 operator+(FFixed_32 rhs) const { return FFixed_32(v + rhs.v); }
    FFixed_32& operator+=(const FFixed_32& rhs) { v += rhs.v; return *this; }

    // subtraction
    FFixed_32 operator-(FFixed_32 rhs) const { return FFixed_32(v - rhs.v); }
    FFixed_32& operator-=(const FFixed_32& rhs) { v -= rhs.v; return *this; }
    FFixed_32 operator-() const { return FFixed_32(-v); }

    // multiplication
    FFixed_32 operator*(FFixed_32 rhs) const;
    FFixed_32& operator*=(const FFixed_32& rhs) { *this = *this * rhs; return *this; }

    // division
    FFixed_32 operator/(FFixed_32 rhs) const;
    FFixed_32& operator/=(const FFixed_32& rhs) { *this = *this / rhs; return *this; }

    // comparison
    bool operator==(FFixed_32 o) const { return v == o.v; }
    bool operator!=(FFixed_32 o) const { return v != o.v; }
    bool operator< (FFixed_32 o) const { return v <  o.v; }
    bool operator> (FFixed_32 o) const { return v >  o.v; }
    bool operator<=(FFixed_32 o) const { return v <= o.v; }
    bool operator>=(FFixed_32 o) const { return v >= o.v; }

    FFixed_32 Sqrt() const;
    FFixed_32 Abs() const { return v < 0 ? FFixed_32(-v) : *this; }
    int32 Sign() const { return (v > 0) ? 1 : -1; }

    FFixed_32 Sin() const;
    FFixed_32 Cos() const;

    friend uint32 GetTypeHash(FFixed_32 F) { return (uint32)(F.v ^ (F.v >> 32)); }

    bool NetSerialize(FArchive& Ar, class UPackageMap*, bool& bOutSuccess)
    {
        Ar.Serialize(&v, sizeof(v));
        bOutSuccess = true;
        return true;
    }

private:
    FFixed_32 MulHigh(FFixed_32 b) const;
    static FFixed_32 poly5(FFixed_32 x2);
};

constexpr int64_t ONE  = 1LL << 32;
constexpr int64_t HALF = 1LL << 31;
static const FFixed_32 Fixed_Pi = FFixed_32(3.14159265358979323846f);
static const FFixed_32 half_pi    = FFixed_32(1.57079632679489661923f);
static const FFixed_32 atan_p1   = FFixed_32(0.9998660f);
static const FFixed_32 atan_p2    = FFixed_32(-0.3302995f);
static const FFixed_32 RAD_TO_DEG = FFixed_32(57.29577951308232f);
static const FFixed_32 DEG_TO_RAD = FFixed_32(0.017453292519943295f);

// safe int32 multipliers
FORCEINLINE FFixed_32 operator*(const FFixed_32& lhs, int32 rhs)
{
    return FFixed_32::FromRaw(lhs.v * rhs);
}
FORCEINLINE FFixed_32 operator*(int32 lhs, const FFixed_32& rhs)
{
    return rhs * lhs;
}
FORCEINLINE FFixed_32 operator/(const FFixed_32& lhs, int32 rhs)
{
    return FFixed_32::FromRaw(lhs.v / rhs);
}

FORCEINLINE float  FixedToFloat(FFixed_32 f)   { return (float)(f.v / 4294967296.0); }
FORCEINLINE FFixed_32 FloatToFixed(float fl)   { return FFixed_32(fl); }
FORCEINLINE FFixed_32 FixedMin(FFixed_32 a, FFixed_32 b) { return (a < b) ? a : b; }
FORCEINLINE FFixed_32 FixedMax(FFixed_32 a, FFixed_32 b) { return (a > b) ? a : b; }
FORCEINLINE FFixed_32 FixedClamp(FFixed_32 v, FFixed_32 min, FFixed_32 max)

{
    return FixedMax(min, FixedMin(max, v));
}

FORCEINLINE int32 FixedFloor(const FFixed_32& x)
{
    const int64_t raw = x.v;
    const int64_t whole = raw >> 32;

    if (raw >= 0)
    {
        // Positive numbers: shifting already floors.
        return (int32)whole;
    }
    else
    {
        // If exactly an integer, just return it.
        if ((raw & 0xFFFFFFFFLL) == 0)
        {
            return (int32)whole;
        }
        else
        {
            // Negative fractional → floor goes one lower.
            return (int32)(whole - 1);
        }
    }
}

// atan(x) approximation for fixed point
static FORCEINLINE FFixed_32 FixedAtan(FFixed_32 x)
{
    // Polynomial: x * (p1 + p2 * x^2)
    FFixed_32 x2 = x * x;
    return x * (atan_p1 + atan_p2 * x2);
}

static FORCEINLINE FFixed_32 FixedAtan2(FFixed_32 y, FFixed_32 x)
{
    if (x.v > 0)
    {
        // atan(y/x)
        return FixedAtan(y / x);
    }
    else if (x.v < 0)
    {
        if (y.v >= 0)
        {
            return FixedAtan(y / x) + PI;
        }
        else
        {
            return FixedAtan(y / x) - PI;
        }
    }
    else // x == 0
    {
        if (y.v > 0)  return HALF_PI;
        if (y.v < 0)  return -HALF_PI;
        return FFixed_32(0); // undefined, return 0
    }
}

static FORCEINLINE FFixed_32 FixedRadiansToDegrees(FFixed_32 r)
{
    return r * RAD_TO_DEG;
}

FORCEINLINE FFixed_32 FixedDegreesToRadians(FFixed_32 d)
{
    return d * DEG_TO_RAD;
}