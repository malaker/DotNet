#pragma once
#include "IILInjector.h"
class FatterILInjector :
	public IILInjector
{
public:
	FatterILInjector(ICorProfilerInfo2* profilerInfo2, FunctionInvokationInfo* ptr, LPCBYTE methodBytes, ULONG methodSize);
	~FatterILInjector();

	// Inherited via IILInjector
	virtual void WriteHeader(void * bytes) override;
	
};

