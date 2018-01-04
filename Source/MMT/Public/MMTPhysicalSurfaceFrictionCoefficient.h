//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Engine/EngineTypes.h"
#include "MMTPhysicalSurfaceFrictionCoefficient.generated.h"


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


USTRUCT(BlueprintType)
struct FMMTPhysicalSurfaceFrictionCoefficients
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Physical surface for which we define friction properties"))
		TEnumAsByte<EPhysicalSurface> PhysicalSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Scale of the friction coefficient in respect to surface type. For example Ice would have smaller friction such as 0.1, while asphalt would improve friction with scale of 1.2"))
		float FrictionScaleFactor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Higher rolling friction coefficient leads to higher rolling friction force"))
		float RollingFrictionCoefficient;

	FMMTPhysicalSurfaceFrictionCoefficients()
	{
		PhysicalSurface = EPhysicalSurface::SurfaceType_Default;
		FrictionScaleFactor = 1.0f;
		//Default rolling friction is set to zero as to avoid potential confusion in regards to drive train tuning or tuning of friction coefficients
		RollingFrictionCoefficient = 0.0f;
	}

	//constructor with arguments
	FMMTPhysicalSurfaceFrictionCoefficients(EPhysicalSurface Surface, float FrictionScale, float RollingFrictionCoefficientIn)
	{
		PhysicalSurface = Surface;
		FrictionScaleFactor = FrictionScale;
		RollingFrictionCoefficient = RollingFrictionCoefficientIn;
	}
};


UCLASS(BlueprintType)
class UMMTPhysicalSurfaceFrictionCoefficientsDataAsset : public  UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Friction coefficients for different physical surfaces"))
		TArray<FMMTPhysicalSurfaceFrictionCoefficients> PhysicalSurfaceFrictionCoefficients;
	
};