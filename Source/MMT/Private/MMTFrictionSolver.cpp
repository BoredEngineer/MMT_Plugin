//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTBPFunctionLibrary.h"
#include "MMTFrictionSolver.h"


void UMMTFrictionSolver::Initialize()
{
	UActorComponent* TryParent = CastChecked<UActorComponent>(GetOuter());
	if (IsValid(TryParent))
	{
		ParentComponentRef = TryParent;
		GetNamesForComponentAndParent();

	}
	else
	{
		//Disable component to avoid potential null point reference
		DisableSolver();

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Inner Friction Solver object failed to receive correct parent reference")));
		UE_LOG(LogTemp, Warning, TEXT("Inner Friction Solver object failed to receive correct parent reference"));
	}

}

void UMMTFrictionSolver::GetNamesForComponentAndParent()
{
	//Get names of component and its parent
	ComponentName = ParentComponentRef->GetName();
	ComponentsParentName = ParentComponentRef->GetOwner()->GetName();
}


bool UMMTFrictionSolver::SetPassivePhysicalSurfaceFrictionCoeffiecientsReference(UMMTPhysicalSurfaceFrictionCoefficientsDataAsset* DataAssetRef) 
{
	if (IsValid(DataAssetRef))
	{
		PassivePhysicalSurfaceFrictionCoefficients = DataAssetRef;
		return true;
	}
	else
	{
		return false;
	}
}

bool UMMTFrictionSolver::SetEffectedMeshComponentReference(UMeshComponent* MeshReference)
{
	if (IsValid(MeshReference))
	{
		EffectedComponentMesh = MeshReference;
		return true;
	}
	else
	{
		return false;
	}
}

void AddPassiveFrictionSet(FFrictionSetPassiveStruct NewPassiveFrictionSet)
{
	//NewPassiveFrictionSet.General.ReferenceFrameComponentName
	//PassiveFrictionSets
}