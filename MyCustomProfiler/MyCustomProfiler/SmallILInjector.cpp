#include "SmallILInjector.h"



SmallILInjector::SmallILInjector(ICorProfilerInfo2* profilerInfo2, FunctionInvokationInfo* ptr, LPCBYTE methodBytes, ULONG methodSize):IILInjector(ptr)
{
}


SmallILInjector::~SmallILInjector()
{
}

void SmallILInjector::WriteHeader(void * bytes)
{
	BYTE newCodeSizeWithoutHeader = (this->GetNewMethodSize() << 2) | CorILMethod_TinyFormat;
	memcpy((BYTE*)bytes, &newCodeSizeWithoutHeader, this->GetHeaderSize());
}

size_t SmallILInjector::GetHeaderSize() {
	return SMALL_HEADER_SIZE;
}
