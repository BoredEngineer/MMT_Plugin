//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "MMTSecondaryTick.generated.h"


USTRUCT(BlueprintType)
struct FSecondaryTickFunction : public FTickFunction
{
	GENERATED_USTRUCT_BODY()

	class AMMTPawn* Target;

	MMT_API virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& CompletionGraphEvent) override;

	MMT_API virtual FString DiagnosticMessage() override;
};