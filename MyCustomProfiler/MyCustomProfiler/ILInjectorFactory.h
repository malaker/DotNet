#include "IILInjector.h"
#include "FunctionInvokationInfo.h"
#include <corhlpr.h>
#include "SmallILInjector.h"
#include "FatILInjector.h"
#include "FatterILInjector.h"

#pragma once
class ILInjectorFactory
{
public:
	IILInjector* CreateInjector(ICorProfilerInfo2* profilerInfo, FunctionInvokationInfo * functionInfo);	
	ILInjectorFactory();
	~ILInjectorFactory();
};

