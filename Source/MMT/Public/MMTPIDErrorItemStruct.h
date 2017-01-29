//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "MMTPIDErrorItemStruct.generated.h"


USTRUCT(BlueprintType)
struct FPIDErrorItemStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT PID Controller")
	float MeasuredValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT PID Controller")
	float DeltaTime;

	//Default constructor
	FPIDErrorItemStruct()
	{
		MeasuredValue = 0.0f;
		DeltaTime = 0.0f;
	}
};