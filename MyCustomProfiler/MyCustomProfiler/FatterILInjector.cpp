#include "FatterILInjector.h"



FatterILInjector::FatterILInjector(ICorProfilerInfo2* profilerInfo2, FunctionInvokationInfo* ptr, LPCBYTE methodBytes, ULONG methodSize) :IILInjector(ptr)
{
}


FatterILInjector::~FatterILInjector()
{
}

void FatterILInjector::WriteHeader(void * bytes)
{
	BYTE flags[] = { 0x03, 0x30, }; // not quite sure what this is. I think the "3" is CorILMethod_FatFormat
	WORD maxStackSize = 1 + (sizeof(ILCode) / 2); // rough estimation
	DWORD newSize = this->GetNewMethodSize();

	memcpy((BYTE*)bytes, &flags, sizeof(WORD));
	memcpy((BYTE*)bytes + sizeof(WORD), &maxStackSize, sizeof(WORD));
	memcpy((BYTE*)bytes + sizeof(WORD) + sizeof(WORD), &newSize, sizeof(DWORD));
}
