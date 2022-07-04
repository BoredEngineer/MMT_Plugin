#include "MMTAsyncCallback.h"
#include "PBDRigidsSolver.h"
#include "MMTPawn.h"

/**
 * Callback from Physics thread
 */
void FMMTAsyncCallback::OnPreSimulate_Internal()
{
	float DeltaTime = GetDeltaTime_Internal();

	Chaos::FPhysicsSolver* PhysicsSolver = static_cast<Chaos::FPhysicsSolver*>(GetSolver());
	if (PhysicsSolver == nullptr)
	{
		return;
	}
	
	// Physics scene ptr isn't used here, hence nullptr
	MMTPawn->PhysSceneStep(nullptr, DeltaTime);
}