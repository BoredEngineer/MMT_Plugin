//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/MeshComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/EngineTypes.h"
#include "MMTTrackSuspensionStack.generated.h"


USTRUCT(BlueprintType)
struct FTrackSuspensionStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack", meta = (ToolTip = "Will use shape traces to find suspension compression. If False will use ray-check for suspension. Ray-check provides better performance but suitable only for driving over smooth terrain and will make it very difficult to get over vertical obstacles"))
		uint32 IsShapeTrace : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack", meta = (EditCondition = "IsShapeTrace", ToolTip = "Name of the mesh component that will be used for sweep of this suspension stack"))
		FString SweepShapeComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack", meta = (ToolTip = "Position of the stack in relation component's root. Normally a point where suspension would be physically attached to the chassis"))
		FVector StackPosition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack", meta = (ToolTip = "Radius of the road-wheel in cm. Needed for when track is using simulation feature or when IsShapeTrace is off and ray-check is used instead of sweeps"))
		float RoadWheelRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack", meta = (ToolTip = "Top limit of the spring in relation to suspension stack position. Doesn't have to be aligned with Z axis. In case of torsion bar suspension, spring can be angled to follow closer radial movement of suspension"))
		FVector SpringTopOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack", meta = (ToolTip = "Bottom limit of the spring in relation to suspension stack position. Doesn't have to be aligned with Z axis. In case of torsion bar suspension, spring can be angled to follow closer radial movement of suspension"))
		FVector SpringBottomOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack", meta = (ToolTip = "Force of the spring. Minimal value should be enough to support portion of the vehicle's mass. Can calculate as SpringStiffnessMinimal = Vehicle_Mass * 980 / total_number_suspesion_stacks_of_all_tracks"))
		float SpringStiffness;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack", meta = (ToolTip = "Dampening coefficient of the spring. The smaller the value the more spring will oscilate. Too large value can lead to unstable behaviour. Value of about 1/10th to 1/100th of spring stiffness should be enough"))
		float SpringDampening;


	UPROPERTY()
		UMeshComponent* ShapeSweepMesh;
	
	UPROPERTY()
	FVector RoadWheelRelativeLocation;

	//Default constructor
	FTrackSuspensionStack()
	{
		IsShapeTrace = false;
		SweepShapeComponentName = FString("none");
		StackPosition = FVector::ZeroVector;
		RoadWheelRadius = 15.0;
		SpringTopOffset = FVector(0, 0, 10);
		SpringBottomOffset = FVector(0, 0, -10);
		SpringStiffness = 1000000.0f;
		SpringDampening = 10000.0f;
		RoadWheelRelativeLocation = FVector::ZeroVector;
	}

};