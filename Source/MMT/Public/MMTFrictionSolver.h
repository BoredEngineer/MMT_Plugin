//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "UObject/NoExportTypes.h"
#include "MMTFrictionSetPassive.h"
#include "MMTPhysicalSurfaceFrictionCoefficient.h"
#include "MMTFrictionSolver.generated.h"


UCLASS()
class MMT_API UMMTFrictionSolver : public UObject
{
	GENERATED_BODY()
	
public:

	// Initializes class logic, finds references and pre-calculates parameters. Expected to be called from BeginPlay only
	UFUNCTION(BlueprintCallable, Category = "MMT Friction Solver")
		void Initialize();

	// Sets reference to a mesh component which will be effected by friction forces
	bool SetEffectedMeshComponentReference(UMeshComponent* MeshReference);

	// Disables solver from calculating friction forces and applying them
	void DisableSolver()
	{
		bDisabled = true;
	}

	// Sets reference to Data Asset that describes friction coefficients in regards to specific physical surfaces
	bool SetPassivePhysicalSurfaceFrictionCoeffiecientsReference(UMMTPhysicalSurfaceFrictionCoefficientsDataAsset* DataAssetRef);

	void AddPassiveFrictionSet(FFrictionSetPassiveStruct NewPassiveFrictionSet);

private:
	UPROPERTY()
		bool bDisabled;
	UPROPERTY()
		bool bDebugMode;

	UPROPERTY()
		UActorComponent* ParentComponentRef;

	UPROPERTY()
		UMeshComponent* EffectedComponentMesh;

	UPROPERTY()
		FString ComponentName = FString("ComponentRefereneFailed");
	UPROPERTY()
		FString ComponentsParentName = FString("ParentRefereneFailed");

	UPROPERTY()
		UMMTPhysicalSurfaceFrictionCoefficientsDataAsset* PassivePhysicalSurfaceFrictionCoefficients;

	UPROPERTY()
		TArray<UMMTFrictionSetPassive*> PassiveFrictionSets;

	//Get names of the component storing instance of this class and name of owner of component
	void GetNamesForComponentAndParent();

	void AddPassiveFrictionSet()
	{
	}
};
