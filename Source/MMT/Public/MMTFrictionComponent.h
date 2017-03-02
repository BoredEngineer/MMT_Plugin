//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/SceneComponent.h"
#include "Components/MeshComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/EngineTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "MMTPhysicalSurfaceRollingFrictionCoefficientStruct.h"
#include "MMTContactPointDataStruct.h"
#include "MMTFrictionComponent.generated.h"

UCLASS(ClassGroup = (MMT), meta = (BlueprintSpawnableComponent))
class MMT_API UMMTFrictionComponent : public USceneComponent
{
	GENERATED_BODY()
public:
	//Set default values for this component
	UMMTFrictionComponent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Disables component from doing physics calculations and applying forces, useful for debugging."))
		bool bDisabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Component to which friction force will be applied, it has to simulate physics"))
		FString EffectedComponentName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Enable on screen debug output"))
		bool IsDebugMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Static friction coefficient in X axis"))
		float MuXStatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Kinetic friction coefficient in X axis"))
		float MuXKinetic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Set TRUE to specify different friction coefficient in Y axis, otherwise MuX is used as uniform coefficient. Useful for objects such as ice skates, brushed contact surface and some types of tank tracks."))
		bool bUseAnisotropicFriction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (EditCondition = "bUseAnisotropicFriction", ToolTip = "Static friction coefficient in Y axis"))
		float MuYStatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (EditCondition = "bUseAnisotropicFriction", ToolTip = "Kinetic friction coefficient in Y axis"))
		float MuYKinetic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Array of physical surfaces and desired rolling friction coefficients for them"))
	TArray<FPhysicalSurfaceRollingFrictionCoefficientStruct> PhysicsSurfaceResponse;

	/**
	*	Register collision event of the component related to Friction Component
	*	@param NormalImpulseAtPoint		Normal impulse is amount of "force" that was exerted on object as the result of collision.  Normal impulse is a typical output of OnComponentHit event.
	*	@param ContactPointLocation		World Space Location of the contact point.
	*	@param ContactPointNormal		Normal vector of the contact point.
	*	@param PhysicalMaterial		PhysicalMaterial of the object with which contact occurred.
	*	@param InducedVelocity		Optional. Induced velocity is the velocity of the surface or an object where contact occurred. For example, for tank being transported on the train cart, induced velocity would be velocity of the cart.
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Friction Component")
	void RegisterFrictionPoint(const FVector& ImpactForceOrImpulseAtPoint, const bool TreatAsImpulse, const FVector& ContactPointLocation, const FVector& ContactPointNormal, UPhysicalMaterial* PhysicalMaterial,
								const FVector InducedVelocity);

	/**
	*	Checks if registered friction point is active
	*	@return 	Returns true if the next contact point in line will be processed in friction calculation.
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Friction Component")
	bool IsFrictionPointActive();

	/**
	*	Empties array of stored friction points, this needs to be done after physics sub-stepping or before new collision information come in
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Friction Component")
	void ResetFrictionPoints();

	/**
	*	Sets velocity of the friction surface. In case of the tank it would be linear velocity of the track.
	*	@param FrictionSurfaceVel	Linear velocity of the friction surface
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Friction Component")
	void SetFrictionSurfaceVelocity(FVector FrictionSurfaceVel = FVector::ZeroVector);

	/**
	*	Runs calculations on friction component, applies friction force to effected component and returns reaction forces (forces that can effect track or a wheel)
	*	@param NumberOfContactPoints		Total number of friction points active on this update cycle
	*	@param DeltaTime					Delta time
	*	@return	NormalizedReactionForce		Reaction force to friction force. When friction force between track and ground pushes vehicle forward, reaction force pushes track in opposite direction
	*	@return	RollingFrictionForce		Rolling friction force is a force opposing rolling of the track or the wheel, it depends on ground pressure and ground surface properties
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Friction Component")
	void PhysicsUpdate(const float& NumberOfContactPoints, const float& DeltaTime, FVector& NormalizedReactionForce, float& RollingFrictionForce);

	/**
	*	Switches debug mode on/off with every call
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Friction Component")
	void ToggleDebugMode();

private:
	UPROPERTY()
	UMeshComponent* EffectedComponentMesh;

	UPROPERTY()
	FVector FrictionSurfaceVelocity; //Linear Track Velocity in case of a tank

	UPROPERTY()
	TArray<FContactPointDataStruct> ContactPointsData;

	UPROPERTY()
	FTransform ReferenceFrameTransform;

	UPROPERTY()
	FVector	PrevRelativeVelocityAtPoint;

	//Find reference to named components
	void GetComponentsReference();

	//Runs calculations on friction component, applies friction force to effected component and returns reaction forces (forces that can effect track or a wheel)
	void ApplyFriction(const FVector& ContactPointLocation, const FVector& ContactPointNormal, const FVector& InducedVelocity, const FVector& PreNormalForceAtPoint,
		const EPhysicalSurface& PhysicalSurface, const float& NumberOfContactPoints, const float& DeltaTime, FVector& NormalizedReactionForce, float& RollingFrictionForce);

};