//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "Components/MeshComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/EngineTypes.h"
#include "MMTTrackSplinePointAnimated.h"
#include "MMTTrackAnimationComponent.generated.h"

UCLASS(ClassGroup = (MMT), meta = (BlueprintSpawnableComponent))
class MMT_API UMMTTrackAnimationComponent : public USceneComponent
{
	GENERATED_BODY()
public:
	
	//Set default values for this component
	UMMTTrackAnimationComponent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Called from editor on compilation of BP
	virtual void PostLoad() override;
	
	//Called from editor on compilation of BP
	virtual void InitializeComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings", meta = (ToolTip = "Array of sprockets, idlers and roadwheels to be animated to be animated"))
	TArray<FString> SprocketsIdlersRoadwheelsNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings", meta = (ToolTip = "Spline component that defines path of the track"))
	FString TrackSplineComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings", meta = (ToolTip = "Instanced mesh component for rendering treads"))
	FString TreadsInstancedMeshComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings", meta = (ToolTip = "Array of default location for track spline control points"))
	TArray<FVector> TrackSplinePointLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings", meta = (ToolTip = "Array of default tangents for track spline control points"))
	TArray<FVector> TrackSplinePointTangents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings", meta = (ToolTip = "Array of spline point indexes and references for their animation"))
	TArray<FTrackSplinePointAnimated> AnimatedTrackSplinePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings", meta = (ToolTip = "Amount of treads on this track"))
	int32 TreadsOnSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings", meta = (ToolTip = "Plays animation in reverse if meshes are rotated 180 degrees"))
	bool IsFlipAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings", meta = (ToolTip = "Enable on screen debug output"))
	bool IsDebugMode;


	/**
	*	Sets new angular and linear velocity of the sprocket, idlers, roadwheels and treads
	*	@param AngularVelocity 		used to rotate sprockets, idlers and roadwheel meshes.
	*	@param LinearVelocity 		used to move treads along the spline.
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Track Animation Component")
	void SetTrackPartsAngularAndLinearVelocity(const float& AngularVelocity, const float& LinearVelocity);

	/**
	*	Updates animation of track elements. Should be called on each tick for maximum fidelity. Call less frequently for LODs.
	*	@param DeltaTime			Delta time
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Track Animation Component")
	void UpdateTrackAnimation(const float& DeltaTime);

	/**
	*	Call from constructor to set track spline component into default state and add tread instances to Treads Instanced Mesh component.
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Track Animation Component")
	void BuildTrackMeshAndSpline();

private:
	UPROPERTY()
	TArray<UMeshComponent*> SprocketsIdlersRoadwheelsComponents;

	UPROPERTY()
	USplineComponent* TrackSplineComponent;

	UPROPERTY()
	UInstancedStaticMeshComponent* TreadsInstancedMeshComponent;

	UPROPERTY()
	float TreadMeshPositionOffset;

	UPROPERTY()
	float TrackPartsAngularVelocity;

	UPROPERTY()
	float TreadsLinearVelocity;

	// Find reference to named components
	void GetComponentsReference();

	//Get transform along spline with flipped rotation when spline flips
	FTransform GetAllignedTransformAlongSpline(const float& Distance);
};