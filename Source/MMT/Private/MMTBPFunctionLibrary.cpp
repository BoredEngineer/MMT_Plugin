//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTBPFunctionLibrary.h"


//For UE4 Profiler ~ Stat
//DECLARE_CYCLE_STAT(TEXT("MMT ~ Get Transform Component"), STAT_MMTGetTransformComponent, STATGROUP_MMT);

UMMTBPFunctionLibrary::UMMTBPFunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// Get Component's world space transform from BodyInstance as its valid during physics sub-stepping
FTransform UMMTBPFunctionLibrary::MMTGetTransformComponent(USceneComponent *Target, FName InSocketName = NAME_None) 
{
	//Gather stats
	//SCOPE_CYCLE_COUNTER(STAT_MMTGetTransformComponent);

	if (Target == NULL) {
		return FTransform::Identity;
	}

	//Check if socket name is provided and store it's relative transform
	FTransform SocketRelativeTransform = FTransform::Identity;
	bool IsSocketRequested = false;
	if (InSocketName != NAME_None) {
		SocketRelativeTransform = Target->GetSocketTransform(InSocketName, ERelativeTransformSpace::RTS_Component);
		//Check if socket transform was found. GetSocketTransform() return transform of the parent when socket is not found.
		if (SocketRelativeTransform.Equals(Target->GetRelativeTransform()) == false) {
			//UE_LOG(LogTemp, Warning, TEXT("valid socket requested"));
			IsSocketRequested = true;
		}
	}

	//Check if component has a valid physics body to retrieve world transform. GetBodyInstanceTransform() returns parent transform if component's physics body is invalid.
	FTransform currentTransform = GetBodyInstanceTransform(Target);
	FTransform ParentTransform = GetBodyInstanceTransform(Target->GetAttachParent());
	if ((currentTransform.Equals(ParentTransform) == false) && (currentTransform.Equals(FTransform::Identity) == false)) {
		//UE_LOG(LogTemp, Warning, TEXT("Debug->return1"));
		return (IsSocketRequested ? SocketRelativeTransform * currentTransform : currentTransform);
	}

	//At this point we know that target component doesn't have valid physics body.
	//We need to find a parent in a chain, with valid physics body and accumulate all relative transforms leading to it.
	FTransform relativeTransform = Target->GetRelativeTransform();
	USceneComponent *currentScene = Target;

	while (true) {
		currentScene = currentScene->GetAttachParent();
		//We reached the top of hierarchy or current object simulates physics and therefore doesn't have a parent, as such objects are detached from the rest.
		if (currentScene == NULL) {
			//UE_LOG(LogTemp, Warning, TEXT("Debug->return2"));
			return (IsSocketRequested ? SocketRelativeTransform * Target->GetComponentTransform() : Target->GetComponentTransform());
		}
		currentTransform = GetBodyInstanceTransform(currentScene);
		ParentTransform = GetBodyInstanceTransform(currentScene->GetAttachParent());
		//We found a body in hierarchy which simulates physics. Now we can return final transform.
		if ((currentTransform.Equals(ParentTransform) == false) && (currentTransform.Equals(FTransform::Identity) == false)) {
			//UE_LOG(LogTemp, Warning, TEXT("Debug->return3"));
			return (IsSocketRequested ? SocketRelativeTransform * (relativeTransform * currentTransform) : relativeTransform * currentTransform);
		}
		relativeTransform *= currentScene->GetRelativeTransform();
	}
}

// Set Component transform on BodyInstance level, to be valid during physics sub-stepping
void UMMTBPFunctionLibrary::MMTSetTransformComponent(USceneComponent * Target, const FTransform& Transform, bool bTeleport)
{
	UPrimitiveComponent *primitiveComponent = Cast<UPrimitiveComponent>(Target);
	if (primitiveComponent == NULL) {
		return;
	}
	FBodyInstance *TargetBodyInstance = primitiveComponent->GetBodyInstance();

	TargetBodyInstance->SetBodyTransform(Transform, bTeleport ? ETeleportType::TeleportPhysics : ETeleportType::None);
}

// Get Actor transform from BodyInstance as its valid during physics sub-stepping
FTransform UMMTBPFunctionLibrary::MMTGetTransformActor(AActor * Actor)
{
	//UPrimitiveComponent* ActorRootComponent = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
	USceneComponent* ActorRootComponent = Actor->GetRootComponent();
	if (ActorRootComponent != NULL) {
		return GetBodyInstanceTransform(ActorRootComponent);
	}
	return FTransform::Identity;
}

// AddForce to a component using BodyInstance as its valid during physics sub-stepping
void UMMTBPFunctionLibrary::MMTAddForceComponent(UPrimitiveComponent * Target, const FVector & Force, bool bAccelChange)
{
	FBodyInstance* BodyInstance = GetBodyInstance(Target);
	if (BodyInstance != NULL) {
		BodyInstance->AddForce(Force, false, bAccelChange);
	}
}

// AddForceAtLocation to a component using BodyInstance as its valid during physics sub-stepping
void UMMTBPFunctionLibrary::MMTAddForceAtLocationComponent(UPrimitiveComponent * Target, const FVector & Force, const FVector & Location)
{
	FBodyInstance* BodyInstance = GetBodyInstance(Target);
	if (BodyInstance != NULL) {
		BodyInstance->AddForceAtPosition(Force, Location, false);
	}
}

// AddTorque to a component using BodyInstance as its valid during physics sub-stepping
void UMMTBPFunctionLibrary::MMTAddTorqueComponent(UPrimitiveComponent * Target, const FVector & Torque, bool bAccelChange)
{
	FBodyInstance* BodyInstance = GetBodyInstance(Target);

	if (BodyInstance != NULL) {
		BodyInstance->AddTorque(Torque, false, bAccelChange);
	}
}

// AddTorque to a component using BodyInstance as its valid during physics sub-stepping
void UMMTBPFunctionLibrary::MMTSetInertiaTensor(UPrimitiveComponent * Target, const FVector& InertiaTensor)
{
	FBodyInstance* BodyInstance = GetBodyInstance(Target);
	if (BodyInstance != NULL) {

		PxRigidBody* PRigidBody = BodyInstance->GetPxRigidBody_AssumesLocked();

		if (PRigidBody != NULL) {

			PRigidBody->setMassSpaceInertiaTensor(U2PVector(InertiaTensor));

		}
	}
}






// Return mesh component reference by finding it by name
UMeshComponent* UMMTBPFunctionLibrary::GetMeshComponentReferenceByName(UActorComponent* Target, FString Name)
{
	if (IsValid(Target))
	{
		AActor* Owner = Target->GetOwner();
		TArray<UActorComponent*> FoundComponents = Owner->GetComponentsByClass(UMeshComponent::StaticClass());

		UMeshComponent* Result;

		for (int32 i = 0; i < FoundComponents.Num(); i++)
		{
			if (FoundComponents[i]->GetName() == Name)
			{
				Result = Cast<UMeshComponent>(FoundComponents[i]);
				if (IsValid(Result))
				{
					return Result;
				}
			}
		}
	}
	return nullptr;
}

// Return spline component reference by finding it by name
USplineComponent* UMMTBPFunctionLibrary::GetSplineComponentReferenceByName(UActorComponent* Target, FString Name)
{
	if (IsValid(Target))
	{
		AActor* Owner = Target->GetOwner();
		TArray<UActorComponent*> FoundComponents = Owner->GetComponentsByClass(USplineComponent::StaticClass());

		USplineComponent* Result;

		for (int32 i = 0; i < FoundComponents.Num(); i++)
		{
			if (FoundComponents[i]->GetName() == Name)
			{
				Result = Cast<USplineComponent>(FoundComponents[i]);
				if (IsValid(Result))
				{
					return Result;
				}
			}
		}
	}
	return nullptr;
}

// Return instanced static mesh component reference by finding it by name
UInstancedStaticMeshComponent* UMMTBPFunctionLibrary::GetInstancedStaticMeshComponentReferenceByName(UActorComponent* Target, FString Name)
{
	if (IsValid(Target))
	{
		AActor* Owner = Target->GetOwner();
		TArray<UActorComponent*> FoundComponents = Owner->GetComponentsByClass(UInstancedStaticMeshComponent::StaticClass());

		UInstancedStaticMeshComponent* Result;

		for (int32 i = 0; i < FoundComponents.Num(); i++)
		{
			if (FoundComponents[i]->GetName() == Name)
			{
				Result = Cast<UInstancedStaticMeshComponent>(FoundComponents[i]);
				if (IsValid(Result))
				{
					return Result;
				}
			}
		}
	}
	return nullptr;
}

void UMMTBPFunctionLibrary::SetLinearBreakThreshold(UPhysicsConstraintComponent* Target, const float LinearBreakThreshold)
{
	if (IsValid(Target))
	{
		Target->ConstraintInstance.ProfileInstance.bLinearBreakable = true;
		Target->ConstraintInstance.ProfileInstance.LinearBreakThreshold = LinearBreakThreshold;
		Target->InitComponentConstraint(); //this will re-create joint and apply new settings
	}
}


// X and Y coefficients of friction define friction coefficient only in respective axes, for intermediate directions, value of friction coefficient needs to be interpolated.
// We use points on ellipse to as interpolated coefficients and X and Y friction coefficients define radii of the ellipse.
void UMMTBPFunctionLibrary::GetMuFromFrictionElipse(const FVector& VelocityDirectionNormalizedWS, const FVector& ForwardVectorWS, const float MuXStatic, const float MuXKinetic, const float MuYStatic, const float MuYKinetic,
	float& MuStatic, float& MuKinetic) //output variables
{
	float VdotX = FVector::DotProduct(VelocityDirectionNormalizedWS, ForwardVectorWS);
	float SqrtOneMinusVdotX2 = sqrtf(1.0f - VdotX * VdotX);
	
	FVector2D MuStaticVect = FVector2D(VdotX * MuXStatic, SqrtOneMinusVdotX2 * MuYStatic);
	FVector2D MuKineticVect = FVector2D(VdotX * MuXKinetic, SqrtOneMinusVdotX2 * MuYKinetic);

	MuStatic = MuStaticVect.Size();
	MuKinetic = MuKineticVect.Size();
}


// Get instance of physics body from component
FBodyInstance * UMMTBPFunctionLibrary::GetBodyInstance(UPrimitiveComponent * PrimitiveComponent)
{
	if (PrimitiveComponent == NULL) {
		return NULL;
	}
	return PrimitiveComponent->GetBodyInstance();
}

// Get transform from physics body of component
FTransform UMMTBPFunctionLibrary::GetBodyInstanceTransform(USceneComponent *SceneComponent) {
	UPrimitiveComponent *primitiveComponent = Cast<UPrimitiveComponent>(SceneComponent);
	if (primitiveComponent == NULL) {
		return FTransform::Identity;
	}
	return primitiveComponent->GetBodyInstance()->GetUnrealWorldTransform();
}