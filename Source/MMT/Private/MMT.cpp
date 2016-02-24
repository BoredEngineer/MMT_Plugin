//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMT.h"



void MMTImpl::StartupModule()
{
	//UE_LOG(LogActor, Log, TEXT("Machinery Modelling Toolkit initialized!"));
}
 
void MMTImpl::ShutdownModule()
{
	//UE_LOG(MMTLog, Log, TEXT("Machinery Modelling Toolkit ia shutting down..."));
}
 
IMPLEMENT_MODULE(MMTImpl, MMT)