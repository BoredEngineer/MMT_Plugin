//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTSuspensionStackComponent.h"


// Sets default values for this component's properties
UMMTSuspensionStackComponent::UMMTSuspensionStackComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//SuspensionStack = ObjectInitializer.CreateDefaultSubobject<UMMTSuspensionStack>(this, TEXT("SuspensionStack"));
	SuspensionStack = CreateDefaultSubobject<UMMTSuspensionStack>(TEXT("SuspensionStack"));
}


// Called when the game starts
void UMMTSuspensionStackComponent::BeginPlay()
{
	Super::BeginPlay();

	//Initialize suspension stack
	SuspensionStack->Initialize();
}


