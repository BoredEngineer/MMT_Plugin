//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTPawn.h"

// Sets default values
AMMTPawn::AMMTPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Make sure it ticks before physics update
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	// Bind function delegate
	OnCalculateCustomPhysics.BindUObject(this, &AMMTPawn::CustomPhysics);
}

// Called when the game starts or when spawned
void AMMTPawn::BeginPlay()
{
	PawnRootComponent = Cast<UPrimitiveComponent>(GetRootComponent());
	if (PawnRootComponent != NULL) {
		RootBodyInstance = PawnRootComponent->GetBodyInstance();
	}

	Super::BeginPlay();
}

// Called every frame
void AMMTPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Add custom physics on Root's BodyInstance
	if (RootBodyInstance != NULL) {
		RootBodyInstance->AddCustomPhysics(OnCalculateCustomPhysics);
	}

}


// Called to bind functionality to input
void AMMTPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

// Called by OnCalculateCustomPhysics delegate when physics update is initiated
void AMMTPawn::CustomPhysics(float DeltaTime, FBodyInstance* BodyInstance)
{
	MMTPawnTransform = MMTGetTransformThisPawn();
	MMTPhysicsTick(DeltaTime);
}

// Get instance of physics body from component
FBodyInstance * AMMTPawn::GetBodyInstance(UPrimitiveComponent * PrimitiveComponent)
{
	if (PrimitiveComponent == NULL) {
		return NULL;
	}
	return PrimitiveComponent->GetBodyInstance();
}

// Get MMTPawn transform from BodyInstance as its valid during physics sub-stepping
FTransform AMMTPawn::MMTGetTransformThisPawn()
{
	if (RootBodyInstance != NULL) {
		return RootBodyInstance->GetUnrealWorldTransform();
	}
	return FTransform::Identity;
}

