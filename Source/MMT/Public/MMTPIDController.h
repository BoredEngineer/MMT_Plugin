//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/ActorComponent.h"
#include "MMTPIDErrorItem.h"
#include "MMTPIDController.generated.h"

UCLASS(ClassGroup = (MMT), meta = (BlueprintSpawnableComponent))
class MMT_API UMMTPIDController : public UActorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component
	UMMTPIDController();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	* Minimum of 3 errors need to be stored. Storing more errors is necessary for Integral part of the controller
	* as it corrects systematic errors of measurements taken over time.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID Settings", meta = (ClampMin = "3", UIMin = "3"), meta = (ToolTip = "Minimum of 3 errors need to be stored. Storing more errors is necessary for Integral part of the controller as it corrects systematic errors of measurements taken over time."))
	int32 NumOfErrorsToStore;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID Settings", meta = (ToolTip = "Proportional coefficient is responsible for how fast error should be corrected"))
	float ProportionalCoefficient;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID Settings", meta = (ToolTip = "Integral coefficient is responsible for correcting systematic errors of measurement over time"))
	float IntegralCoefficient;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID Settings", meta = (ToolTip = "Derivative coefficient is responsible for correcting oscillation introduced by derivative coefficient"))
	float DerivativeCoefficient;

	/**
	*	Add measurement error and delta time to PID Controller's memory 
	*	@param PIDErrorItem	measurement error and delta time item
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT PID Controller")
	void AddMeasuredError(const FPIDErrorItem& PIDErrorItem);

	/**
	*	Get calculated control signal
	*	@return calculated control signal
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT PID Controller")
	float GetControlCoefficient();

	/**
	*	Resets PID Controller by deleting all previously measured errors
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT PID Controller")
	void ResetController();

private:
	UPROPERTY()
	TArray<FPIDErrorItem> MeasuredErrors;
};