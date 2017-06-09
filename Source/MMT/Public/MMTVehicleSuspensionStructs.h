#pragma once
//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Engine/EngineTypes.h"
#include "MMTTrack.h"
#include "MMTVehicleSuspensionStructs.generated.h"

//----------------- Tracks configuration
USTRUCT()
struct FTrackSets
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Name of this track for debugging purposes and improving usability, for example 'Left Track'"))
		FName TrackName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Disables item from physics calculations and applying forces, useful for debugging."))
		bool bDisabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Enable on screen debug output"))
		bool bDebugMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Name of the scene component that will be used as frame of reference for position and rotation of track center. For example, track can be mounted on rotating platform"))
		FString ReferenceFrameComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Track class that contains common track configuration parameters"))
		TSubclassOf<UMMTTrack> TrackClass;
	
	FTrackSets()
	{
		TrackName = FName(TEXT("MyTrack"));
		ReferenceFrameComponentName = FString("None");
		bDisabled = false;
		bDebugMode = false;
	}
};

USTRUCT()
struct FTracksConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Set to true if vehicle has friction surfaces such as ski (directional friction)"))
		bool bEnableTracks;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (EditCondition = "bEnableTracks", ToolTip = "You need as many sets as many tracks you have, typical tank will have 2 sets of tracks"))
		TArray<FTrackSets> TrackSets;
};

//----------------- Wheels configuration
USTRUCT()
struct FWheelSets
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Name of this wheel for debugging purposes and improving usability, for example 'Back Right Wheel'"))
		FName WheelName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Disables item from physics calculations and applying forces, useful for debugging."))
		bool bDisabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Enable on screen debug output"))
		bool bDebugMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Name of the scene component that will be used as frame of reference for position and rotation (yaw) of wheel center"))
		FString ReferenceFrameComponentName;
	FWheelSets()
	{
		WheelName = FName(TEXT("MyWheel"));
		ReferenceFrameComponentName = FString("None");
		bDisabled = false;
		bDebugMode = false;
	}
};

USTRUCT()
struct FWheelsConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Set to true if vehicle has friction surfaces such as ski (directional friction)"))
		bool bEnableWheels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (EditCondition = "bEnableWheels", ToolTip = "You need as many sets as many wheels you have, typical car will have 4 sets of wheels"))
		TArray<FWheelSets> WheelSets;
};


//----------------- Passive surface configuration
USTRUCT()
struct FPassiveSets
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Name of this passive surface for debugging purposes and improving usability, for example 'Left Steering Ski'"))
		FName SurfaceName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Disables item from physics calculations and applying forces, useful for debugging."))
		bool bDisabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Enable on screen debug output"))
		bool bDebugMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Name of the scene component that will be used as frame of reference for position and rotation of ski center"))
		FString ReferenceFrameComponentName;

	FPassiveSets()
	{
		SurfaceName = FName(TEXT("MySki"));
		ReferenceFrameComponentName = FString("None");
		bDisabled = false;
		bDebugMode = false;
	}
};

USTRUCT()
struct FPassiveSurfaceConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Set to true if vehicle has friction surfaces such as ski (directional friction)"))
		bool bEnablePassiveSurfaces;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (EditCondition = "bEnablePassiveSurfaces", ToolTip = "You need as many sets as many surfaces you have, typical sled will have 2 sets of ski"))
		TArray<FPassiveSets> PassiveSets;
};