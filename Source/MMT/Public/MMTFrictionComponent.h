//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/SceneComponent.h"
#include "Components/MeshComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/EngineTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "MMTPhysicalSurfaceRollingFrictionCoefficient.h"
#include "MMTContactPointData.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Component to which friction force will be applied, it has to simulate physics"))
	FString EffectedComponentName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Enable on screen debug output"))
	bool IsDebugMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Static friction coefficient in X axis"))
	float MuXStatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Kinetic friction coefficient in X axis"))
	float MuXKinetic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Static friction coefficient in Y axis"))
	float MuYStatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Kinetic friction coefficient in Y axis"))
	float MuYKinetic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Friction Settings", meta = (ToolTip = "Array of physical surfaces and desired rolling friction coefficients for them"))
	TArray<FPhysicalSurfaceRollingFrictionCoefficient> PhysicsSurfaceResponse;

	/**
	*	Register collision event of the component related to Friction Component
	*	@param NormalImpulseAtPoint		Normal impulse is amount of "force" that was exerted on object as the result of collision.  Normal impulse is a typical output of OnComponentHit event.
	*	@param ContactPointLocation		World Space Location of the contact point.
	*	@param ContactPointNormal		Normal vector of the contact point.
	*	@param PhysicalMaterial		PhysicalMaterial of the object with which contact occurred.
	*	@param InducedVelocity		Optional. Induced velocity is the velocity of the surface or an object where contact occurred. For example, for tank being transported on the train cart, induced velocity would be velocity of the cart.
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Friction Component")
	void RegisterFrictionPoint(const FVector& NormalImpulseAtPoint, const FVector& ContactPointLocation, const FVector& ContactPointNormal, UPhysicalMaterial* PhysicalMaterial,
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
	*	@param 	Linear velocity of the friction surface
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Friction Component")
	void SetFrictionSurfaceVelocity(const FVector& FrictionSurfaceVel);

private:
	UPROPERTY()
	UMeshComponent* EffectedComponentMesh;

	UPROPERTY()
	FVector FrictionSurfaceVelocity; //Linear Track Velocity in case of a tank

	UPROPERTY()
	TArray<FContactPointData> ContactPointsData;

	UPROPERTY()
	FTransform ReferenceFrameTransform;

	UPROPERTY()
	FVector	PreviousVelocityAtPoint;

	//Find reference to named components
	void GetComponentsReference();
};