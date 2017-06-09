//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/ActorComponent.h"
#include "MMTVehicleSuspensionStructs.h"
#include "MMTTrack.h"
#include "MMTVehicleSuspensionComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MMT_API UMMTVehicleSuspensionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UMMTVehicleSuspensionComponent(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnComponentCreated() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Disables component from doing physics calculations and applying forces, useful for debugging"))
		bool bDisabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Enable on screen debug output"))
		bool bDebugMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Name of the mesh component that will receive forces, usually chassis/root component"))
		FString EffectedComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Set this up if vehicle needs caterpillar tracks"))
		FTracksConfig TracksConfiguration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Set this up if vehicle needs wheels"))
		FWheelsConfig WheelsConfiguration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Set this up if vehicle needs surfaces such as ski"))
		FPassiveSurfaceConfig PassiveSurfacesConfiguration;

	UFUNCTION(BlueprintCallable, Category = "MMT Vehicle Suspension Component")
		UMeshComponent* GetEffectedComponentMeshReference() { return EffectedComponentMesh; }

	UFUNCTION(BlueprintCallable, Category = "MMT Vehicle Suspension Component")
		void UpdatePhysics(float DeltaTime);

private:

	//Mesh reference that we will apply our suspension and traction forces too. 
	UPROPERTY()
		UMeshComponent* EffectedComponentMesh;

	UPROPERTY()
		TArray<UMMTTrack*> TrackCollection;

	//Get references for named components
	void GetNamedComponentsReference();
};
