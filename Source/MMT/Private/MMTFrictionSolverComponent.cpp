//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTBPFunctionLibrary.h"
#include "MMTFrictionSolverComponent.h"


// Sets default values for this component's properties
UMMTFrictionSolverComponent::UMMTFrictionSolverComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	FrictionSolver = CreateDefaultSubobject<UMMTFrictionSolver>(TEXT("FrictionSolver"));

	bDisabled = false;
	bDebugMode = false;
	EffectedComponentName = FString("Chassis");
}


// Called when the game starts
void UMMTFrictionSolverComponent::BeginPlay()
{
	Super::BeginPlay();

	//Initialize FrictionSolver
	FrictionSolver->Initialize();
	
	GetNamedComponentsReference();
	PopulatePassiveSetFromStructure();
}

//Find and store reference to named components
void UMMTFrictionSolverComponent::GetNamedComponentsReference()
{
	//Find reference for effected component
	if (EffectedComponentName != FString("None"))
	{
		if(FrictionSolver->SetEffectedMeshComponentReference(UMMTBPFunctionLibrary::GetMeshComponentReferenceByName(this, EffectedComponentName)))
		{}
		else
		{
			FrictionSolver->DisableSolver();
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *GetOwner()->GetName(), *GetName(), *EffectedComponentName));
			UE_LOG(LogTemp, Warning, TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *GetOwner()->GetName(), *GetName(), *EffectedComponentName);
		}
	}
	else
	{
		FrictionSolver->DisableSolver();
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's EffectedComponentMesh property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName()));
		UE_LOG(LogTemp, Warning, TEXT("%s->%s component's EffectedComponentName property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName());
	}
}

void UMMTFrictionSolverComponent::PopulatePassiveSetFromStructure()
{
	//Check if designer enabled passive friction sets
	if (PassiveSetSettings.bHasPassives)
	{
		if(FrictionSolver->SetPassivePhysicalSurfaceFrictionCoeffiecientsReference(PassiveSetSettings.PassivePhysicalSurfaceFrictionCoefficients))
		{

			for (FFrictionSetPassiveStruct Surface : PassiveSetSettings.PassiveFrictionSets)
			{
				//Check name references first
				USceneComponent* RefFrameComponent = UMMTBPFunctionLibrary::GetComponentReferenceByNameAndClass<USceneComponent>(this, Surface.General.ReferenceFrameComponentName);
				if (IsValid(RefFrameComponent))
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s"), *RefFrameComponent->GetName()));
				}
				//Surface.General.ReferenceFrameComponentName
			}
		}
		else
		{
			FrictionSolver->DisableSolver();
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s PassivePhysicalSurfaceFrictionCoefficients can't be null, select proper data asset to be used as reference"), *GetOwner()->GetName(), *GetName()));
			UE_LOG(LogTemp, Warning, TEXT("%s->%s PassivePhysicalSurfaceFrictionCoefficients can't be null, select proper data asset to be used as reference"), *GetOwner()->GetName(), *GetName());
		}
	}
}