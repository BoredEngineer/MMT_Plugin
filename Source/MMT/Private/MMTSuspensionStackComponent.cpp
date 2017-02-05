//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTSuspensionStackComponent.h"


// Sets default values for this component's properties
UMMTSuspensionStackComponent::UMMTSuspensionStackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SuspensionStack = CreateDefaultSubobject<UMMTSuspensionStack, UMMTSuspensionStack>(TEXT("SuspensionStack"));
}


// Called when the game starts
void UMMTSuspensionStackComponent::BeginPlay()
{
	Super::BeginPlay();

	//Pass own reference to Suspension Stack as it's not capable of getting it's parent on its own
	SuspensionStack->SetParentComponentReference(this);
	//Initialize suspension stack
	SuspensionStack->Initialize();
	
}


