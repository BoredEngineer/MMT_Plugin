//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/ActorComponent.h"
#include "MMTFrictionSolver.h"
#include "MMTFrictionSetPassive.h"
#include "MMTFrictionSolverComponent.generated.h"


USTRUCT(BlueprintType)
struct FPassiveFrictionSetSettings
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Set to true if vehicle has friction surfaces such as ski."))
		bool bHasPassives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (EditCondition = "bHasSurfaces", ToolTip = "Reference data asset of physical surface friction coefficients"))
		UMMTPhysicalSurfaceFrictionCoefficientsDataAsset* PassivePhysicalSurfaceFrictionCoefficients;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (EditCondition = "bHasSurfaces", ToolTip = "Surface friction sets are entities such as ski or just regular surfaces. For example, aerosled with 4 ski will have 4 sets of surfaces."))
		TArray<FFrictionSetPassiveStruct> PassiveFrictionSets;

};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MMT_API UMMTFrictionSolverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMMTFrictionSolverComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Disables component from doing physics calculations and applying forces, useful for debugging"))
		bool bDisabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Enable on screen debug output"))
		bool bDebugMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Name of the mesh component that will receive friction force, usually chassis/root component"))
		FString EffectedComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Settings of the friction surfaces"))
		FPassiveFrictionSetSettings PassiveSetSettings;

	UPROPERTY(VisibleAnywhere, Instanced, BlueprintReadWrite)
		UMMTFrictionSolver* FrictionSolver;

	// Called when the game starts
	virtual void BeginPlay() override;

private:
	//Get references for named components
	void GetNamedComponentsReference();

	//Populate Surface friction sets using structures populated by designer
	void PopulatePassiveSetFromStructure();

};
