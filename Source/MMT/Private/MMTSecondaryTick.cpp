//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTPawn.h"
#include "MMTSecondaryTick.h"


void FSecondaryTickFunction::ExecuteTick( float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& CompletionGraphEvent)
{
	//if (Target && !Target->HasAnyFlags(RF_PendingKill | RF_Unreachable))
	//if (Target && !Target->HasAnyFlags(RF_BeginDestroyed) && !Target->IsValidLowLevel())
	if (Target && !Target->IsPendingKillOrUnreachable())
	{
		FScopeCycleCounterUObject ActorScope(Target);
		Target->TickPostPhysics(DeltaTime, TickType, *this);
	}
}

FString FSecondaryTickFunction::DiagnosticMessage()
{
	//return Target->GetFullName() + TEXT("[MyComponent Post Physics Tick]");
	return FString("MyComponent Post Physics Tick");
}