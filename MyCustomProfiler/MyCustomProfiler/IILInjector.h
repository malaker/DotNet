#pragma once
#include <corhlpr.h>
#include "FunctionInvokationInfo.h"
#define FAT_HEADER_SIZE sizeof(WORD) + sizeof(WORD) + sizeof(DWORD) + sizeof(DWORD)
#define SMALL_HEADER_SIZE sizeof(BYTE)
#define MAX_TINY_FORMAT_SIZE 64


typedef struct
{
	BYTE nop;
	BYTE ldstr;
	BYTE stringToken[4];
	BYTE call;
	BYTE callToken[4];
} ILCode;

class IILInjector
{

public:
	IILInjector(FunctionInvokationInfo* pFunctionInvokationInfo);
	~IILInjector();
	virtual void* GetFunctionBody();
protected:
	FunctionInvokationInfo* p_FunctionInvokationInfo;
	virtual void WriteHeader(void* bytes) = 0;
	virtual void WriteNewILCode(void* bytes);
	virtual void WriteOldILCode(void* bytes);
	virtual void WriteExtraILCode(void* bytes);
	virtual void* AllocateNewMethodBody(FunctionID functionID);
	virtual ULONG GetOldMethodBodySize();
	virtual ULONG GetOldHeaderSize();
	virtual ULONG GetNewMethodSize();
	virtual size_t GetHeaderSize();
};


