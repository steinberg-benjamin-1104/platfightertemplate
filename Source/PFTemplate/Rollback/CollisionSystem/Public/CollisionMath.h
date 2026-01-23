#pragma once
#include "CollisionDefinitions.h"

class CollisionMath
{
public:
	
	static bool CapsulesOverlap(const FCollisionComponent& A, const FCollisionComponent& B);
	static bool PolygonsOverlap(const FCollisionComponent& A, const FCollisionComponent& B);

protected:
	// Basic Point/Segment helpers
	static FFixedVector2D GetClosestPointOnSegment(const FFixedVector2D& P, const FFixedVector2D& A, const FFixedVector2D& B);
    
	// The core "Combat" math (Capsule vs Capsule)
	static FFixed_32 ClosestDistSegmentSegmentSq(FFixedVector2D p1, FFixedVector2D q1, FFixedVector2D p2, FFixedVector2D q2);
    
	// The core "Environment" math (Polygon vs Polygon)
	static void ProjectPolygon(const FCollisionComponent& Poly, const FFixedVector2D& Axis, FFixed_32& OutMin, FFixed_32& OutMax);
	static bool CheckPolyPolyOverlap(const FCollisionComponent& A, const FCollisionComponent& B);
	
};