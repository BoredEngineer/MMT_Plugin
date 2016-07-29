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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID Settings", meta = (ClampMin = "3", UIMin = "3"))
	int32 NumOfErrorsToStore;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID Settings")
	float ProportionalCoefficient;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID Settings")
	float IntegralCoefficient;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID Settings")
	float DerivativeCoefficient;

	//UFUNCTION(BlueprintCallable, Category = "MMT PID Controller")
	static void AddMeasuredError(FPIDErrorItem* PIDErrorItem);

private:
	TArray<FPIDErrorItem*> MeasuredErrors;
};