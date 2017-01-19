//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/MeshComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/EngineTypes.h"
#include "MMTTrackSprocketAndIdler.generated.h"


USTRUCT(BlueprintType)
struct FTrackSprocketAndIdler
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprocket and idler", meta = (ToolTip = "On most of the vehicles, sprocket and main idler can collide with environment and provide traction"))
		uint32 IsAbleToProvideTraction : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprocket and idler", meta = (EditCondition = "IsAbleToProvideTraction", ToolTip = "Name of the mesh component of sprocket or idler that will be used for calculation of physics for traction. Needs to have collision"))
		FString MeshComponentName;

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
	FTrackSprocketAndIdler()
	{
		IsAbleToProvideTraction = false;
		MeshComponentName = FString("none");
		StackPosition = FVector::ZeroVector;
		RoadWheelRadius = 15.0;
		SpringTopOffset = FVector(0, 0, 10);
		SpringBottomOffset = FVector(0, 0, -10);
		SpringStiffness = 1000000.0f;
		SpringDampening = 10000.0f;
		RoadWheelRelativeLocation = FVector::ZeroVector;
	}

};