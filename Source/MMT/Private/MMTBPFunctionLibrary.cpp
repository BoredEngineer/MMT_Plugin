//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTBPFunctionLibrary.h"


UMMTBPFunctionLibrary::UMMTBPFunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// Get Component transform from BodyInstance as its valid during physics sub-stepping
FTransform UMMTBPFunctionLibrary::MMTGetTransformComponent(UPrimitiveComponent * Target)
{
	FBodyInstance* BodyInstance = GetBodyInstance(Target);
	if (BodyInstance != NULL) {
		return BodyInstance->GetUnrealWorldTransform();
	}
	return FTransform::Identity;
}

// Get Component transform from BodyInstance as its valid during physics sub-stepping
FTransform UMMTBPFunctionLibrary::MMTGetTransformComponentFull(USceneComponent *Target) {
	if (Target == NULL) {
		return FTransform::Identity;
	}

	FTransform currentTransform = GetBodyInstanceTransform(Target);
	//Maybe I'm still wrong about what GetBodyInstanceTransform() returns when body doesn't simulate physics.
	//But it doesn't return Indetity if body has collision mesh but doesn't simulate physics
	//if (currentTransform.Equals(FTransform::Identity) == false) {
	FTransform ParentTransform = GetBodyInstanceTransform(Target->GetAttachParent());
	if ((currentTransform.Equals(ParentTransform) == false) && (currentTransform.Equals(FTransform::Identity) == false)) {
		UE_LOG(LogTemp, Warning, TEXT("return1"));
		return currentTransform;
	}

	FTransform relativeTransform = Target->GetRelativeTransform();
	USceneComponent *currentScene = Target;

	while (true) {
		currentScene = currentScene->GetAttachParent();
		if (currentScene == NULL) {
			UE_LOG(LogTemp, Warning, TEXT("return2"));
			return Target->GetComponentTransform();
		}
		currentTransform = GetBodyInstanceTransform(currentScene);
		//Maybe I'm still wrong about what GetBodyInstanceTransform() returns when body doesn't simulate physics.
		//But it doesn't return Indetity if body has collision mesh but doesn't simulate physics
		ParentTransform = GetBodyInstanceTransform(currentScene->GetAttachParent());
		//if (currentTransform.Equals(FTransform::Identity) == false) {
		if ((currentTransform.Equals(ParentTransform) == false) && (currentTransform.Equals(FTransform::Identity) == false)) {
			UE_LOG(LogTemp, Warning, TEXT("return3"));
			return relativeTransform * currentTransform;
		}
		relativeTransform *= currentScene->GetRelativeTransform();
	}
}

// Get Component transform from BodyInstance as its valid during physics sub-stepping
FTransform UMMTBPFunctionLibrary::MMTGetTransformComponentWithSocket(USceneComponent *Target, FName InSocketName = NAME_None) {
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
			UE_LOG(LogTemp, Warning, TEXT("valid socket requested"));
			IsSocketRequested = true;
		}
	}

	//Check if component has a valid physics body to retrieve world transform. GetBodyInstanceTransform() returns parent transform if component's physics body is invalid.
	FTransform currentTransform = GetBodyInstanceTransform(Target);
	FTransform ParentTransform = GetBodyInstanceTransform(Target->GetAttachParent());
	if ((currentTransform.Equals(ParentTransform) == false) && (currentTransform.Equals(FTransform::Identity) == false)) {
		UE_LOG(LogTemp, Warning, TEXT("Debug->return1"));
		//return currentTransform;
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
			UE_LOG(LogTemp, Warning, TEXT("Debug->return2"));
			//return Target->GetComponentTransform();
			return (IsSocketRequested ? SocketRelativeTransform * Target->GetComponentTransform() : Target->GetComponentTransform());
		}
		currentTransform = GetBodyInstanceTransform(currentScene);
		ParentTransform = GetBodyInstanceTransform(currentScene->GetAttachParent());
		//We found a body in hierarchy which simulates physics. Now we can return final transform.
		if ((currentTransform.Equals(ParentTransform) == false) && (currentTransform.Equals(FTransform::Identity) == false)) {
			UE_LOG(LogTemp, Warning, TEXT("Debug->return3"));
			//return relativeTransform * currentTransform;
			return (IsSocketRequested ? SocketRelativeTransform * (relativeTransform * currentTransform) : relativeTransform * currentTransform);
		}
		relativeTransform *= currentScene->GetRelativeTransform();
	}
}

// Get Actor transform from BodyInstance as its valid during physics sub-stepping
FTransform UMMTBPFunctionLibrary::MMTGetTransformActor(AActor * Actor)
{
	UPrimitiveComponent* ActorRootComponent = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
	if (ActorRootComponent != NULL) {
		return ActorRootComponent->GetBodyInstance()->GetUnrealWorldTransform();
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

// Get instance of physics body from component
FBodyInstance * UMMTBPFunctionLibrary::GetBodyInstance(UPrimitiveComponent * PrimitiveComponent)
{
	if (PrimitiveComponent == NULL) {
		return NULL;
	}
	return PrimitiveComponent->GetBodyInstance();
}

// Get transform from physics body of component
FTransform UMMTBPFunctionLibrary::GetBodyInstanceTransform(USceneComponent *sceneComponent) {
	UPrimitiveComponent *primitiveComponent = Cast<UPrimitiveComponent>(sceneComponent);
	if (primitiveComponent == NULL) {
		return FTransform::Identity;
	}
	return primitiveComponent->GetBodyInstance()->GetUnrealWorldTransform();
}