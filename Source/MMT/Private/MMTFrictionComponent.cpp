//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTBPFunctionLibrary.h"
#include "MMTFrictionComponent.h"

//Set default values
UMMTFrictionComponent::UMMTFrictionComponent()
{
	EffectedComponentName = FString("none");
	IsDebugMode = false;
	MuXStatic = 0.75f;
	MuXKinetic = 0.5f;
	MuYStatic = 1.0f;
	MuYKinetic = 0.75f;

	PhysicsSurfaceResponse.Add(FPhysicalSurfaceRollingFrictionCoefficient());
	//PhysicsSurfaceResponse.Add(FPhysicalSurfaceRollingFrictionCoefficient(EPhysicalSurface::SurfaceType_Default, 0.02f));


}

// Called when the game starts or when spawned
void UMMTFrictionComponent::BeginPlay()
{
	// Get references to named components
	GetComponentsReference();

	Super::BeginPlay();
}


// Register collision information for future processing in friction component
void UMMTFrictionComponent::RegisterFrictionPoint(const FVector& NormalImpulseAtPoint, const FVector& ContactPointLocation, const FVector& ContactPointNormal, 
													UPhysicalMaterial* PhysicalMaterial, const FVector InducedVelocity)
{
	FContactPointData NewContactPoint;
	TEnumAsByte<EPhysicalSurface> PhysicalSurfaceLoc = EPhysicalSurface::SurfaceType_Default;

	NewContactPoint.IsPointActive = true;
	NewContactPoint.InducedVelocity = InducedVelocity;
	NewContactPoint.NormalImpulseAtPoint = NormalImpulseAtPoint;
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

// Empties array of stored friction points, this needs to be done after physics sub-stepping or before new collision information come in
void UMMTFrictionComponent::ResetFrictionPoints()
{
	ContactPointsData.Empty();
}

// Set friction surface linear velocity
void UMMTFrictionComponent::SetFrictionSurfaceVelocity(const FVector& FrictionSurfaceVel)
{
	FrictionSurfaceVelocity = FrictionSurfaceVel;
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
void UMMTFrictionComponent::PhysicsUpdate(const float& NumberOfContactPoints, const float& DeltaTime, FVector& NormalizedReactionForce, FVector& RollingFrictionForce)
{
	FVector NormalizedReactionForceOut = FVector::ZeroVector;
	FVector RollingFrictionForceOut = FVector::ZeroVector;

	if (ContactPointsData.Num() > 0)
	{
		if (ContactPointsData[0].IsPointActive)
		{
			// Get physics sub-stepping valid transform of the friction component
			ReferenceFrameTransform = UMMTBPFunctionLibrary::MMTGetTransformComponent( this, NAME_None);
			
			// Collisions are often to happen on the edge of the collision mesh, we "project" points on the X axis of the friction component as if collision happened
			// on the center-line of the track
			FVector PointLocationCentered = ReferenceFrameTransform.InverseTransformPosition(ContactPointsData[0].ContactPointLocation);
			PointLocationCentered = ReferenceFrameTransform.TransformPosition(FVector(PointLocationCentered.X, 0.0f, PointLocationCentered.Z));

			FVector PointNormalCentered = ReferenceFrameTransform.InverseTransformVector(ContactPointsData[0].ContactPointNormal);
			PointNormalCentered = ReferenceFrameTransform.TransformVector(FVector(PointNormalCentered.X, 0.0f, PointNormalCentered.Z));

			FVector PreNormalForceCentered = ReferenceFrameTransform.InverseTransformVector(ContactPointsData[0].NormalImpulseAtPoint / DeltaTime);
			PreNormalForceCentered = ReferenceFrameTransform.TransformVector(FVector(PreNormalForceCentered.X, 0.0f, PreNormalForceCentered.Z));

			//Apply Friction calculations
			ApplyFriction(PointLocationCentered, PointNormalCentered, ContactPointsData[0].InducedVelocity, PreNormalForceCentered, ContactPointsData[0].PhysicalSurface,
				NumberOfContactPoints, DeltaTime, NormalizedReactionForce, RollingFrictionForce);
		}
		else
		{
			//Return zero vector if point is inactive
			NormalizedReactionForce = FVector::ZeroVector;
			RollingFrictionForce = FVector::ZeroVector;
		}
		
		// Remove first element as it was processed or wasn't active
		ContactPointsData.RemoveAt(0, 1, true);
	}
	else
	{
		//Return zero vector if no points are stored in array
		NormalizedReactionForce = FVector::ZeroVector;
		RollingFrictionForce = FVector::ZeroVector;
	}
}


// Runs calculations on friction component, applies friction force to effected component and returns reaction forces(forces that can effect track or a wheel)
void UMMTFrictionComponent::ApplyFriction(const FVector& ContactPointLocation, const FVector& ContactPointNormal, const FVector& InducedVelocity, const FVector& PreNormalForceAtPoint,
	const EPhysicalSurface& PhysicalSurface, const float& NumberOfContactPoints, const float& DeltaTime, FVector& NormalizedReactionForce, FVector& RollingFrictionForce)
{
	float NormalForceAtContactPoint = PreNormalForceAtPoint.ProjectOnTo(ContactPointNormal).Size();
	
	// Check if Effected Component Mesh reference is valid and escape early otherwise
	if (!IsValid(EffectedComponentMesh))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's EffectedComponentMesh reference is invalid!"), *GetOwner()->GetName(), *GetName()));
		UE_LOG(LogTemp, Warning, TEXT("%s->%s component's EffectedComponentMesh reference is invalid!"), *GetOwner()->GetName(), *GetName());
		NormalizedReactionForce = FVector::ZeroVector;
		RollingFrictionForce = FVector::ZeroVector;
		return;
	}
	
	//Find relative velocity of the friction surface and ground/another object at point
	FVector RelativeVelocityAtPoint = EffectedComponentMesh->GetPhysicsLinearVelocityAtPoint(ContactPointLocation) + InducedVelocity + FrictionSurfaceVelocity;
	RelativeVelocityAtPoint = RelativeVelocityAtPoint.VectorPlaneProject(RelativeVelocityAtPoint, ContactPointNormal);

	//filter out oscillations when vehicle is standing still but friction overshoots
	RelativeVelocityAtPoint = (PrevRelativeVelocityAtPoint + RelativeVelocityAtPoint) * 0.5;
	PrevRelativeVelocityAtPoint = RelativeVelocityAtPoint;
	
	// early exit if velocity is too low to consider as vehicle most likely standing still
	if (RelativeVelocityAtPoint.Size() < 1.0f)
	{
		NormalizedReactionForce = FVector::ZeroVector;
		RollingFrictionForce = FVector::ZeroVector;
		return;
	}

	//Calculate static and kinetic friction coefficients, taking into account velocity direction and friction ellipse
	float MuStatic;
	float MuKinetic;
	UMMTBPFunctionLibrary::GetMuFromFrictionElipse(RelativeVelocityAtPoint.GetSafeNormal(), ReferenceFrameTransform.TransformVector(FVector(1.0f, 1.0f, 1.0f)),
													MuXStatic, MuXKinetic, MuYStatic, MuYKinetic, MuStatic, MuKinetic);

	//Calculate "stopping force" which is amount of force necessary to completely remove velocity of the object
	FVector	StoppingForce = ((RelativeVelocityAtPoint * (-1.0f) * EffectedComponentMesh->GetMass()) / DeltaTime) / NumberOfContactPoints;

	//Static friction threshold
	float MuStaticByLoad = NormalForceAtContactPoint * MuStatic;

	//Friction Force that will be applied to effected mesh component
	FVector ApplicationForce;
	if (StoppingForce.Size() >= MuStaticByLoad)
	{
		ApplicationForce = StoppingForce.GetClampedToSize(0.0f, NormalForceAtContactPoint * MuKinetic);
	}
	else
	{
		ApplicationForce = StoppingForce.GetClampedToSize(0.0f, MuStaticByLoad);
	}

	//Apply friction force
	UMMTBPFunctionLibrary::MMTAddForceAtLocationComponent(EffectedComponentMesh, ApplicationForce, ContactPointLocation);

	//Calculate Reaction force
	NormalizedReactionForce = (ApplicationForce * (-1.0f)) / EffectedComponentMesh->GetMass();

	
	//Calculate Rolling Friction
	float RollingFrictionCoefficient = FPhysicalSurfaceRollingFrictionCoefficient().RollingFrictionCoefficient;
	
	for (int32 i = 0; i < PhysicsSurfaceResponse.Num(); i++)
	{
		if (PhysicsSurfaceResponse[i].PhysicalSurface == PhysicalSurface)
		{
			RollingFrictionCoefficient = PhysicsSurfaceResponse[i].RollingFrictionCoefficient;
			break;
		}
	}

	RollingFrictionForce = RelativeVelocityAtPoint.GetSafeNormal() * NormalForceAtContactPoint * RollingFrictionCoefficient;
}
