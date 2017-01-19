//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/SceneComponent.h"
#include "MMTTrackSplinePointAnimated.h"
#include "MMTTrackSuspensionStack.h"
#include "MMTTrackSprocketAndIdler.h"
#include "MMTTrackSimulationComponent.generated.h"


UCLASS(ClassGroup = (MMT), meta = (BlueprintSpawnableComponent))
class MMT_API UMMTTrackSimulationComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMMTTrackSimulationComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension", meta = (ToolTip = "Array of suspension stacks and road-wheels"))
		TArray<FTrackSuspensionStack> SuspensionStacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension", meta = (ToolTip = "Array of sprockets and idlers"))
		TArray<FTrackSprocketAndIdler> SprocketsAndIdlers;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation (optional)", meta = (ToolTip = "Will use Instanced Static Mesh component to visualize treads of the track. Otherwise user can use animated UV material and skinned mesh for this purpose"))
		uint32 IsUseISMtreads : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation (optional)", meta = (EditCondition = "IsUseISMtreads", ToolTip = "Spline component that defines path of the track for simple animation"))
		FString TrackSplineSimpleComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation (optional)", meta = (EditCondition = "IsUseISMtreads", ToolTip = "Instanced mesh component for rendering treads"))
		FString TreadsInstancedMeshComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation (optional)", meta = (EditCondition = "IsUseISMtreads", ToolTip = "Array of spline point indexes and references for their animation"))
		TArray<FTrackSplinePointAnimated> AnimatedTrackSplinePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation (optional)", meta = (EditCondition = "IsUseISMtreads", ToolTip = "Amount of treads on this track"))
		int32 TreadsOnSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation (optional)", meta = (EditCondition = "IsUseISMtreads", ToolTip = "Plays animation in reverse if meshes are rotated 180 degrees"))
		bool IsFlipAnimation;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Simulation (optional)", meta = (ToolTip = "Will simulate tracks as main source of traction and obstacle agility. In this mode, road-wheels will roll on the tracks"))
		uint32 IsSimulateTracks : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Simulation (optional)", meta = (EditCondition = "IsSimulateTracks", ToolTip = "Amount of Verlet integration iterations for resolving movement of the track. More iterations gives smoother animation but costs performance"))
		int32 NumberOfVerletIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Simulation (optional)", meta = (EditCondition = "IsSimulateTracks", ToolTip = "Spline component that defines default setup of the simulated track"))
		FString TrackSplineAdvancedComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Misc", meta = (ToolTip = "Enables on screen debug output"))
		bool IsDebugMode;

};
