//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsSettings.h"
#include "MMTBPFunctionLibrary.h"
#include "MMTFrictionComponent.h"

//For UE4 Profiler ~ Stat
//DECLARE_CYCLE_STAT(TEXT("MMT ~ Register friction point"), STAT_MMTRegisterFrictionPoint, STATGROUP_MMT);
//DECLARE_CYCLE_STAT(TEXT("MMT ~ Friction Physics Update"), STAT_MMTFrictionPhysicsUpdate, STATGROUP_MMT);
//DECLARE_CYCLE_STAT(TEXT("MMT ~ Friction Apply"), STAT_MMTFrictionApply, STATGROUP_MMT);


//Set default values
UMMTFrictionComponent::UMMTFrictionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	bDisabled = false;
	EffectedComponentName = FString("none");
	IsDebugMode = false;
	MuXStatic = 0.75f;
	MuXKinetic = 0.5f;
	bUseAnisotropicFriction = false;
	MuYStatic = 1.0f;
	MuYKinetic = 0.75f;

	PhysicsSurfaceResponse.Add(FPhysicalSurfaceRollingFrictionCoefficientStruct());
}

// Called when the game starts or when spawned
void UMMTFrictionComponent::BeginPlay()
{
	// Get references to named components
	GetComponentsReference();

	//Clear up contact data array just in case
	ContactPointsData.Empty();

	Super::BeginPlay();
}


// Register collision information for future processing in friction component
void UMMTFrictionComponent::RegisterFrictionPoint(const FVector& ImpactForceOrImpulseAtPoint, const bool TreatAsImpulse, const FVector& ContactPointLocation, const FVector& ContactPointNormal,
													UPhysicalMaterial* PhysicalMaterial, const FVector InducedVelocity)
{
	//Gather stats
	//SCOPE_CYCLE_COUNTER(STAT_MMTRegisterFrictionPoint);

	FContactPointDataStruct NewContactPoint;
	TEnumAsByte<EPhysicalSurface> PhysicalSurfaceLoc = EPhysicalSurface::SurfaceType_Default;

	NewContactPoint.IsPointActive = true;
	NewContactPoint.InducedVelocity = InducedVelocity;
	NewContactPoint.ImpactForceOrImpulseAtPoint = ImpactForceOrImpulseAtPoint;
	NewContactPoint.bTreatAsImpulse = TreatAsImpulse;
	NewContactPoint.ContactPointLocation = ContactPointLocation;
	NewContactPoint.ContactPointNormal = ContactPointNormal;
	
	if (IsValid(PhysicalMaterial))
	{
		PhysicalSurfaceLoc = PhysicalMaterial->SurfaceType;
	}

	NewContactPoint.PhysicalSurface = PhysicalSurfaceLoc;

	ContactPointsData.Add(NewContactPoint);
}


// Checks if registered friction point is active
bool UMMTFrictionComponent::IsFrictionPointActive()
{
	if (ContactPointsData.Num() > 0)
	{
		return ContactPointsData[0].IsPointActive;
	}
	return false;
}

// Empties array of stored friction points, this needs to be done after physics sub-stepping or before new collision information comes in
void UMMTFrictionComponent::ResetFrictionPoints()
{
	ContactPointsData.Empty();
}

// Set friction surface linear velocity
void UMMTFrictionComponent::SetFrictionSurfaceVelocity(FVector FrictionSurfaceVel)
{
	FrictionSurfaceVelocity = FrictionSurfaceVel;
}

// Set friction surface linear speed
void UMMTFrictionComponent::SetFrictionSurfaceSpeed(float FrictionSurfaceSpeed)
{
	FrictionSurfaceSpd = FrictionSurfaceSpeed;
}

//Find and store reference to named components
void UMMTFrictionComponent::GetComponentsReference()
{
	if (EffectedComponentName != FString("none"))
	{
		EffectedComponentMesh = UMMTBPFunctionLibrary::GetMeshComponentReferenceByName(this, EffectedComponentName);
		if (!IsValid(EffectedComponentMesh))
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *GetOwner()->GetName(), *GetName(), *EffectedComponentName));
			UE_LOG(LogTemp, Warning, TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *GetOwner()->GetName(), *GetName(), *EffectedComponentName);
		}
	}
	else
	{
		EffectedComponentMesh = NULL;
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's EffectedComponentName property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName()));
		UE_LOG(LogTemp, Warning, TEXT("%s->%s component's EffectedComponentName property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName());
	}
}

// Handles logic of contact points stored in array and passes parameters to ApplyFriction() where actual physics calculations are done
bool UMMTFrictionComponent::PhysicsUpdate(const float& NumberOfContactPoints, const float& DeltaTime, FVector& NormalizedReactionForce, float& RollingFrictionForce)
{
	//Gather stats
	//SCOPE_CYCLE_COUNTER(STAT_MMTFrictionPhysicsUpdate);

	bool isResolvedAsStatic = false;

	FVector NormalizedReactionForceOut = FVector::ZeroVector;
	FVector RollingFrictionForceOut = FVector::ZeroVector;

	LastFrictionForce = FVector::ZeroVector;
	LastFrictionLimit = 0.0f;

	if ((ContactPointsData.Num() > 0) & !bDisabled)
	{
		if (ContactPointsData[0].IsPointActive)
		{
			// Get physics sub-stepping valid transform of the friction component
			ReferenceFrameTransform = UMMTBPFunctionLibrary::MMTGetTransformComponent( this, NAME_None);
			
			// Collisions are often to happen on the edge of the collision mesh, we "project" points on the X axis of the friction component as if collision happened
			// on the center-line of the track
			FVector PointLocationCentered;
			FVector PointNormalCentered;
			FVector PreNormalForceCentered;
			


			//Apply Friction calculations
			if (IsAlternateFriction)
			{
				PointLocationCentered = ContactPointsData[0].ContactPointLocation;
				PointNormalCentered = ContactPointsData[0].ContactPointNormal;

				if (ContactPointsData[0].bTreatAsImpulse)
				{
					PreNormalForceCentered = ContactPointsData[0].ImpactForceOrImpulseAtPoint / DeltaTime;
				}
				else
				{
					PreNormalForceCentered = ContactPointsData[0].ImpactForceOrImpulseAtPoint;
				}

				isResolvedAsStatic = ApplyFrictionAlternative(PointLocationCentered, PointNormalCentered, ContactPointsData[0].InducedVelocity, PreNormalForceCentered, 
					ContactPointsData[0].PhysicalSurface, NumberOfContactPoints, DeltaTime, NormalizedReactionForce, RollingFrictionForce);
			}
			else
			{
				PointLocationCentered = ReferenceFrameTransform.InverseTransformPosition(ContactPointsData[0].ContactPointLocation);
				PointLocationCentered = ReferenceFrameTransform.TransformPosition(FVector(PointLocationCentered.X, 0.0f, PointLocationCentered.Z));

				PointNormalCentered = ReferenceFrameTransform.InverseTransformVector(ContactPointsData[0].ContactPointNormal);
				PointNormalCentered = ReferenceFrameTransform.TransformVector(FVector(PointNormalCentered.X, 0.0f, PointNormalCentered.Z));

				//Should provide higher precision if DeltaTime fluctuates between frames
				if (ContactPointsData[0].bTreatAsImpulse)
				{
					PreNormalForceCentered = ReferenceFrameTransform.InverseTransformVector(ContactPointsData[0].ImpactForceOrImpulseAtPoint / DeltaTime);
				}
				else
				{
					PreNormalForceCentered = ReferenceFrameTransform.InverseTransformVector(ContactPointsData[0].ImpactForceOrImpulseAtPoint);
				}
				PreNormalForceCentered = ReferenceFrameTransform.TransformVector(FVector(PreNormalForceCentered.X, 0.0f, PreNormalForceCentered.Z));

				FrictionSurfaceVelocity = FVector::CrossProduct(PointNormalCentered, ReferenceFrameTransform.TransformVector(FVector(0.f, 1.f, 0.f))).GetSafeNormal() * FrictionSurfaceSpd;

				isResolvedAsStatic = ApplyFriction(PointLocationCentered, PointNormalCentered, ContactPointsData[0].InducedVelocity, PreNormalForceCentered, 
					ContactPointsData[0].PhysicalSurface, NumberOfContactPoints, DeltaTime, NormalizedReactionForce, RollingFrictionForce);
			}

		}
		else
		{
			//Return zero vector if point is inactive
			NormalizedReactionForce = FVector::ZeroVector;
			RollingFrictionForce = 0.0f;
		}
		
		// Remove first element as it was processed or wasn't active
		ContactPointsData.RemoveAt(0, 1, true);
	}
	else
	{
		//Return zero vector if no points are stored in array or component is disabled
		NormalizedReactionForce = FVector::ZeroVector;
		RollingFrictionForce = 0.0f;
	}

	return isResolvedAsStatic;
}

//Switch debug mode on/off
void UMMTFrictionComponent::ToggleDebugMode()
{
	IsDebugMode = !IsDebugMode;
}

FVector UMMTFrictionComponent::GetFrictionForce()
{
	return LastFrictionForce;
}

float UMMTFrictionComponent::GetFrictionLimit()
{
	return LastFrictionLimit;
}

// Runs calculations on friction component, applies friction force to effected component and returns reaction forces(forces that can effect track or a wheel)
bool UMMTFrictionComponent::ApplyFriction(const FVector& ContactPointLocation, const FVector& ContactPointNormal, const FVector& InducedVelocity, const FVector& PreNormalForceAtPoint,
	const EPhysicalSurface& PhysicalSurface, const float& NumberOfContactPoints, const float& DeltaTime, FVector& NormalizedReactionForce, float& RollingFrictionForce)
{
	// Gather stats
	//SCOPE_CYCLE_COUNTER(STAT_MMTFrictionApply);

	bool isResolvedAsStatic = false;

	float NormalForceAtContactPoint = PreNormalForceAtPoint.ProjectOnTo(ContactPointNormal).Size();
	
	// Check if Effected Component Mesh reference is valid and escape early otherwise
	if (!IsValid(EffectedComponentMesh))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's EffectedComponentMesh reference is invalid!"), *GetOwner()->GetName(), *GetName()));
		UE_LOG(LogTemp, Warning, TEXT("%s->%s component's EffectedComponentMesh reference is invalid!"), *GetOwner()->GetName(), *GetName());
		NormalizedReactionForce = FVector::ZeroVector;
		RollingFrictionForce = 0.0f;
		return false;
	}
	
	//Find relative velocity of the friction surface and ground/another object at point
	FVector RelativeVelocityAtPoint = EffectedComponentMesh->GetPhysicsLinearVelocityAtPoint(ContactPointLocation) + InducedVelocity + FrictionSurfaceVelocity;
	RelativeVelocityAtPoint = RelativeVelocityAtPoint.VectorPlaneProject(RelativeVelocityAtPoint, ContactPointNormal);

	//filter out oscillations when vehicle is standing still but friction overshoots
	RelativeVelocityAtPoint = (PrevRelativeVelocityAtPoint + RelativeVelocityAtPoint) * 0.5;
	PrevRelativeVelocityAtPoint = RelativeVelocityAtPoint;
	
	/*
	* This piece of code, leads to small forces, like parasitic forces of suspension to sleep in. Need to build a different solution for static case, like based on a constraint for example
	// early exit if velocity is too low to consider as vehicle most likely standing still
	if (RelativeVelocityAtPoint.Size() < 1.0f)
	{
		NormalizedReactionForce = FVector::ZeroVector;
		RollingFrictionForce = FVector::ZeroVector;
		return;
	}
	*/

	//Calculate static and kinetic friction coefficients, taking into account velocity direction and friction ellipse
	float MuStatic;
	float MuKinetic;
	if (bUseAnisotropicFriction)
	{
		UMMTBPFunctionLibrary::GetMuFromFrictionElipse(RelativeVelocityAtPoint.GetSafeNormal(), ReferenceFrameTransform.TransformVector(FVector::ForwardVector),
			MuXStatic, MuXKinetic, MuYStatic, MuYKinetic, MuStatic, MuKinetic);
	}
	else
	{
		MuStatic = MuXStatic;
		MuKinetic = MuXKinetic;
	}
	

	//Calculate "stopping force" which is amount of force necessary to completely remove velocity of the object
	FVector	StoppingForce = ((RelativeVelocityAtPoint * (-1.0f) * EffectedComponentMesh->GetMass()) / DeltaTime) / NumberOfContactPoints;

	//Static friction threshold
	float StaticFrictionLimit = NormalForceAtContactPoint * MuStatic;
	LastFrictionLimit = StaticFrictionLimit;

	//Friction Force that will be applied to effected mesh component
	FVector ApplicationForce;
	//If stopping force is larger than static friction limit we switch to kinetic friction
	if (StoppingForce.Size() >= StaticFrictionLimit)
	{
		float KineticFrictionLimit = NormalForceAtContactPoint * MuKinetic;
		LastFrictionLimit = KineticFrictionLimit;

		ApplicationForce = StoppingForce.GetClampedToSize(0.0f, KineticFrictionLimit);
		if (IsDebugMode)
		{
			DrawDebugString(GetWorld(), ContactPointLocation, FString("Kinetic Friction"), nullptr, FColor::Magenta, 0.0f, false);
		}
	}
	else
	{
		//We are in static friction mode, if both induced and friction surface velocities are zero then we report full static case for this friction component
		//in case when all components are in full static mode, we can spawn physics constraint and prevent sliding of the vehicle
		if (FMath::IsNearlyZero(InducedVelocity.SizeSquared()) & FMath::IsNearlyZero(FrictionSurfaceVelocity.SizeSquared()))
		{
			isResolvedAsStatic = true;
		}
		
		ApplicationForce = StoppingForce.GetClampedToSize(0.0f, StaticFrictionLimit);
		if (IsDebugMode)
		{
			DrawDebugString(GetWorld(), ContactPointLocation, FString("Static Friction"), nullptr, FColor::Red, 0.0f, false);
		}
	}
	
	//Apply friction force
	UMMTBPFunctionLibrary::MMTAddForceAtLocationComponent(EffectedComponentMesh, ApplicationForce, ContactPointLocation);

	LastFrictionForce = ApplicationForce;

	//Calculate Reaction force, reaction force is scaled down by the total mass of the effected component. Later this force is scaled up to match mass of the component it will be applied to.
	//For example, reaction force can rotate not powered wheels when vehicle is rolling down. In this case, "normalized" reaction force will be scaled up by the mass of the wheel before applied as torque to the wheel itself
	NormalizedReactionForce = (ApplicationForce * (-1.0f)) / EffectedComponentMesh->GetMass();

	
	//Calculate Rolling Friction
	float RollingFrictionCoefficient = FPhysicalSurfaceRollingFrictionCoefficientStruct().RollingFrictionCoefficient;
	
	for (int32 i = 0; i < PhysicsSurfaceResponse.Num(); i++)
	{
		if (PhysicsSurfaceResponse[i].PhysicalSurface == PhysicalSurface)
		{
			RollingFrictionCoefficient = PhysicsSurfaceResponse[i].RollingFrictionCoefficient;
			break;
		}
	}

	//RollingFrictionForce = RelativeVelocityAtPoint.GetSafeNormal() * NormalForceAtContactPoint * RollingFrictionCoefficient;// / EffectedComponentMesh->GetMass();
	RollingFrictionForce = NormalForceAtContactPoint * RollingFrictionCoefficient;

	if (IsDebugMode)
	{
		DrawDebugLine(GetWorld(), ContactPointLocation, ContactPointLocation + ApplicationForce * 0.005f, FColor::Yellow, false, 0.0f, 0, 3.0f);
		DrawDebugLine(GetWorld(), ContactPointLocation, ContactPointLocation + RelativeVelocityAtPoint.GetSafeNormal() * RollingFrictionForce * 0.01f, FColor::Green, false, 0.0f, 0, 3.0f);
		//DrawDebugString(GetWorld(), ContactPointLocation + FVector(0.0f, 0.0f, 50.0f), UMMTBPFunctionLibrary::GetEnumValueAsString<EPhysicalSurface>("EPhysicalSurface", PhysicalSurface), nullptr, FColor::Cyan, 0.0f, false);
		DrawDebugString(GetWorld(), ContactPointLocation + FVector(0.0f, 0.0f, 50.0f), UMMTBPFunctionLibrary::GetPhysicalSurfaceRedableName(PhysicalSurface), nullptr, FColor::Cyan, 0.0f, false);
	}

	return isResolvedAsStatic;
}

bool UMMTFrictionComponent::ApplyFrictionAlternative(const FVector& ContactPointLocation, const FVector& ContactPointNormal, const FVector& InducedVelocity, const FVector& PreNormalForceAtPoint,
	const EPhysicalSurface& PhysicalSurface, const float& NumberOfContactPoints, const float& DeltaTime, FVector& NormalizedReactionForce, float& RollingFrictionForce)
{
	// Gather stats
	//SCOPE_CYCLE_COUNTER(STAT_MMTFrictionApply);

	bool isResolvedAsStatic = false;

	FVector NormalForceAtContactPoint = PreNormalForceAtPoint.ProjectOnTo(ContactPointNormal) * (-1.0f);
	float MagnitudeNormalForceAtContactPoint = NormalForceAtContactPoint.Size();

	// Check if Effected Component Mesh reference is valid and escape early otherwise
	if (!IsValid(EffectedComponentMesh))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's EffectedComponentMesh reference is invalid!"), *GetOwner()->GetName(), *GetName()));
		UE_LOG(LogTemp, Warning, TEXT("%s->%s component's EffectedComponentMesh reference is invalid!"), *GetOwner()->GetName(), *GetName());
		NormalizedReactionForce = FVector::ZeroVector;
		RollingFrictionForce = 0.0f;
		return false;
	}

	//Find relative velocity of the friction surface and ground/another object at point
	FVector RelativeVelocityAtPoint = EffectedComponentMesh->GetPhysicsLinearVelocityAtPoint(ContactPointLocation) + InducedVelocity + FrictionSurfaceVelocity;
	RelativeVelocityAtPoint = RelativeVelocityAtPoint.VectorPlaneProject(RelativeVelocityAtPoint, ContactPointNormal);

	//filter out oscillations when vehicle is standing still but friction overshoots
	RelativeVelocityAtPoint = (PrevRelativeVelocityAtPoint + RelativeVelocityAtPoint) * 0.5;
	PrevRelativeVelocityAtPoint = RelativeVelocityAtPoint;

	/*
	* This piece of code, leads to small forces, like parasitic forces of suspension to sleep in. Need to build a different solution for static case, like based on a constraint for example
	// early exit if velocity is too low to consider as vehicle most likely standing still
	if (RelativeVelocityAtPoint.Size() < 1.0f)
	{
	NormalizedReactionForce = FVector::ZeroVector;
	RollingFrictionForce = FVector::ZeroVector;
	return;
	}
	*/

	//Calculate static and kinetic friction coefficients, taking into account velocity direction and friction ellipse
	float MuStatic;
	float MuKinetic;
	if (bUseAnisotropicFriction)
	{
		UMMTBPFunctionLibrary::GetMuFromFrictionElipse(RelativeVelocityAtPoint.GetSafeNormal(), ReferenceFrameTransform.TransformVector(FVector::ForwardVector),
			MuXStatic, MuXKinetic, MuYStatic, MuYKinetic, MuStatic, MuKinetic);
	}
	else
	{
		MuStatic = MuXStatic;
		MuKinetic = MuXKinetic;
	}

	//Calculate "stopping force" which is amount of force necessary to completely remove velocity of the object
	FVector	StoppingForce = ((RelativeVelocityAtPoint * (-1.0f) * EffectedComponentMesh->GetMass()) / DeltaTime) / NumberOfContactPoints;


	//Calculate Stopping Force to combat only effects of gravity. What we need to find is [m * g * sin(a)] for each point. What we know already is [m * g * cos(a)] which is projected
	//suspension force - normal force. Note that we can't use mass of the whole vehicle as m as we don't know distribution of mass over multiple contact points.
	//We assume standard direction of gravity.
	FVector InverseGravityDirection = FVector(0.0f, 0.0f, 1.0f);
	FVector GravityStoppingForce = FVector::ZeroVector;
	float cosA = FVector::DotProduct(NormalForceAtContactPoint.GetSafeNormal(), InverseGravityDirection);
	cosA = FMath::Abs(cosA);

	//don't add any force if we are standing on flat surface
	if (!FMath::IsNearlyZero(cosA, 0.0000001f))
	{
		//Extract m * g
		float MassByGravity = MagnitudeNormalForceAtContactPoint / cosA;
		float sinA = FMath::Sqrt(1.0f - (cosA * cosA));

		//MassByGravity = EffectedComponentMesh->GetMass()*980.0f;

		//Add stopping force to deal with gravity. Projecting inverse gravity vector on the slope, gives us vector parallel to slope direction
		GravityStoppingForce = (InverseGravityDirection.VectorPlaneProject(InverseGravityDirection, ContactPointNormal).GetSafeNormal() * MassByGravity * sinA);// / NumberOfContactPoints;
	}

	//Static friction threshold
	float MuStaticByLoad = MagnitudeNormalForceAtContactPoint * MuStatic;

	//Friction Force that will be applied to effected mesh component
	FVector ApplicationForce;
	//If stopping force is larger than static friction limit we switch to kinetic friction
	if (StoppingForce.Size() >= MuStaticByLoad)
	{
		ApplicationForce = StoppingForce.GetClampedToSize(0.0f, MagnitudeNormalForceAtContactPoint * MuKinetic);
		GravityStoppingForce = GravityStoppingForce.GetClampedToSize(0.0f, MagnitudeNormalForceAtContactPoint * MuKinetic);
		if (IsDebugMode)
		{
			DrawDebugString(GetWorld(), ContactPointLocation, FString("Kinetic Friction"), nullptr, FColor::Magenta, 0.0f, false);
		}
	}
	else
	{
		//We are in static friction mode, if both induced and friction surface velocities are zero then we report full static case for this friction component
		//in case when all components are in full static mode, we can spawn physics constraint and prevent sliding of the vehicle
		if (FMath::IsNearlyZero(InducedVelocity.SizeSquared()) & FMath::IsNearlyZero(FrictionSurfaceVelocity.SizeSquared()))
		{
			isResolvedAsStatic = true;
		}

		ApplicationForce = StoppingForce.GetClampedToSize(0.0f, MuStaticByLoad);
		GravityStoppingForce = GravityStoppingForce.GetClampedToSize(0.0f, MuStaticByLoad);
		if (IsDebugMode)
		{
			DrawDebugString(GetWorld(), ContactPointLocation, FString("Static Friction"), nullptr, FColor::Red, 0.0f, false);
		}
	}

	//Apply friction force
	UMMTBPFunctionLibrary::MMTAddForceAtLocationComponent(EffectedComponentMesh, ApplicationForce, ContactPointLocation);
	UMMTBPFunctionLibrary::MMTAddForceComponent(EffectedComponentMesh, GravityStoppingForce, false);
	//DrawDebugLine(GetWorld(), ContactPointLocation, ContactPointLocation + GravityStoppingForce * 0.05f, FColor::Yellow, false, 0.0f, 0, 3.0f);

	LastFrictionForce = ApplicationForce;

	//Calculate Reaction force, reaction force is scaled down by the total mass of the effected component. Later this force is scaled up to match mass of the component it will be applied to.
	//For example, reaction force can rotate not powered wheels when vehicle is rolling down. In this case, "normalized" reaction force will be scaled up by the mass of the wheel before applied as torque to the wheel itself
	NormalizedReactionForce = (ApplicationForce * (-1.0f)) / EffectedComponentMesh->GetMass();


	//Calculate Rolling Friction
	float RollingFrictionCoefficient = FPhysicalSurfaceRollingFrictionCoefficientStruct().RollingFrictionCoefficient;

	for (int32 i = 0; i < PhysicsSurfaceResponse.Num(); i++)
	{
		if (PhysicsSurfaceResponse[i].PhysicalSurface == PhysicalSurface)
		{
			RollingFrictionCoefficient = PhysicsSurfaceResponse[i].RollingFrictionCoefficient;
			break;
		}
	}

	//RollingFrictionForce = RelativeVelocityAtPoint.GetSafeNormal() * NormalForceAtContactPoint * RollingFrictionCoefficient;// / EffectedComponentMesh->GetMass();
	RollingFrictionForce = MagnitudeNormalForceAtContactPoint * RollingFrictionCoefficient;

	if (IsDebugMode)
	{
		DrawDebugLine(GetWorld(), ContactPointLocation, ContactPointLocation + ApplicationForce * 0.005f, FColor::Yellow, false, 0.0f, 0, 3.0f);
		DrawDebugLine(GetWorld(), ContactPointLocation, ContactPointLocation + RelativeVelocityAtPoint.GetSafeNormal() * RollingFrictionForce * 0.01f, FColor::Green, false, 0.0f, 0, 3.0f);
		//DrawDebugString(GetWorld(), ContactPointLocation + FVector(0.0f, 0.0f, 50.0f), UMMTBPFunctionLibrary::GetEnumValueAsString<EPhysicalSurface>("EPhysicalSurface", PhysicalSurface), nullptr, FColor::Cyan, 0.0f, false);
		DrawDebugString(GetWorld(), ContactPointLocation + FVector(0.0f, 0.0f, 50.0f), UMMTBPFunctionLibrary::GetPhysicalSurfaceRedableName(PhysicalSurface), nullptr, FColor::Cyan, 0.0f, false);
	}

	return isResolvedAsStatic;
}


/*stash of code for later reusal

// Gather stats
//SCOPE_CYCLE_COUNTER(STAT_MMTFrictionApply);

float NormalForceAtContactPoint = PreNormalForceAtPoint.ProjectOnTo(ContactPointNormal).Size();

// Check if Effected Component Mesh reference is valid and escape early otherwise
if (!IsValid(EffectedComponentMesh))
{
GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's EffectedComponentMesh reference is invalid!"), *GetOwner()->GetName(), *GetName()));
UE_LOG(LogTemp, Warning, TEXT("%s->%s component's EffectedComponentMesh reference is invalid!"), *GetOwner()->GetName(), *GetName());
NormalizedReactionForce = FVector::ZeroVector;
RollingFrictionForce = 0.0f;
return;
}

//Find relative velocity of the friction surface and ground/another object at point
FVector RelativeVelocityAtPoint = EffectedComponentMesh->GetPhysicsLinearVelocityAtPoint(ContactPointLocation) + InducedVelocity + FrictionSurfaceVelocity;
RelativeVelocityAtPoint = RelativeVelocityAtPoint.VectorPlaneProject(RelativeVelocityAtPoint, ContactPointNormal);

//filter out oscillations when vehicle is standing still but friction overshoots
//RelativeVelocityAtPoint = (PrevRelativeVelocityAtPoint + RelativeVelocityAtPoint) * 0.5;
//PrevRelativeVelocityAtPoint = RelativeVelocityAtPoint;

//Get velocity of the center of mass and project on collision surface
FVector COMVelocity = EffectedComponentMesh->GetPhysicsLinearVelocity();
COMVelocity = COMVelocity.VectorPlaneProject(COMVelocity, ContactPointNormal);

COMVelocity = (COMVelocity + PrevLinearVelocity) * 0.5f;// +InducedVelocity + FrictionSurfaceVelocity;
PrevLinearVelocity = COMVelocity;


//COMVelocity = COMVelocity.VectorPlaneProject(COMVelocity, ContactPointNormal);

//Get only rotational portion of velocity
FVector VelocityFromRotation = RelativeVelocityAtPoint - COMVelocity;


//Calculate static and kinetic friction coefficients, taking into account velocity direction and friction ellipse
float MuStatic;
float MuKinetic;
if (bUseAnisotropicFriction)
{
UMMTBPFunctionLibrary::GetMuFromFrictionElipse(RelativeVelocityAtPoint.GetSafeNormal(), ReferenceFrameTransform.TransformVector(FVector::ForwardVector),
MuXStatic, MuXKinetic, MuYStatic, MuYKinetic, MuStatic, MuKinetic);
}
else
{
MuStatic = MuXStatic;
MuKinetic = MuXKinetic;
}


//Calculate "stopping force" which is amount of force necessary to completely remove velocity of the object
FVector	StoppingForceCOM = ((COMVelocity * (-1.0f) * EffectedComponentMesh->GetMass()) / DeltaTime) / NumberOfContactPoints;
UE_LOG(LogTemp, Warning, TEXT("%s->%s number of contact points: %s"), *GetOwner()->GetName(), *GetName(), *FString::SanitizeFloat(NumberOfContactPoints));

//Get angular velocity of the reference frame
FVector AngularVelocity = EffectedComponentMesh->GetPhysicsAngularVelocity();
AngularVelocity = AngularVelocity.ProjectOnToNormal(ContactPointNormal);
AngularVelocity = (AngularVelocity + PrevAngularVelocity) * 0.5f;
PrevAngularVelocity = AngularVelocity;

//Calculate "stopping torque" which is amount of torque necessary to completely remove angular velocity of the object
FVector StoppingTorque = ((AngularVelocity * (-1.0f) * EffectedComponentMesh->GetInertiaTensor()) / DeltaTime) / NumberOfContactPoints;

DrawDebugLine(GetWorld(), ContactPointLocation, ContactPointLocation + StoppingTorque, FColor::Emerald, false, 0.0f, 0, 3.0f);

FVector DirectionStoppingTorque = StoppingTorque;
DirectionStoppingTorque.Normalize();

float DistanceSquared = (ContactPointLocation - EffectedComponentMesh->GetCenterOfMass()).SizeSquared();

FVector VectorToPoint = EffectedComponentMesh->GetCenterOfMass() - ContactPointLocation;
FVector DirectionToPoint = VectorToPoint;
DirectionToPoint.Normalize();

//Get force from torque and contact points location. The force is pointing in opposite of expected direction on purpose.
//FVector StoppingForceAngular = FVector::CrossProduct(DirectionToPoint, DirectionStoppingTorque) * (StoppingTorque.Size() / VectorToPoint.Size());
FVector StoppingForceAngular = (VelocityFromRotation * (-1.0f) * EffectedComponentMesh->GetInertiaTensor()) / (DistanceSquared * DeltaTime);

if (AngularVelocity.SizeSquared() < 0.01f)
{
StoppingForceAngular = FVector::ZeroVector;
}

DrawDebugLine(GetWorld(), ContactPointLocation, ContactPointLocation + AngularVelocity, FColor::Black, false, 0.0f, 0, 3.0f);

//We need to know full force to test for friction force limit and cap the fore if limit is breached
FVector StoppingForce = StoppingForceCOM + StoppingForceAngular;

//Static friction threshold
float MuStaticByLoad = NormalForceAtContactPoint * MuStatic;

//Friction Force that will be applied to effected mesh component
FVector ApplicationForce;
//If stopping force is larger than static friction limit we switch to kinetic friction
if (StoppingForce.Size() >= MuStaticByLoad)
{
ApplicationForce = StoppingForce.GetClampedToSize(0.0f, NormalForceAtContactPoint * MuKinetic);
if (IsDebugMode)
{
DrawDebugString(GetWorld(), ContactPointLocation, FString("Kinetic Friction"), nullptr, FColor::Magenta, 0.0f, false);
}
}
else
{
ApplicationForce = StoppingForce.GetClampedToSize(0.0f, MuStaticByLoad);
if (IsDebugMode)
{
DrawDebugString(GetWorld(), ContactPointLocation, FString("Static Friction"), nullptr, FColor::Red, 0.0f, false);
}
}

//Find portion of forces after clamp
float AngForceMag = StoppingForceAngular.SizeSquared();
float LinForceMag = StoppingForceCOM.SizeSquared();
float alpha;

FVector StoppingForceAngularDirection = StoppingForceAngular;
FVector StoppingForceCOMDirection = StoppingForceCOM;

//If we happen to move without rotation
if (FMath::IsNearlyZero(AngForceMag))
{
StoppingForceCOMDirection = ApplicationForce;
}
else
{
StoppingForceAngularDirection.Normalize();
StoppingForceCOMDirection.Normalize();

alpha = AngForceMag / (AngForceMag + LinForceMag);
float ApplicationForceMagnitude = ApplicationForce.Size();

StoppingForceAngularDirection *= ApplicationForceMagnitude * alpha;
StoppingForceCOMDirection *= ApplicationForceMagnitude * (1 - alpha);
}

FVector FrictionTorque = FVector::CrossProduct(ApplicationForce, EffectedComponentMesh->GetCenterOfMass() - ContactPointLocation);

//Apply friction force as part of force applied to center of mass and torque as a result of force application off-center of mass
UMMTBPFunctionLibrary::MMTAddForceComponent(EffectedComponentMesh, StoppingForceCOMDirection, false);
UMMTBPFunctionLibrary::MMTAddTorqueComponent(EffectedComponentMesh, FrictionTorque, false);

LastFrictionForce = ApplicationForce;

//Calculate Reaction force, reaction force is scaled down by the total mass of the effected component. Later this force is scaled up to match mass of the component it will be applied to.
//For example, reaction force can rotate not powered wheels when vehicle is rolling down. In this case, "normalized" reaction force will be scaled up by the mass of the wheel before applied as torque to the wheel itself
//NormalizedReactionForce = (ApplicationForce * (-1.0f)) / EffectedComponentMesh->GetMass();

NormalizedReactionForce = (-1.0f) * ( (StoppingForceAngularDirection / EffectedComponentMesh->GetInertiaTensor()) +	(StoppingForceCOMDirection / EffectedComponentMesh->GetMass()) );
//NormalizedReactionForce = (-1.0f) * ((StoppingForceAngularDirection) + (StoppingForceCOMDirection / EffectedComponentMesh->GetMass()));
//NormalizedReactionForce = (-1.0f) * ( (StoppingForceAngularDirection / EffectedComponentMesh->GetInertiaTensor()) +	(StoppingForceCOMDirection) );
//NormalizedReactionForce = (-1.0f) * ( StoppingForceCOMDirection / EffectedComponentMesh->GetMass() );


//Calculate Rolling Friction
float RollingFrictionCoefficient = FPhysicalSurfaceRollingFrictionCoefficientStruct().RollingFrictionCoefficient;

for (int32 i = 0; i < PhysicsSurfaceResponse.Num(); i++)
{
if (PhysicsSurfaceResponse[i].PhysicalSurface == PhysicalSurface)
{
RollingFrictionCoefficient = PhysicsSurfaceResponse[i].RollingFrictionCoefficient;
break;
}
}

//RollingFrictionForce = RelativeVelocityAtPoint.GetSafeNormal() * NormalForceAtContactPoint * RollingFrictionCoefficient;// / EffectedComponentMesh->GetMass();
RollingFrictionForce = NormalForceAtContactPoint * RollingFrictionCoefficient;

if (IsDebugMode)
{
DrawDebugLine(GetWorld(), ContactPointLocation, ContactPointLocation + ApplicationForce * 0.005f, FColor::Yellow, false, 0.0f, 0, 3.0f);
DrawDebugLine(GetWorld(), ContactPointLocation, ContactPointLocation + NormalizedReactionForce * 0.005f, FColor::Purple, false, 0.0f, 0, 3.0f);
DrawDebugLine(GetWorld(), ContactPointLocation, ContactPointLocation + RelativeVelocityAtPoint.GetSafeNormal() * RollingFrictionForce * 0.01f, FColor::Green, false, 0.0f, 0, 3.0f);
//DrawDebugString(GetWorld(), ContactPointLocation + FVector(0.0f, 0.0f, 50.0f), UMMTBPFunctionLibrary::GetEnumValueAsString<EPhysicalSurface>("EPhysicalSurface", PhysicalSurface), nullptr, FColor::Cyan, 0.0f, false);
DrawDebugString(GetWorld(), ContactPointLocation + FVector(0.0f, 0.0f, 50.0f), UMMTBPFunctionLibrary::GetPhysicalSurfaceRedableName(PhysicalSurface), nullptr, FColor::Cyan, 0.0f, false);
}




*/


/*
//Calculate Stopping Force to combat only effects of gravity. What we need to find is [m * g * sin(a)] for each point. What we know already is [m * g * cos(a)] which is projected
//suspension force - normal force. Note that we can't use mass of the whole vehicle as m as we don't know distribution of mass over multiple contact points.
//We assume standard direction of gravity.
FVector InverseGravityDirection = FVector(0.0f, 0.0f, 1.0f);
FVector GravityStoppingForce = FVector::ZeroVector;
float cosA = FVector::DotProduct(NormalForceAtContactPoint.GetSafeNormal(), InverseGravityDirection);
cosA = FMath::Abs(cosA);

//don't add any force if we are standing on flat surface
if (!FMath::IsNearlyZero(cosA, 0.0000001f))
{
//Extract m * g
float MassByGravity = MagnitudeNormalForceAtContactPoint / cosA;
float sinA = FMath::Sqrt(1.0f - (cosA * cosA));

//MassByGravity = EffectedComponentMesh->GetMass()*980.0f;

//Add stopping force to deal with gravity. Projecting inverse gravity vector on the slope, gives us vector parallel to slope direction
GravityStoppingForce = (InverseGravityDirection.VectorPlaneProject(InverseGravityDirection, ContactPointNormal).GetSafeNormal() * MassByGravity * sinA);// / NumberOfContactPoints;
}
*/
