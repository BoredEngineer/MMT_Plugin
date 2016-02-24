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
FTransform UMMTBPFunctionLibrary::MMTGetTransformComponentFull(UPrimitiveComponent * Target)
{
	FBodyInstance* BodyInstance = GetBodyInstance(Target);
	if (BodyInstance != NULL) 
	{
		FTransform WorldTransform = BodyInstance->GetUnrealWorldTransform();

		FTransform RelativeTransform = Target->GetRelativeTransform();

		FBodyInstance * RootBodyInstance;
		FTransform RootWorldTransform;
		UPrimitiveComponent* RootComponent = Cast<UPrimitiveComponent>(Target->GetAttachmentRoot());
		
		if (RootComponent != NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Top component in hierarchy %s"), *RootComponent->GetName());
			RootBodyInstance = RootComponent->GetBodyInstance();
			RootWorldTransform = RootBodyInstance->GetUnrealWorldTransform(); 
			while (WorldTransform.Equals(RootWorldTransform, 0.0f)) 
			{
				UPrimitiveComponent* ParentComponent = Cast<UPrimitiveComponent>(Target->GetAttachParent());
				if (ParentComponent != NULL) 
				{
					UE_LOG(LogTemp, Warning, TEXT("Current level of hierarchy %s"), *Target->GetName());
					
					FBodyInstance * ParentBodyInstance = ParentComponent->GetBodyInstance();
					FTransform ParentBodyWorldTransform = ParentBodyInstance->GetUnrealWorldTransform();
					if (!ParentBodyWorldTransform.Equals(RootWorldTransform, 0.0f)) 
					{
						//we found component with valid physics transform
						WorldTransform = RelativeTransform * ParentBodyWorldTransform;
						UE_LOG(LogTemp, Warning, TEXT("return1"));
						return WorldTransform;
					}
					else 
					{
						//get relative transform and accumulate for next interation
						RelativeTransform = RelativeTransform * ParentComponent->GetRelativeTransform();
					}
					//we reached top of hierachy
					if (RootBodyInstance == ParentBodyInstance) 
					{
						UE_LOG(LogTemp, Warning, TEXT("Reached top of hierarchy %s"), *ParentComponent->GetName());
						WorldTransform = RootWorldTransform;
						break;
					}
					Target = ParentComponent;
				}
				else 
				{
					UE_LOG(LogTemp, Warning, TEXT("MMT Warning: Parent Component in %s is not a Primitive Component, result is Identity transform. "), *Target->GetName());
					return WorldTransform;
				}

			}
		}
		else{
			UE_LOG(LogTemp, Warning, TEXT("MMT Warning: Root Component in %s is not a Primitive Component, result is Identity transform. "), *Target->GetName());
			return WorldTransform;
		}
		UE_LOG(LogTemp, Warning, TEXT("return2"));
		return WorldTransform;
	}
	UE_LOG(LogTemp, Warning, TEXT("MMT Warning: MMTGetTransformComponent()-> BodyInstance on component %s is not found, result is Identity transform"), *Target->GetName());
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