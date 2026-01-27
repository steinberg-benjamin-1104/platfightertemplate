#pragma once
#include "CollisionDefinitions.h"

class CollisionMath
{
public:
	
	static bool CapsulesOverlap(const FCapsuleCollision& A, const FCapsuleCollision& B);
	static FSweepResult SweepPolygonVsPolygon(
		const FPolygonCollision& MovingPoly, const FFixedVector2D Velocity, const FPolygonCollision& StaticPoly);

protected:
	// The core "Combat" math (Capsule vs Capsule)
	static FFixed_32 ClosestDistSegmentSegmentSq(FFixedVector2D p1, FFixedVector2D q1, FFixedVector2D p2, FFixedVector2D q2);

private:
	static void GetProjectionInterval(const FPolygonShape2D& Poly, const FFixedVector2D& Axis, FFixed_32& Min, FFixed_32& Max);
};