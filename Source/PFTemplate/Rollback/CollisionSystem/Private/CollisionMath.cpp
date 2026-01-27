#include "CollisionMath.h"

const FFixed_32 FIXED_MAX = FFixed_32::FromRaw(0x7FFFFFFFFFFFFFFFLL); 
const FFixed_32 FIXED_MIN = FFixed_32::FromRaw(-0x7FFFFFFFFFFFFFFFLL);

FFixed_32 CollisionMath::ClosestDistSegmentSegmentSq(FFixedVector2D p1, FFixedVector2D q1, FFixedVector2D p2, FFixedVector2D q2)
{
    FFixedVector2D d1 = q1 - p1; 
    FFixedVector2D d2 = q2 - p2; 
    FFixedVector2D r = p1 - p2;
    FFixed_32 a = d1.LengthSquared(); 
    FFixed_32 e = d2.LengthSquared(); 
    FFixed_32 f = FFixedVector2D::Dot(d2, r);
    FFixed_32 c = FFixedVector2D::Dot(d1, r);
    FFixed_32 b = FFixedVector2D::Dot(d1, d2);
    FFixed_32 denom = a * e - b * b;

    FFixed_32 s = (denom != FFixed_32(0)) ? ((b * f - c * e) / denom) : FFixed_32(0);
    s = (s < FFixed_32(0)) ? FFixed_32(0) : (s > FFixed_32(1) ? FFixed_32(1) : s);

    FFixed_32 t = (b * s + f) / e;
    if (t < FFixed_32(0)) {
        t = FFixed_32(0);
        s = (a > FFixed_32(0)) ? ((-c / a < FFixed_32(0)) ? FFixed_32(0) : (-c / a > FFixed_32(1) ? FFixed_32(1) : -c / a)) : FFixed_32(0);
    } else if (t > FFixed_32(1)) {
        t = FFixed_32(1);
        s = (a > FFixed_32(0)) ? (((b - c) / a < FFixed_32(0)) ? FFixed_32(0) : ((b - c) / a > FFixed_32(1) ? FFixed_32(1) : (b - c) / a)) : FFixed_32(0);
    }

    return ((p1 + d1 * s) - (p2 + d2 * t)).LengthSquared();
}

bool CollisionMath::CapsulesOverlap(const FCapsuleCollision& A, const FCapsuleCollision& B)
{
    // Calculate spines internally
    FFixedVector2D A1 = A.WorldPosition - (A.Capsule.Axis * A.Capsule.HalfLength);
    FFixedVector2D A2 = A.WorldPosition + (A.Capsule.Axis * A.Capsule.HalfLength);
    FFixedVector2D B1 = B.WorldPosition - (B.Capsule.Axis * B.Capsule.HalfLength);
    FFixedVector2D B2 = B.WorldPosition + (B.Capsule.Axis * B.Capsule.HalfLength);

    FFixed_32 DistSq = ClosestDistSegmentSegmentSq(A1, A2, B1, B2);
    FFixed_32 CombinedR = A.Capsule.Radius + B.Capsule.Radius;
    
    return DistSq <= (CombinedR * CombinedR);
}

void CollisionMath::GetProjectionInterval(const FPolygonShape2D& Poly, const FFixedVector2D& Axis, FFixed_32& Min, FFixed_32& Max)
{
    Min = Max = Axis.Dot(Poly.Vertices[0]);
    for (int i = 1; i < Poly.VertexCount; i++) {
        FFixed_32 Projection = Axis.Dot(Poly.Vertices[i]);
        if (Projection < Min) Min = Projection;
        if (Projection > Max) Max = Projection;
    }
}

FSweepResult CollisionMath::SweepPolygonVsPolygon(const FPolygonCollision& MovingPoly, const FFixedVector2D Velocity, const FPolygonCollision& StaticPoly)
{
    FSweepResult Result;
    FFixed_32 tEnter = FIXED_MIN;
    FFixed_32 tLeave = FIXED_MAX;
    FFixedVector2D HitNormal;
    
    auto CheckAxes = [&](const FPolygonShape2D& PolyA, const FPolygonShape2D& PolyB, const FFixedVector2D& RelVel)
    {
        for (int i = 0; i < PolyA.VertexCount; i++) {
            // Get edge normal
            FFixedVector2D p1 = PolyA.Vertices[i];
            FFixedVector2D p2 = PolyA.Vertices[(i + 1) % PolyA.VertexCount];
            FFixedVector2D Edge = p2 - p1;
            FFixedVector2D Normal = FFixedVector2D(-Edge.Z, Edge.X).GetSafeNormal();

            // Project both polygons onto Normal
            FFixed_32 minA, maxA, minB, maxB;
            GetProjectionInterval(PolyA, Normal, minA, maxA);
            GetProjectionInterval(PolyB, Normal, minB, maxB);

            // Project velocity onto Normal
            FFixed_32 vProj = Normal.Dot(RelVel);

            if (vProj == 0) {
                if (minA > maxB || minB > maxA) return false; // Parallel and separated
            } else {
                FFixed_32 t0 = (minB - maxA) / vProj;
                FFixed_32 t1 = (maxB - minA) / vProj;
                if (t0 > t1) Swap(t0, t1);

                if (t0 > tEnter) {
                    tEnter = t0;
                    HitNormal = Normal; // Track the normal of the axis of impact
                }
                if (t1 < tLeave) tLeave = t1;
                if (tEnter > tLeave) return false;
            }
        }
        return true;
    };

    if (CheckAxes(MovingPoly.Polygon, StaticPoly.Polygon, Velocity) && CheckAxes(StaticPoly.Polygon, MovingPoly.Polygon, Velocity)) {
        if (tEnter >= 0 && tEnter <= 1.0f) {
            Result.bHit = true;
            Result.Time = tEnter;
            Result.Normal = HitNormal;
            // Impact point is StartPos + (Velocity * tEnter) 
            // Adjusting by the vertex that caused the collision
            Result.ImpactPoint = MovingPoly.WorldPosition + (Velocity * tEnter); 
        }
    }
    return Result;
}
