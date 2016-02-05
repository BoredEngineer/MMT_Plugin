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
		return Target->GetBodyInstance()->GetUnrealWorldTransform();
	}
	return FTransform::Identity;
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