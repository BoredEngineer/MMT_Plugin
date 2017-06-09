//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "MMTPhysicalSurfaceFrictionCoefficient.h"
#include "MMTFrictionSetBase.generated.h"


USTRUCT(BlueprintType)
struct FFrictionSetBaseStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Name of this friction set for debugging purposes and improving usability"))
		FName FrictionSetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Name of the mesh component that will be used as frame of reference for world space position and rotations"))
		FString ReferenceFrameComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Disables item from physics calculations and applying forces, useful for debugging."))
		bool bDisabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Enable on screen debug output"))
		bool bDebugMode;
	
	FFrictionSetBaseStruct()
	{
		FrictionSetName = FName(TEXT("MyFrictionSetName"));
		ReferenceFrameComponentName = FString("None");
		bDisabled = false;
		bDebugMode = false;
	}
};

UCLASS(abstract) //, DefaultToInstanced, EditInlineNew, abstract, collapsecategories, hidecategories = Object)
class MMT_API UMMTFrictionSetBase : public  UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
		FName FrictionSetName;
	
	UPROPERTY()
		UMeshComponent* EffectedComponentMesh;
	
	UPROPERTY()
		USceneComponent* ReferenceFrameComponent;

	UPROPERTY()
		bool bDisabled;
	
	UPROPERTY()
		bool bDebugMode;

private:
};
