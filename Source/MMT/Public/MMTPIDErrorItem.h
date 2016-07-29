//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "MMTPIDErrorItem.generated.h"


USTRUCT(BlueprintType)
struct FPIDErrorItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT PID Controller")
	float MeasuredValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT PID Controller")
	float DeltaTime;

};