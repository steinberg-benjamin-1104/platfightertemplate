#pragma once
#include "Fixed32.h"

struct FFixedVector2D
{
    FFixed_32 X;
    FFixed_32 Z;

    //construction
    FFixedVector2D() = default;
    FFixedVector2D(FFixed_32 x, FFixed_32 z) : X(x), Z(z) {}
    FFixedVector2D(FFixed_32 x, float z) : X(x), Z(z) {}
    FFixedVector2D(float x, FFixed_32 z) : X(x), Z(z) {}
    FFixedVector2D(float x, float z) : X(x), Z(z) {}

    //operator overloads
    FFixedVector2D operator+(const FFixedVector2D& o) const { return {X + o.X, Z + o.Z}; }
    FFixedVector2D operator-(const FFixedVector2D& o) const { return {X - o.X, Z - o.Z}; }
    FFixedVector2D operator*(FFixed_32 s) const { return {X * s, Z * s}; }
    FFixedVector2D operator/(FFixed_32 s) const { return {X / s, Z / s}; }
    FFixedVector2D& operator*=(FFixed_32 s)
    {
        X *= s;
        Z *= s;
        return *this;
    }

    //utilities
    FFixed_32 Dot  (const FFixedVector2D& o) const { return X * o.X + Z * o.Z; }
    static FFixed_32 Dot(const FFixedVector2D& A, const FFixedVector2D& B) {
        return (A.X * B.X) + (A.Z * B.Z);
    }
    FFixed_32 Cross(const FFixedVector2D& o) const { return X * o.Z - Z * o.X; }
    FFixed_32 LengthSquared() const { return Dot(*this); }
    static FFixed_32 DistSq(const FFixedVector2D& A, const FFixedVector2D& B) { return Dot(A, B); }
    FFixed_32 Length() const { return LengthSquared().Sqrt(); }
    FFixedVector2D GetSafeNormal() const
    {
        FFixed_32 len = Length();
        return len.v == 0 ? FFixedVector2D{} : *this / len;
    }
};

FORCEINLINE 

FORCEINLINE FVector Fixed2DToVector(const FFixedVector2D& V)
{
    return FVector(FixedToFloat(V.X), 0.f, FixedToFloat(V.Z));
}

FORCEINLINE FVector2D Fixed2DToVector2D(const FFixedVector2D& V)
{
    return FVector2D(FixedToFloat(V.X), FixedToFloat(V.Z));
}

FORCEINLINE FFixedVector2D VectorToFixed2D(const FVector& V)
{
    return FFixedVector2D(FloatToFixed(V.X), FloatToFixed(V.Z));
}

FORCEINLINE FFixedVector2D Vector2DToFixed2D(const FVector2D& V)
{
    return FFixedVector2D(FloatToFixed(V.X), FloatToFixed(V.Y));
}

FORCEINLINE FFixedVector2D operator*(FFixed_32 s, const FFixedVector2D& v)
{
    return { v.X * s, v.Z * s };
}

FORCEINLINE void LogFixedVector2D(const FString& Label, const FFixedVector2D& Vec)
{
    const FVector Converted = Fixed2DToVector(Vec);
    UE_LOG(LogTemp, Warning, TEXT("%s: X=%f, Z=%f"), *Label, Converted.X, Converted.Z);
}

FORCEINLINE FFixed_32 FixedDistance(const FFixedVector2D& A, const FFixedVector2D& B)
{
    return (A - B).Length();
}

FORCEINLINE FFixed_32 FixedDistanceSquared(const FFixedVector2D& A, const FFixedVector2D& B)
{
    return (A - B).LengthSquared();
}
