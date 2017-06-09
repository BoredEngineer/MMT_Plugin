//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Engine/EngineTypes.h"
#include "MMTSuspensionStack.h"
#include "MMTTrack.generated.h"


//----------------- Track settings
USTRUCT()
struct FMMTTrackPhysicalSurfaceFriction
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Physical surface for which we define friction properties"))
		TEnumAsByte<EPhysicalSurface> PhysicalSurface;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Scale of the friction coefficient in respect to surface type. For example Ice would have smaller friction such as 0.1, while asphalt would improve friction with scale of 1.2"))
		float FrictionScaleFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Higher rolling friction coefficient leads to higher rolling friction force"))
		float RollingFrictionCoefficient;

	FMMTTrackPhysicalSurfaceFriction()
	{
		PhysicalSurface = EPhysicalSurface::SurfaceType_Default;
		FrictionScaleFactor = 1.0f;
		//Default rolling friction is set to zero as to avoid potential confusion in regards to drive train tuning or tuning of friction coefficients
		RollingFrictionCoefficient = 0.0f;
	}
};

USTRUCT()
struct FMMTTrackFrictionSettings
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Static friction coefficient in X axis"))
		float MuXStatic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Kinetic friction coefficient in X axis"))
		float MuXKinetic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Set TRUE to specify different friction coefficient in Y axis, otherwise MuX is used as uniform coefficient. Useful for objects such as ice skates, brushed contact surface and some types of tank tracks."))
		bool bUseAnisotropicFriction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseAnisotropicFriction", ToolTip = "Static friction coefficient in Y axis"))
		float MuYStatic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseAnisotropicFriction", ToolTip = "Kinetic friction coefficient in Y axis"))
		float MuYKinetic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Friction coefficients for different physical surfaces"))
		TArray<FMMTTrackPhysicalSurfaceFriction> TrackPhysicalSurfaceFrictionCoefficients;

	FMMTTrackFrictionSettings()
	{
		MuXStatic = 0.75f;
		MuXKinetic = 0.5f;
		bUseAnisotropicFriction = false;
		MuYStatic = 1.0f;
		MuYKinetic = 0.75f;
		TrackPhysicalSurfaceFrictionCoefficients.Add(FMMTTrackPhysicalSurfaceFriction());
	}
};


UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent), ClassGroup = (MMT))
class MMT_API UMMTTrack : public USceneComponent
{
	GENERATED_BODY()
public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Traction settings of the track"))
	//	FMMTTrackTractionSettings TrackTractionSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Name of the mesh component that will receive forces, usually chassis/root component"))
		FString EffectedComponentName = FString("Chassis");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Friction settings of the track"))
		FMMTTrackFrictionSettings TrackFrictionSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Friction settings of the track"))
		TArray<FSuspensionStackStruct> SuspendedWheels;

	// Called when the game starts
		virtual void BeginPlay() override;

		virtual void OnComponentCreated() override;

	//Create objects with default settings
		void CreateSubObjects();

	//Set effected mesh component reference, usually chassis/root component
	UFUNCTION(BlueprintCallable, Category = "MMT Track")
		void SetEffectedMeshReference(UMeshComponent* EffectedMeshComponentRef);

	UFUNCTION(BlueprintCallable, Category = "MMT Track")
		void UpdatePhysics(float DeltaTime);

	//Toggle debug mode on/off
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void ToggleDebugMode();

private:
	TArray<UMMTSuspensionStack*> SuspendedWheelsRef;
};