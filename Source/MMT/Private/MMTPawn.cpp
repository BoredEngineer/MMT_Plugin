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

	SecondaryTick.TickGroup = TG_PostPhysics;
	SecondaryTick.bCanEverTick = true;
	SecondaryTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void AMMTPawn::BeginPlay()
{
	PawnRootComponent = Cast<UPrimitiveComponent>(GetRootComponent());
	if (PawnRootComponent != NULL) {
		RootBodyInstance = PawnRootComponent->GetBodyInstance();
	}

	UWorld* World = GetWorld();
	if (World)
	{
		FPhysScene* PScene = World->GetPhysicsScene();
		if (PScene)
		{
			// Register physics step delegate
			OnPhysSceneStepHandle = PScene->OnPhysSceneStep.AddUObject(this, &AMMTPawn::PhysSceneStep);
		}
	}
	Super::BeginPlay();

	
	//if (!IsTemplate() && SecondaryTick.bCanEverTick)
	//{
		SecondaryTick.Target = this;
		SecondaryTick.SetTickFunctionEnable(SecondaryTick.bStartWithTickEnabled);
		SecondaryTick.RegisterTickFunction(GetLevel());
		//SecondaryTick.RegisterTickFunction(GetWorld()->GetLevel(0));
		//SecondaryTick.RegisterTickFunction(GetOwner()->GetLevel());
		
	//}
	
}

// Called whenever this actor is being removed from a level
void AMMTPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FPhysScene* PScene = World->GetPhysicsScene();
		if (PScene)
		{
			// Unregister physics step delegate
			PScene->OnPhysSceneStep.Remove(OnPhysSceneStepHandle);
		}
	}

	Super::EndPlay(EndPlayReason);
}

// Called every frame after physics update
void AMMTPawn::TickPostPhysics(	float DeltaSeconds,	ELevelTick TickType, FSecondaryTickFunction& ThisTickFunction)
{
	// Non-player update.
	const bool bShouldTick = ((TickType != LEVELTICK_ViewportsOnly) || GetOwner()->ShouldTickIfViewportsOnly());
	if (bShouldTick)
	{
		if (!IsPendingKill() && GetWorld())
		{
			//if (GetOwner()->GetWorldSettings() != NULL && !IsRunningDedicatedServer())
			if (GetWorldSettings() != NULL && !IsRunningDedicatedServer())
			{
				MMTAfterPhysicsTick(DeltaSeconds);
			}
		}
	}
}

// Called to bind functionality to input
void AMMTPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Called by OnPhysSceneStepHandle delegate when physics update is initiated
void AMMTPawn::PhysSceneStep(FPhysScene* PhysScene, uint32 SceneType, float DeltaTime)
{
	MMTPawnTransform = MMTGetTransformThisPawn();
	MMTPhysicsTick(DeltaTime);
}

void AMMTPawn::MMTPhysicsTick_Implementation(float SubstepDeltaTime)
{

}

// Get instance of physics body from component
FBodyInstance * AMMTPawn::GetBodyInstance(UPrimitiveComponent * PrimitiveComponent)
{
	if (PrimitiveComponent == NULL) {
		return NULL;
	}
	return PrimitiveComponent->GetBodyInstance();
}

void AMMTPawn::MMTAfterPhysicsTick_Implementation(float DeltaTime)
{

}

// Get MMTPawn transform from BodyInstance as its valid during physics sub-stepping
FTransform AMMTPawn::MMTGetTransformThisPawn()
{
	if (RootBodyInstance != NULL) {
		return RootBodyInstance->GetUnrealWorldTransform();
	}
	return FTransform::Identity;
}

