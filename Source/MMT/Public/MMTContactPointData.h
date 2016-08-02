//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Engine/EngineTypes.h"
#include "MMTContactPointData.generated.h"


USTRUCT(BlueprintType)
struct FContactPointData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component")
	bool IsPointActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component")
	FVector InducedVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component")
	FVector NormalImpulseAtPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component")
	FVector ContactPointLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component")
	FVector ContactPointNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component")
	TEnumAsByte<EPhysicalSurface> PhysicalSurface;
		

	//Default constructor
	FContactPointData()
	{
		IsPointActive = false;
		InducedVelocity = FVector::ZeroVector;
		NormalImpulseAtPoint = FVector::ZeroVector;
		ContactPointLocation = FVector::ZeroVector;
		ContactPointNormal = FVector::ZeroVector;
		PhysicalSurface = EPhysicalSurface::SurfaceType_Default;
	}

};