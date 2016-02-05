//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "GameFramework/Pawn.h"
#include "MMTPawn.generated.h"

UCLASS()
class MMT_API AMMTPawn : public APawn
{
	GENERATED_BODY()
public:
	// Sets default values for this pawn's properties
	AMMTPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Pawn transform that is updated during sub-stepping
	UPROPERTY(BlueprintReadOnly)
	FTransform MMTPawnTransform = FTransform::Identity;

	// Delegate for runnung custom physics update
	FCalculateCustomPhysics OnCalculateCustomPhysics;
	void CustomPhysics(float DeltaTime, FBodyInstance* BodyInstance);

	/* This event is called on every physics tick, including sub-steps. Physics update is running in parallel thread, when sub-stepping is enabled don't call any non-physics methods, draw debug or print string functions or engine will crash. */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "MMT Physics Tick"), Category = "MMT physics sub-stepping")
	void MMTPhysicsTick(float SubstepDeltaTime);

	/**
	*	Get world-space transform of this pawn. BodyInstance is used to retrive transform, its up-to date with physics sub-stepping.
	*	@return			Pawn's transform in world space
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MMT Get Transform -Pawn"), Category = "MMT physics sub-stepping")
	FTransform MMTGetTransformThisPawn();


private:
	// Reference to MMTPawn root component
	UPROPERTY()
	UPrimitiveComponent* PawnRootComponent;
	
	FBodyInstance* RootBodyInstance;
	static FBodyInstance* GetBodyInstance(UPrimitiveComponent* PrimitiveComponent);

};