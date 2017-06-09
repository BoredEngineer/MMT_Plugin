//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTTrack.h"


//Create sub-objects
void UMMTTrack::CreateSubObjects() 
{
	//Create suspension stacks
	for (FSuspensionStackStruct SuspStruct : SuspendedWheels)
	{
		UMMTSuspensionStack* NewStack = NewObject<UMMTSuspensionStack>(this);
		NewStack->SuspensionSettings = SuspStruct;
		NewStack->SprungComponentName = EffectedComponentName;
		SuspendedWheelsRef.Add(NewStack);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s Suspension Stack created"), *GetName()));
	}

}

void UMMTTrack::OnComponentCreated()
{
	Super::OnComponentCreated();
}

// Called when the game starts
void UMMTTrack::BeginPlay()
{
	Super::BeginPlay();

	//GetNamedComponentsReference();

	CreateSubObjects();

	for (UMMTSuspensionStack* SuspStack : SuspendedWheelsRef)
	{
		SuspStack->Initialize();
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s Reporting"), *GetName()));
	}
}

void UMMTTrack::SetEffectedMeshReference(UMeshComponent* EffectedMeshComponentRef)
{
	for (UMMTSuspensionStack* SuspStack : SuspendedWheelsRef)
	{
		SuspStack->SetSprungComponentReference(EffectedMeshComponentRef);
	}
}

void UMMTTrack::UpdatePhysics(float DeltaTime)
{
	for (UMMTSuspensionStack* SuspStack : SuspendedWheelsRef)
	{
		SuspStack->PhysicsUpdate(DeltaTime);
	}
}

void UMMTTrack::ToggleDebugMode()
{
	for (UMMTSuspensionStack* SuspStack : SuspendedWheelsRef)
	{
		SuspStack->ToggleDebugMode();
	}
}

