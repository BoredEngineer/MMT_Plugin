#pragma once

#include "CoreMinimal.h"
#include "PhysicsPublic.h"

class AMMTPawn;

/**
 * Async callback from the Physics Engine
 */
class FMMTAsyncCallback : public Chaos::TSimCallbackObject<>
{
public:
	void SetMMTPawn(AMMTPawn* pawn) { MMTPawn = pawn; }
private:
	virtual void OnPreSimulate_Internal() override;
	AMMTPawn* MMTPawn;
};
