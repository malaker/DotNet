#pragma once
#include "IILInjector.h"
class SmallILInjector :
	public IILInjector
{
public:
	SmallILInjector(ICorProfilerInfo2* profilerInfo2, FunctionInvokationInfo* ptr, LPCBYTE methodBytes, ULONG methodSize);
	~SmallILInjector();
protected:
	// Inherited via IILInjector
	virtual void WriteHeader(void * bytes) override;
	size_t GetHeaderSize();
};

