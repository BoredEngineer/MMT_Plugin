//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/MeshComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/EngineTypes.h"
#include "MMTSuspensionSimModeEnum.h"
#include "MMTSuspensionStackStruct.generated.h"


USTRUCT(BlueprintType)
struct FSuspensionStackStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Disables component from doing physics calculations and applying forces, useful for debugging."))
		bool bDisabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Suspension simulation mode in which suspension stack starts by default. Simulation mode can be later changed in real-time (useful for LODs). RayCheck - is simplest and fastest but works correctly only on relatively smooth terrain. Shape Sweep - works with any type of terrain and obstacles but more expensive than RayCheck. Tread Simulation - road-wheels will collide with dynamic track surface, provides the most accurate simulation compared to other modes (requires track simulation component to work)"))
		ESuspensionSimMode SimulationMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Name of the mesh component that will receive suspension force, usually root component"))
		FString SprungComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "By default, component's position is used as stack position. Set to TRUE when array of stacks is used inside of a single component, otherwise all stacks will be in the same spot"))
		uint32 bUseCustomPosition : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (EditCondition = "bUseCustomPosition", ToolTip = "Local position of the stack in relation to component. Normally a point where suspension would be physically attached to the chassis"))
		FVector StackLocalPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Radius of the road-wheel in cm. Needed for when track is using simulation feature or when IsShapeTrace is off and ray-check is used instead of sweeps"))
		float RoadWheelRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Thickness of the track in cm. Needed for offset calculations to prevent wheels from clipping track"))
		float TrackThickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Top limit of the spring in relation to suspension stack position. Doesn't have to be aligned with Z axis. In case of torsion bar suspension, spring can be angled to follow closer radial movement of suspension"))
		FVector SpringTopOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Bottom limit of the spring in relation to suspension stack position. Doesn't have to be aligned with Z axis. In case of torsion bar suspension, spring can be angled to follow closer radial movement of suspension"))
		FVector SpringBottomOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Force of the spring. Minimal value should be enough to support portion of the vehicle's mass. Can calculate as SpringStiffnessMinimal = Vehicle_Mass * 980 / total_number_suspesion_stacks_of_all_tracks"))
		float SpringStiffness;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "As spring compresses it produces more and more force which can push vehicle more than desired. This parameter defines maximum force that can be produced by the spring as ratio of SpringStiffness. Value of 1.05 means that maximum force will be not more than 5% higher than SpringStiffness."))
		float SpringMaxOutputRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Dampening coefficient of the spring. The smaller the value the more spring will oscilate. Too large value can lead to unstable behaviour. Value of about 1/10th to 1/100th of spring stiffness should be enough"))
		float SpringDampening;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Will return velocity of the body that wheel made contact with. Takes performance but allows to drive vehicle on moving platforms correctly."))
		bool bGetContactBodyVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Will output debug information on screen"))
		bool bEnableDebugMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ray Check mode", meta = (ToolTip = "Default channel for suspension tracing in Ray Check mode"))
		TEnumAsByte <ECollisionChannel> RayCheckTraceChannel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Check mode", meta = (ToolTip = "Can use sphere check to find suspension compression?"))
		bool bCanSphereCheck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Check mode", meta = (EditCondition = "bCanSphereCheck", ToolTip = "Default channel for suspension tracing in Sphere Check mode"))
		TEnumAsByte <ECollisionChannel> SphereCheckTraceChannel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape Sweep mode", meta = (ToolTip = "Can use shape sweep to find suspension compression?"))
		bool bCanShapeSweep;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape Sweep mode", meta = (EditCondition = "bCanShapeSweep", ToolTip = "Name of the mesh component that will be used for shape sweep of this suspension stack"))
		FString SweepShapeComponentName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape Sweep mode", meta = (ToolTip = "Will rotate sweep shape such that its Z vector will be parallel to direction of sweep, otherwise shape is not rotated"))
		bool bRotateAlongTraceVector;



	//Default constructor
	FSuspensionStackStruct()
	{
		bDisabled = false;
		SimulationMode = ESuspensionSimMode::RayCheck;
		SprungComponentName = FString("Root");
		bUseCustomPosition = false;
		StackLocalPosition = FVector::ZeroVector;
		RoadWheelRadius = 15.0;
		TrackThickness = 3.0;
		SpringTopOffset = FVector(0, 0, 10);
		SpringBottomOffset = FVector(0, 0, -10);
		SpringStiffness = 1000000.0f;
		SpringMaxOutputRatio = 1.05f;
		SpringDampening = 10000.0f;
		bGetContactBodyVelocity = false;
		bEnableDebugMode = false;
		RayCheckTraceChannel = ECC_Visibility;
		bCanSphereCheck = false;
		SphereCheckTraceChannel = ECC_Visibility;
		bCanShapeSweep = false;
		SweepShapeComponentName = FString("none");
	}

};