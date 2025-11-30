#pragma once
#include "CoreMinimal.h"
#include "Fixed32.h"
#include <stdint.h>
#include "FixedVector2D.generated.h"

/* 2-D fixed-point vector for XZ plane (fighting game) */
struct FFixedVector2D
{
    FIXED_32 X;
    FIXED_32 Z;

    //construction
    FFixedVector2D() = default;
    FFixedVector2D(FIXED_32 x, FIXED_32 z) : X(x), Z(z) {}
    FFixedVector2D(FIXED_32 x, float z) : X(x), Z(z) {}
    FFixedVector2D(float x, FIXED_32 z) : X(x), Z(z) {}
    FFixedVector2D(float x, float z) : X(x), Z(z) {}

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
    FFixedVector2D GetSafeNormal() const
    {
        FIXED_32 len = Length();
        return len.v == 0 ? FFixedVector2D{} : *this / len;
    }
    
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

USTRUCT(BlueprintType)
struct FVec2TV
{
    GENERATED_BODY()

    // These are what the data table will expose
    UPROPERTY(EditAnywhere)
    float X = 0.f;

    UPROPERTY(EditAnywhere)
    float Z = 0.f;

    // Convert to your fixed vector
    FFixedVector2D ToFixed() const
    {
        return FFixedVector2D(X, Z);
    }
};

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

FORCEINLINE FFixedVector2D operator*(FIXED_32 s, const FFixedVector2D& v)
{
    return { v.X * s, v.Z * s };
}