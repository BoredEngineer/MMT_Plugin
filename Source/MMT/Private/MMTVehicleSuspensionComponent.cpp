//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTBPFunctionLibrary.h"
#include "MMTVehicleSuspensionComponent.h"


// Sets default values for this component's properties
UMMTVehicleSuspensionComponent::UMMTVehicleSuspensionComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	bDisabled = false;
	bDebugMode = false;
	EffectedComponentName = FString("Chassis");
}


// Called when the game starts
void UMMTVehicleSuspensionComponent::BeginPlay()
{
	Super::BeginPlay();

	GetNamedComponentsReference();

	//Update effected components reference
	if (IsValid(EffectedComponentMesh))
	{
		for (UMMTTrack* Track : TrackCollection)
		{
			Track->SetEffectedMeshReference(EffectedComponentMesh);
			//toggle debug
			Track->ToggleDebugMode();
		}
	}
}


void UMMTVehicleSuspensionComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	//Create tracks
	for (FTrackSets TrackSet : TracksConfiguration.TrackSets)
	{
		if (IsValid(TrackSet.TrackClass))
		{
			TrackCollection.Add(NewObject<UMMTTrack>(this, TrackSet.TrackClass));
		}
	}
	//Initialize tracks
	for (UMMTTrack* Track : TrackCollection)
	{
		Track->CreateSubObjects();
	}
}


//Find and store reference to named components
void UMMTVehicleSuspensionComponent::GetNamedComponentsReference()
{
	//Find reference for effected component
	if (EffectedComponentName != FString("None"))
	{
		
		EffectedComponentMesh = UMMTBPFunctionLibrary::GetComponentReferenceByNameAndClass<UMeshComponent>(this, EffectedComponentName);
		if (!IsValid(EffectedComponentMesh))
		{
			EffectedComponentMesh = nullptr;
			bDisabled = true;
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *GetOwner()->GetName(), *GetName(), *EffectedComponentName));
			UE_LOG(LogTemp, Warning, TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *GetOwner()->GetName(), *GetName(), *EffectedComponentName);
		}
	}
	else
	{
		EffectedComponentMesh = nullptr;
		bDisabled = true;
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's EffectedComponentMesh property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName()));
		UE_LOG(LogTemp, Warning, TEXT("%s->%s component's EffectedComponentName property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName());
	}
}


// Called when the game starts
void UMMTVehicleSuspensionComponent::UpdatePhysics(float DeltaTime)
{
	//Initialize tracks
	for (UMMTTrack* Track : TrackCollection)
	{
		Track->UpdatePhysics(DeltaTime);
	}
}