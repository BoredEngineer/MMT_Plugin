//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTPIDController.h"


// Sets default values
UMMTPIDController::UMMTPIDController()
{
	NumOfErrorsToStore = 5;
	ProportionalCoefficient = 0.1f;
	IntegralCoefficient = 0.0f;
	DerivativeCoefficient = 0.05f;
}


// Add error measurment into array
void UMMTPIDController::AddMeasuredError(FPIDErrorItem* PIDErrorItem)
{
	if(PIDErrorItem != NULL)
	{
		MeasuredErrors.Insert(PIDErrorItem, 0);
		
		// remove last item in array if we reached desired amount of errors to track
		if (MeasuredErrors.Num() > NumOfErrorsToStore)
		{
			MeasuredErrors.Pop(true);
		}
	}

}