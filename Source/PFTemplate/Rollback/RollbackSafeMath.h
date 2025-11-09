#pragma once
#include "CoreMinimal.h"
#include <stdint.h>

/*  Q32.32 64-bit fixed point  */
struct FIXED_32
{
    int64_t v = 0;                       // raw value
    static constexpr int32_t FRAC = 32;  // 32 frac bits
    static constexpr int64_t ONE  = 1LL << FRAC;
    static constexpr int64_t HALF = 1LL << (FRAC - 1);

    FIXED_32() = default;
    explicit FIXED_32(int64_t raw) : v(raw) {}
    FIXED_32(int32_t whole, uint32_t frac) : v((int64_t)whole * ONE + (int64_t)frac) {}

    /* cast helpers (editor / debug only) */
    FORCEINLINE float AsFloat() const { return (float)v / (float)ONE; }
    FORCEINLINE double AsDouble() const { return (double)v / (double)ONE; }

    /* basic ops – branch-free, deterministic */
    FORCEINLINE FIXED_32 operator+(FIXED_32 o) const { return FIXED_32(v + o.v); }
    FORCEINLINE FIXED_32 operator-(FIXED_32 o) const { return FIXED_32(v - o.v); }
    FORCEINLINE FIXED_32 operator-() const { return FIXED_32(-v); }

    /* mul / div keep 32 frac bits */
    FORCEINLINE FIXED_32 operator*(FIXED_32 o) const
    {
        return FIXED_32((int64_t)(((__int128_t)v * (__int128_t)o.v) >> FRAC));
    }
    FORCEINLINE FIXED_32 operator/(FIXED_32 o) const
    {
        return FIXED_32((int64_t)(((__int128_t)v << FRAC) / (__int128_t)o.v));
    }

    /* compare */
    FORCEINLINE bool operator==(FIXED_32 o) const { return v == o.v; }
    FORCEINLINE bool operator!=(FIXED_32 o) const { return v != o.v; }
    FORCEINLINE bool operator<(FIXED_32 o) const { return v < o.v; }
    FORCEINLINE bool operator>(FIXED_32 o) const { return v > o.v; }
    FORCEINLINE bool operator<=(FIXED_32 o) const { return v <= o.v; }
    FORCEINLINE bool operator>=(FIXED_32 o) const { return v >= o.v; }

    /* deterministic sqrt (integer Newton) */
    FORCEINLINE FIXED_32 Sqrt() const
    {
        if (v <= 0) return FIXED_32(0);
        uint64_t r = (uint64_t)v;
        uint64_t b = 1ULL << 62;
        while (b > r) b >>= 2;
        uint64_t res = 0;
        while (b != 0)
        {
            if (r >= res + b) { r -= res + b; res = (res >> 1) + b; }
            else res >>= 1;
            b >>= 2;
        }
        return FIXED_32((int64_t)res << (FRAC / 2)); // sqrt(v) in Q32.32
    }

    /* deterministic abs / clamp / min / max */
    FORCEINLINE FIXED_32 Abs() const { return v < 0 ? FIXED_32(-v) : *this; }
    FORCEINLINE static FIXED_32 Min(FIXED_32 a, FIXED_32 b) { return a < b ? a : b; }
    FORCEINLINE static FIXED_32 Max(FIXED_32 a, FIXED_32 b) { return a > b ? a : b; }
    FORCEINLINE FIXED_32 Clamp(FIXED_32 min, FIXED_32 max) const
    {
        return Max(min, Min(max, *this));
    }
};

/* 3-D vector using fixed point  */
struct FFixedVector
{
    FIXED_32 X, Y, Z;
    FFixedVector() = default;
    FFixedVector(FIXED_32 x, FIXED_32 y, FIXED_32 z) : X(x), Y(y), Z(z) {}
    explicit FFixedVector(const FVector& FloatVec)
        : X(FIXED_32((int64_t)(FloatVec.X * FIXED_32::ONE)))
        , Y(FIXED_32((int64_t)(FloatVec.Y * FIXED_32::ONE)))
        , Z(FIXED_32((int64_t)(FloatVec.Z * FIXED_32::ONE)))
    {}

    FORCEINLINE FVector ToFVector() const
    {
        return FVector(X.AsFloat(), Y.AsFloat(), Z.AsFloat());
    }

    /* basic ops */
    FORCEINLINE FFixedVector operator+(const FFixedVector& o) const
    { return FFixedVector(X + o.X, Y + o.Y, Z + o.Z); }
    FORCEINLINE FFixedVector operator-(const FFixedVector& o) const
    { return FFixedVector(X - o.X, Y - o.Y, Z - o.Z); }
    FORCEINLINE FFixedVector operator*(FIXED_32 s) const
    { return FFixedVector(X * s, Y * s, Z * s); }
    FORCEINLINE FFixedVector operator/(FIXED_32 s) const
    { return FFixedVector(X / s, Y / s, Z / s); }

    /* dot / cross / len² / len */
    FORCEINLINE FIXED_32 Dot(const FFixedVector& o) const
    { return X * o.X + Y * o.Y + Z * o.Z; }
    FORCEINLINE FFixedVector Cross(const FFixedVector& o) const
    {
        return FFixedVector(
            Y * o.Z - Z * o.Y,
            Z * o.X - X * o.Z,
            X * o.Y - Y * o.X);
    }
    FORCEINLINE FIXED_32 LengthSquared() const
    { return Dot(*this); }
    FORCEINLINE FIXED_32 Length() const
    { return LengthSquared().Sqrt(); }

    /* deterministic normalize (avoids div-by-zero) */
    FORCEINLINE FFixedVector GetSafeNormal() const
    {
        FIXED_32 len = Length();
        return len.v == 0 ? FFixedVector(FIXED_32(0), FIXED_32(0), FIXED_32(0)) : *this / len;
    }
};

/* 2-D AABB intersection – zero allocations, branchless  */
struct FFixedBox2D
{
    FFixedVector Min, Max;
    FFixedBox2D() = default;
    FFixedBox2D(const FFixedVector& min, const FFixedVector& max) : Min(min), Max(max) {}

    FORCEINLINE bool Intersect(const FFixedBox2D& o) const
    {
        return !(Max.X < o.Min.X || o.Max.X < Min.X ||
                 Max.Y < o.Min.Y || o.Max.Y < Min.Y);
    }
    FORCEINLINE bool Contains(const FFixedVector& p) const
    {
        return p.X >= Min.X && p.X <= Max.X &&
               p.Y >= Min.Y && p.Y <= Max.Y;
    }
    FORCEINLINE FFixedVector GetCenter() const
    { return (Min + Max) * FIXED_32(FIXED_32::HALF); }
    FORCEINLINE FFixedVector GetExtent() const
    { return (Max - Min) * FIXED_32(FIXED_32::HALF); }
};