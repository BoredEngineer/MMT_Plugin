//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/SceneComponent.h"
#include "MMTSuspensionStack.h"
#include "MMTSuspensionStackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMT_API UMMTSuspensionStackComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMMTSuspensionStackComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, Instanced, BlueprintReadWrite)
	UMMTSuspensionStack* SuspensionStack;

	// Called when the game starts
	virtual void BeginPlay() override;

};
