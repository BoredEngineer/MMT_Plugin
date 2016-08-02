//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Engine/EngineTypes.h"
#include "MMTPhysicalSurfaceRollingFrictionCoefficient.generated.h"


USTRUCT(BlueprintType)
struct FPhysicalSurfaceRollingFrictionCoefficient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction", meta = (ToolTip = "Physical surface for which we want to define rolling friction coefficient"))
	TEnumAsByte<EPhysicalSurface> PhysicalSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Friction", meta = (ToolTip = "Higher rolling friction coefficient leads to higher rolling friction force"))
	float RollingFrictionCoefficient;
	
	//default constructor
	FPhysicalSurfaceRollingFrictionCoefficient()
	{
		PhysicalSurface = EPhysicalSurface::SurfaceType_Default;
		RollingFrictionCoefficient = 0.02f;
	}

	//constructor with arguments
	FPhysicalSurfaceRollingFrictionCoefficient(EPhysicalSurface Surface, float Coefficient)
	{
		PhysicalSurface = Surface;
		RollingFrictionCoefficient = Coefficient;
	}
};