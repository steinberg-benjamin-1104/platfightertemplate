#include "CollisionMath.h"

FFixedVector2D CollisionMath::GetClosestPointOnSegment(const FFixedVector2D& P, const FFixedVector2D& A, const FFixedVector2D& B) {
    FFixedVector2D AB = B - A;
    FFixed_32 AbSq = AB.LengthSquared();
    if (AbSq <= FFixed_32(0)) return A;

    FFixed_32 t = FFixedVector2D::Dot(P - A, AB) / AbSq;
    t = (t < FFixed_32(0)) ? FFixed_32(0) : (t > FFixed_32(1) ? FFixed_32(1) : t);
    return A + (AB * t);
}

FFixed_32 CollisionMath::ClosestDistSegmentSegmentSq(FFixedVector2D p1, FFixedVector2D q1, FFixedVector2D p2, FFixedVector2D q2) {
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

void CollisionMath::ProjectPolygon(const FCollisionComponent& Poly, const FFixedVector2D& Axis, FFixed_32& OutMin, FFixed_32& OutMax) {
    OutMin = FFixedVector2D::Dot(Axis, Poly.WorldPosition + Poly.Polygon.Vertices[0]);
    OutMax = OutMin;
    for (int i = 1; i < Poly.Polygon.VertexCount; ++i) {
        FFixed_32 Proj = FFixedVector2D::Dot(Axis, Poly.WorldPosition + Poly.Polygon.Vertices[i]);
        if (Proj < OutMin) OutMin = Proj;
        if (Proj > OutMax) OutMax = Proj;
    }
}

bool CollisionMath::CheckPolyPolyOverlap(const FCollisionComponent& A, const FCollisionComponent& B) {
    auto TestAxes = [&](const FCollisionComponent& P1, const FCollisionComponent& P2) {
        for (int i = 0; i < P1.Polygon.VertexCount; ++i) {
            FFixedVector2D V1 = P1.Polygon.Vertices[i];
            FFixedVector2D V2 = P1.Polygon.Vertices[(i + 1) % P1.Polygon.VertexCount];
            FFixedVector2D Edge = V2 - V1;
            FFixedVector2D Axis = FFixedVector2D(-Edge.Z, Edge.X).GetSafeNormal();

            FFixed_32 MinA, MaxA, MinB, MaxB;
            ProjectPolygon(P1, Axis, MinA, MaxA);
            ProjectPolygon(P2, Axis, MinB, MaxB);
            if (MaxA < MinB || MaxB < MinA) return false;
        }
        return true;
    };
    return TestAxes(A, B) && TestAxes(B, A);
}

bool CollisionMath::CapsulesOverlap(const FCollisionComponent& A, const FCollisionComponent& B) {
    // Calculate spines internally
    FFixedVector2D A1 = A.WorldPosition - (A.Capsule.Axis * A.Capsule.HalfLength);
    FFixedVector2D A2 = A.WorldPosition + (A.Capsule.Axis * A.Capsule.HalfLength);
    FFixedVector2D B1 = B.WorldPosition - (B.Capsule.Axis * B.Capsule.HalfLength);
    FFixedVector2D B2 = B.WorldPosition + (B.Capsule.Axis * B.Capsule.HalfLength);

    FFixed_32 DistSq = ClosestDistSegmentSegmentSq(A1, A2, B1, B2);
    FFixed_32 CombinedR = A.Capsule.Radius + B.Capsule.Radius;
    
    return DistSq <= (CombinedR * CombinedR);
}

bool CollisionMath::PolygonsOverlap(const FCollisionComponent& A, const FCollisionComponent& B) {
    return CheckPolyPolyOverlap(A, B);
}