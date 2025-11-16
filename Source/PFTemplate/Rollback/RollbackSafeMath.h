#pragma once
#include "CoreMinimal.h"
#include <stdint.h>

/*  Q32.32 64-bit fixed point  */
struct FIXED_32
{
    int64_t v = 0;  // raw 32.32 value

    // construction / conversion
    FIXED_32() = default;
    explicit FIXED_32(int64_t raw) : v(raw) {}
    
    static FIXED_32 FromUnits(int32_t whole, uint32_t frac);
    int32_t GetWhole() const;
    uint32_t GetFrac() const;

    // operator overloads
    FIXED_32 operator+(FIXED_32 o) const { return FIXED_32(v + o.v); }
    FIXED_32 operator-(FIXED_32 o) const { return FIXED_32(v - o.v); }
    FIXED_32 operator-() const { return FIXED_32(-v); }
    FIXED_32 operator*(FIXED_32 o) const;
    FIXED_32 operator/(FIXED_32 o) const;
    FIXED_32 Sqrt() const;

    //comparison
    bool operator==(FIXED_32 o) const { return v == o.v; }
    bool operator!=(FIXED_32 o) const { return v != o.v; }
    bool operator< (FIXED_32 o) const { return v <  o.v; }
    bool operator> (FIXED_32 o) const { return v >  o.v; }
    bool operator<=(FIXED_32 o) const { return v <= o.v; }
    bool operator>=(FIXED_32 o) const { return v >= o.v; }

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
    std::pair<FIXED_32,FIXED_32> SinCos() const;
    
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

/* 2-D fixed-point vector for XZ plane (fighting game) */
struct FFixedVector2D
{
    FIXED_32 X;
    FIXED_32 Z;

    //construction
    FFixedVector2D() = default;
    FFixedVector2D(FIXED_32 x, FIXED_32 z) : X(x), Z(z) {}
    
    static FFixedVector2D FromFloatXY(float x, float z);
    void ToFloatXY(float& OutX, float& OutZ) const;

    //operator overloads
    FFixedVector2D operator+(const FFixedVector2D& o) const { return {X + o.X, Z + o.Z}; }
    FFixedVector2D operator-(const FFixedVector2D& o) const { return {X - o.X, Z - o.Z}; }
    FFixedVector2D operator*(FIXED_32 s) const { return {X * s, Z * s}; }
    FFixedVector2D operator/(FIXED_32 s) const { return {X / s, Z / s}; }

    //utilities
    FIXED_32 Dot  (const FFixedVector2D& o) const { return X * o.X + Z * o.Z; }
    FIXED_32 Cross(const FFixedVector2D& o) const { return X * o.Z - Z * o.X; }
    FIXED_32 LengthSquared() const { return Dot(*this); }
    FIXED_32 Length() const { return LengthSquared().Sqrt(); }
    FFixedVector2D GetSafeNormal() const;
    
    friend uint32 GetTypeHash(const FFixedVector2D& V)
    {
        return HashCombine(GetTypeHash(V.X), GetTypeHash(V.Z));
    }

    bool NetSerialize(FArchive& Ar, class UPackageMap*, bool& bOutSuccess)
    {
        Ar << X.v << Z.v;
        bOutSuccess = true;
        return true;
    }
};