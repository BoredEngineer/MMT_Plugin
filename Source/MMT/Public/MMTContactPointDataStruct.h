//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Engine/EngineTypes.h"
#include "MMTContactPointDataStruct.generated.h"


USTRUCT(BlueprintType)
struct FContactPointDataStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component")
	bool IsPointActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component")
	FVector InducedVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component", meta = (ToolTip = "Provide force of the impact at contact point in form of force or an impulse. Collision events provide Normal Impulse, but suspension pushing on the wheel against the ground, provides Force."))
	FVector ImpactForceOrImpulseAtPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component", meta = (ToolTip = "Should we treat ImpactForceOrImpulseAtPoint as impulse instead of force?"))
	bool bTreatAsImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component")
	FVector ContactPointLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component")
	FVector ContactPointNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction Component")
	TEnumAsByte<EPhysicalSurface> PhysicalSurface;
		

	//Default constructor
	FContactPointDataStruct()
	{
		IsPointActive = false;
		InducedVelocity = FVector::ZeroVector;
		ImpactForceOrImpulseAtPoint = FVector::ZeroVector;
		bTreatAsImpulse = false;
		ContactPointLocation = FVector::ZeroVector;
		ContactPointNormal = FVector::ZeroVector;
		PhysicalSurface = EPhysicalSurface::SurfaceType_Default;
	}

};