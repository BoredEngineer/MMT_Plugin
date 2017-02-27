//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Engine/EngineTypes.h"
#include "MMTPhysicalSurfaceRollingFrictionCoefficientStruct.generated.h"


USTRUCT(BlueprintType)
struct FPhysicalSurfaceRollingFrictionCoefficientStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction", meta = (ToolTip = "Physical surface for which we want to define rolling friction coefficient"))
	TEnumAsByte<EPhysicalSurface> PhysicalSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction", meta = (ToolTip = "Higher rolling friction coefficient leads to higher rolling friction force"))
	float RollingFrictionCoefficient;
	
	//Default rolling friction is set to zero as to avoid potential confusion in regards to drive train tuning or tuning of friction coefficients
	FPhysicalSurfaceRollingFrictionCoefficientStruct()
	{
		PhysicalSurface = EPhysicalSurface::SurfaceType_Default;
		RollingFrictionCoefficient = 0.0f;
	}

	//constructor with arguments
	FPhysicalSurfaceRollingFrictionCoefficientStruct(EPhysicalSurface Surface, float Coefficient)
	{
		PhysicalSurface = Surface;
		RollingFrictionCoefficient = Coefficient;
	}
};