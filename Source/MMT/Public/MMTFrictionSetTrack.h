//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "MMTFrictionSetBase.h"
#include "MMTFrictionSetTrack.generated.h"




UCLASS()
class MMT_API UMMTFrictionSetTrack : public UMMTFrictionSetBase
{
	GENERATED_BODY()

public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		int32 MyTrackProperty;


};