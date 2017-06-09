//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "MMTFrictionSetBase.h"
#include "MMTFrictionSetPassive.generated.h"

USTRUCT(BlueprintType)
struct FFrictionSetPassiveSpecificStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface", meta = (ToolTip = "Static friction coefficient in X axis"))
		float MuXStatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface", meta = (ToolTip = "Kinetic friction coefficient in X axis"))
		float MuXKinetic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface", meta = (ToolTip = "Set TRUE to specify different friction coefficient in Y axis, otherwise MuX is used as uniform coefficient. Useful for objects such as ice skates, brushed contact surface and some types of tank tracks."))
		bool bUseAnisotropicFriction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface", meta = (EditCondition = "bUseAnisotropicFriction", ToolTip = "Static friction coefficient in Y axis"))
		float MuYStatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface", meta = (EditCondition = "bUseAnisotropicFriction", ToolTip = "Kinetic friction coefficient in Y axis"))
		float MuYKinetic;

	FFrictionSetPassiveSpecificStruct()
	{
		MuXStatic = 0.75f;
		MuXKinetic = 0.5f;
		bUseAnisotropicFriction = false;
		MuYStatic = 1.0f;
		MuYKinetic = 0.75f;
	}

};

USTRUCT(BlueprintType)
struct FFrictionSetPassiveStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FFrictionSetBaseStruct General;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FFrictionSetPassiveSpecificStruct Surface;
};



//Passive friction set is for processing surfaces such as ski or similar which don't have active moving surfaces such as wheels or tracks

UCLASS()
class MMT_API UMMTFrictionSetPassive : public UMMTFrictionSetBase
{
	GENERATED_BODY()

public:
	
	// Should be called right after creating instance
	void SetDefaults(FName FrictionSetNameIn, UMeshComponent* EffectedComponentMeshIn, USceneComponent* ReferenceFrameComponentIn, bool bDisabledIn, bool bDebugModeIn,
		float MuXStaticIn, float MuXKineticIn, bool bUseAnisotropicFrictionIn, float MuYStaticIn, float MuYKineticIn)
	{
		FrictionSetName = FrictionSetNameIn;
		EffectedComponentMesh = EffectedComponentMeshIn;
		ReferenceFrameComponent = ReferenceFrameComponentIn;
		bDisabled = bDisabledIn;
		bDebugMode = bDebugModeIn;
		MuXStatic = MuXStaticIn;
		MuXKinetic = MuXKineticIn;
		bUseAnisotropicFriction = bUseAnisotropicFrictionIn;
		MuYStatic = MuYStaticIn;
		MuYKinetic = MuYKineticIn;
	}

private:
	UPROPERTY()
		float MuXStatic;
	
	UPROPERTY()
		float MuXKinetic;

	UPROPERTY()
		bool bUseAnisotropicFriction;
	
	UPROPERTY()
		float MuYStatic;

	UPROPERTY()
		float MuYKinetic;
};