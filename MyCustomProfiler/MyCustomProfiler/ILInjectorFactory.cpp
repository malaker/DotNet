#include "ILInjectorFactory.h"

IILInjector* ILInjectorFactory::CreateInjector(ICorProfilerInfo2* profilerInfo, FunctionInvokationInfo * functionInfo)
{
	LPCBYTE oldMethodBytes;
	ULONG oldMethodSize;
	oldMethodBytes = functionInfo->GetMethodBytes();
	oldMethodSize = functionInfo->GetMethodSize();
	if (((COR_ILMETHOD_TINY*)oldMethodBytes)->IsTiny())
	{
		if (oldMethodSize > (MAX_TINY_FORMAT_SIZE - sizeof(ILCode)))
		{

			return new FatterILInjector(profilerInfo, functionInfo, oldMethodBytes, oldMethodSize);
		}

		return new SmallILInjector(profilerInfo, functionInfo, oldMethodBytes, oldMethodSize);
	}
	else
	{
		return new FatILInjector(profilerInfo, functionInfo, oldMethodBytes, oldMethodSize);
	}
}

ILInjectorFactory::ILInjectorFactory()
{
}


ILInjectorFactory::~ILInjectorFactory()
{
}
