#include "Procedural2DCapsuleComponent.h"
#include "KismetProceduralMeshLibrary.h" // Needed for normal/tangent calculation

UProcedural2DCapsuleComponent::UProcedural2DCapsuleComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UProcedural2DCapsuleComponent::OnRegister()
{
    Super::OnRegister();
    RebuildCapsule();
}

void UProcedural2DCapsuleComponent::SetMaterialSet(const FHitboxMaterialSet& MaterialSet)
{
    if (MaterialSet.FillMaterial)
    {
        UMaterialInstanceDynamic* FillDyn = UMaterialInstanceDynamic::Create(MaterialSet.FillMaterial, this);
        if (FillDyn)
        {
            SetMaterial(0, FillDyn);
        }
    }

    if (MaterialSet.OutlineMaterial)
    {
        UMaterialInstanceDynamic* OutlineDyn = UMaterialInstanceDynamic::Create(MaterialSet.OutlineMaterial, this);
        if (OutlineDyn)
        {
            SetMaterial(1, OutlineDyn);
        }
    }
}

void UProcedural2DCapsuleComponent::RebuildCapsule()
{
    ClearAllMeshSections();

    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FColor> VertexColors;
    TArray<FProcMeshTangent> Tangents;

    const float AngleStep = PI / CircleSegments;
    TArray<FVector> OutlinePoints;

    bool bIsCircle = CapsuleSize.Y <= KINDA_SMALL_NUMBER;

    if (bIsCircle)
    {
        for (int32 i = 0; i <= CircleSegments * 2; ++i)
        {
            float Angle = i * (PI / CircleSegments);
            float X = FMath::Cos(Angle) * CapsuleSize.X;
            float Z = FMath::Sin(Angle) * CapsuleSize.X;
            OutlinePoints.Add(FVector(X, 0, Z));
        }
    }
    else
    {
        const FVector LeftCenter = FVector(-CapsuleSize.Y, 0, 0);
        const FVector RightCenter = FVector(CapsuleSize.Y, 0, 0);

        for (int32 i = 0; i <= CircleSegments; ++i)
        {
            float Angle = HALF_PI - i * AngleStep;
            float X = -FMath::Cos(Angle) * CapsuleSize.X;
            float Z = FMath::Sin(Angle) * CapsuleSize.X;
            OutlinePoints.Add(LeftCenter + FVector(X, 0, Z));
        }

        for (int32 i = 0; i <= CircleSegments; ++i)
        {
            float Angle = -HALF_PI + i * AngleStep;
            float X = FMath::Cos(Angle) * CapsuleSize.X;
            float Z = FMath::Sin(Angle) * CapsuleSize.X;
            OutlinePoints.Add(RightCenter + FVector(X, 0, Z));
        }
    }

    Vertices.Add(FVector(0, 0, 0)); // Center vertex

    for (const FVector& P : OutlinePoints)
    {
        Vertices.Add(P);
    }

    for (const FVector& V : Vertices)
    {
        float U = (V.X + (CapsuleSize.Y + CapsuleSize.X)) / (2.0f * (CapsuleSize.Y + CapsuleSize.X));
        float VCoord = (V.Z + CapsuleSize.X) / (2.0f * CapsuleSize.X);
        UVs.Add(FVector2D(U, VCoord));
        VertexColors.Add(FColor::White);
    }

    for (int32 i = 1; i <= OutlinePoints.Num(); ++i)
    {
        int32 Curr = i;
        int32 Next = (i % OutlinePoints.Num()) + 1;

        Triangles.Add(0);
        Triangles.Add(Curr);
        Triangles.Add(Next);
    }

    Normals.Init(FVector::ZeroVector, Vertices.Num());
    Tangents.Init(FProcMeshTangent(), Vertices.Num());
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);

    CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);

    // === Outline Section ===
    TArray<FVector> InnerRing = OutlinePoints;
    TArray<FVector> OuterRing;

    for (int32 i = 0; i < OutlinePoints.Num(); ++i)
    {
        // Find 2D outward direction
        float CapCenterX = OutlinePoints[i].X < 0 ? -CapsuleSize.Y : CapsuleSize.Y;
        FVector2D Center2D(CapCenterX, 0);
        FVector2D Point2D(OutlinePoints[i].X, OutlinePoints[i].Z);
        FVector2D Outward2D = (Point2D - Center2D).GetSafeNormal();
        FVector Outward3D = FVector(Outward2D.X, 0, Outward2D.Y);

        InnerRing[i] += FVector(0.1f, 0, 0); // Offset toward normal for clean outline
        OuterRing.Add(OutlinePoints[i] + Outward3D * OutlineThickness + FVector(0.1f, 0, 0));
    }

    TArray<FVector> OutlineVerts;
    OutlineVerts.Append(InnerRing);
    OutlineVerts.Append(OuterRing);

    TArray<int32> OutlineTriangles;
    int32 NumPoints = InnerRing.Num();

    for (int32 i = 0; i < NumPoints; ++i)
    {
        int32 Next = (i + 1) % NumPoints;

        OutlineTriangles.Add(i);
        OutlineTriangles.Add(i + NumPoints);
        OutlineTriangles.Add(Next);

        OutlineTriangles.Add(Next);
        OutlineTriangles.Add(i + NumPoints);
        OutlineTriangles.Add(Next + NumPoints);
    }

    TArray<FVector> OutlineNormals;
    TArray<FVector2D> OutlineUVs;
    TArray<FColor> OutlineColors;
    TArray<FProcMeshTangent> OutlineTangents;

    for (const FVector& V : OutlineVerts)
    {
        OutlineUVs.Add(FVector2D(0, 0));
        OutlineColors.Add(FColor::White);
    }

    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(OutlineVerts, OutlineTriangles, OutlineUVs, OutlineNormals, OutlineTangents);

    CreateMeshSection(1, OutlineVerts, OutlineTriangles, OutlineNormals, OutlineUVs, OutlineColors, OutlineTangents, true);

    // === Apply Materials ===
    UMaterialInterface* FillMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Path/To/M_CapsuleFill.M_CapsuleFill"));
    if (FillMat)
    {
        UMaterialInstanceDynamic* FillDyn = UMaterialInstanceDynamic::Create(FillMat, this);
        if (FillDyn)
        {
            SetMaterial(0, FillDyn);
        }
    }

    UMaterialInterface* OutlineMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Path/To/M_CapsuleOutline.M_CapsuleOutline"));
    if (OutlineMat)
    {
        UMaterialInstanceDynamic* LineDyn = UMaterialInstanceDynamic::Create(OutlineMat, this);
        if (LineDyn)
        {
            SetMaterial(1, LineDyn);
        }
    }

    SetMeshSectionVisible(1, true);
}

#if WITH_EDITOR
void UProcedural2DCapsuleComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropertyName = PropertyChangedEvent.GetPropertyName();

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UProcedural2DCapsuleComponent, CapsuleSize.X) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UProcedural2DCapsuleComponent, CapsuleSize.Y) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UProcedural2DCapsuleComponent, CircleSegments))
    {
        RebuildCapsule();
    }
}
#endif