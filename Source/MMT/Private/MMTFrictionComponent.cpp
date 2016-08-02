//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTFrictionComponent.h"
#include "MMTBPFunctionLibrary.h"

//Set default values
UMMTFrictionComponent::UMMTFrictionComponent()
{
	EffectedComponentName = FString("none");
	IsDebugMode = false;
	MuXStatic = 0.75f;
	MuXKinetic = 0.5f;
	MuYStatic = 1.0f;
	MuYKinetic = 0.75f;

	PhysicsSurfaceResponse.Add(FPhysicalSurfaceRollingFrictionCoefficient(EPhysicalSurface::SurfaceType_Default, 0.02f));


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
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's EffectedComponentName property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName()));
		UE_LOG(LogTemp, Warning, TEXT("%s->%s component's EffectedComponentName property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName());
	}
}