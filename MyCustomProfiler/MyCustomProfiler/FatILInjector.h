#pragma once
#include "IILInjector.h"
class FatILInjector :
	public IILInjector
{
public:
	FatILInjector(ICorProfilerInfo2* profilerInfo2, FunctionInvokationInfo* ptr, LPCBYTE methodBytes, ULONG methodSize);
	~FatILInjector();
protected:
	// Inherited via IILInjector
	virtual void WriteHeader(void * bytes) override;
	void FixSEHSections(LPCBYTE methodBytes, ULONG newILSize);
	BOOL CanRewrite();
	ULONG GetDWORDAlignmentOffset();
	ULONG GetOldMethodSizeWithoutExtraSections();
	ULONG GetExtraSectionsSize();
	BOOL HasSEH();
	COR_ILMETHOD_FAT *GetFatInfo();
	size_t GetHeaderSize();
	
};



