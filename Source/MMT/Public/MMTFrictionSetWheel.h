//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "MMTFrictionSetBase.h"
#include "MMTFrictionSetWheel.generated.h"

/**
*
*/
UCLASS()
class MMT_API UMMTFrictionSetWheel : public UMMTFrictionSetBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		int32 MyWheelProperty;


};