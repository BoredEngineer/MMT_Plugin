//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsSettings.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"
#include "MMTBPFunctionLibrary.generated.h"

UCLASS()
class MMT_API UMMTBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	
	/**
	*	Get world-space component's transform. BodyInstance is used to retrieve transform, its up-to date with physics sub-stepping.
	*	@param Target	Component's reference to get the transform for
	*	@param InSocketName	Optional socket name parameter. Will return socket transform, if socket not found returns component's transform
	*	@return			Component's or socket's transform in world space
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Get Transform -Comp"), Category = "MMT physics sub-stepping")
	static FTransform MMTGetTransformComponent(USceneComponent * Target, FName InSocketName);

	/**
	*	Sets component's BodyInstance transform. To be used during sub-stepping to move/rotate PhysX bodies directly.
	*	@param Target	Component's reference to set transform for
	*	@param bTeleport		No velocity will be inferred, object is moved in instant
	*	@param Transform	New transform to be set
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Set Transform -Comp"), Category = "MMT physics sub-stepping")
	static void MMTSetTransformComponent(USceneComponent * Target, const FTransform& Transform, bool bTeleport);

	/**
	*	Get world-space actor's root component transform. BodyInstance is used to retrieve transform, its up-to date with physics sub-stepping.
	*	@param Actor	Actor's reference to get the transform for
	*	@return			Actor's root component transform in world space
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Get Transform -Actor"), Category = "MMT physics sub-stepping")
	static FTransform MMTGetTransformActor(AActor* Actor);

	/**
	*	Add a force to a component. Can apply force during physics sub-steps.
	*	@param Target		Component's reference to apply force to
	*	@param Force		Force vector to apply. Magnitude indicates strength of force.
	*	@param bAccelChange If true, Force is taken as a change in acceleration instead of a physical force (i.e. mass will have no affect).
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Add Force -Comp"), Category = "MMT physics sub-stepping")
	static void MMTAddForceComponent(UPrimitiveComponent* Target, const FVector& Force, bool bAccelChange);

	/**
	*	Add a force to a component at a particular location. Can apply force during physics sub-steps.
	*	@param Target		Component's reference to apply force to
	*	@param Force		Force vector to apply. Magnitude indicates strength of force.
	*	@param Location		Location to apply force, in world space.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Add Force At Location -Comp"), Category = "MMT physics sub-stepping")
	static void MMTAddForceAtLocationComponent(UPrimitiveComponent* Target, const FVector& Force, const FVector& Location);

	/**
	*	Add a torque to a component. Can apply torque during physics sub-steps.
	*	@param Target		Component's reference to apply torque to
	*	@param Torque		Force vector to apply. Magnitude indicates strength of force.
	*	@param bAccelChange If true, Force is taken as a change in acceleration instead of a physical force (i.e. mass will have no affect).
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Add Torque -Comp"), Category = "MMT physics sub-stepping")
	static void MMTAddTorqueComponent(UPrimitiveComponent* Target, const FVector& Torque, bool bAccelChange);

	/**
	*	Sets inertia tensor on component.
	*	@param Target		Component's reference to set inertia tensor
	*	@param TensorVector	Inertia tensor vector
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Set Inertia Tensor -Comp"), Category = "MMT physics sub-stepping")
	static void MMTSetInertiaTensor(UPrimitiveComponent* Target, const FVector& InertiaTensor);

	/**
	*	Returns reference to mesh component by the name of the component.
	*	@param Target		Components reference that requests reference to mesh component
	*	@param Name			Name of the mesh component that needs to be found
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Get Mesh Component Reference by Name"), Category = "MMT Utility")
	static UMeshComponent* GetMeshComponentReferenceByName(UActorComponent* Target, FString Name);

	/**
	*	Returns reference to spline component by the name of the component.
	*	@param Target		Components reference that requests reference to mesh component
	*	@param Name			Name of the spline component that needs to be found
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Get Spline Component Reference by Name"), Category = "MMT Utility")
	static USplineComponent* GetSplineComponentReferenceByName(UActorComponent* Target, FString Name);

	/**
	*	Returns reference to instanced static mesh component by the name of the component.
	*	@param Target		Components reference that requests reference to mesh component
	*	@param Name			Name of the spline component that needs to be found
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Get Instanced Static Mesh Component Reference by Name"), Category = "MMT Utility")
	static UInstancedStaticMeshComponent* GetInstancedStaticMeshComponentReferenceByName(UActorComponent* Target, FString Name);

	/**
	*	Returns static and kinetic friction coefficients from friction ellipse in relation to direction of velocity vector
	*	@param VelocityDirectionNormalizedWS	Normalized direction vector in world space for which friction coefficient need to be calculated
	*	@param ForwardVectorWS					World space direction that defines direction of X axis of friction ellipse
	*	@param MuXStatic				Static friction coefficient in X axis
	*	@param MuXKinetic				Kinetic friction coefficient in X axis
	*	@param MuYStatic				Static friction coefficient in Y axis
	*	@param MuYKinetic				Kinetic friction coefficient in Y axis
	*	@return	MuStatic				Calculated static friction coefficient
	*	@return	MuKinetic				Calculated kinetic friction coefficient
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Get Friction Coefficients from Friction Ellipse"), Category = "MMT Utility")
	static void GetMuFromFrictionElipse(const FVector& VelocityDirectionNormalizedWS, const FVector& ForwardVectorWS, const float MuXStatic, const float MuXKinetic, const float MuYStatic, const float MuYKinetic, 
										float& MuStatic, float& MuKinetic); //output variables
	
	/**
	*	Sets linear break threshold on physics constraint component as such functionality is not exposed to BP.
	*	@param Target					Physics constraint to set break threshold on
	*	@param LinearBreakThreshold		Float magnitude of force that is enough to break constraint
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Set physics constraint linear break threshold"), Category = "MMT Utility")
	static void SetLinearBreakThreshold(UPhysicsConstraintComponent* Target, const float LinearBreakThreshold);


	//Get enum as readable string
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
	{
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr)
		{
			return FString("Invalid Enum");
		}

		return enumPtr->GetEnumName((int32)Value);
	}

	//Get readable name of physical surface
	static FORCEINLINE FString GetPhysicalSurfaceRedableName(const EPhysicalSurface Surface)
	{
		TArray<FPhysicalSurfaceName> PhysSurfaces = UPhysicsSettings::Get()->PhysicalSurfaces;
		
		for (int32 i = 0; i < PhysSurfaces.Num(); i++)
		{
			if (PhysSurfaces[i].Type == Surface)
			{
				return  PhysSurfaces[i].Name.ToString();
			}
		}
		return FString("Physical Surface name not found");
	}


private:
	static FBodyInstance* GetBodyInstance(UPrimitiveComponent* PrimitiveComponent);
	static FTransform GetBodyInstanceTransform(USceneComponent *SceneComponent);
};