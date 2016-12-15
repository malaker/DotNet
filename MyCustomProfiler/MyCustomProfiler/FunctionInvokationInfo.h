#pragma once

#include "cor.h"
#include "corprof.h"
#include <string>
#include "ParamMethodInfo.h"
#include <vector>
class FunctionInvokationInfo
{
	FunctionID m_id;
	int m_CalledTimes;
	std::wstring m_name;
	std::wstring m_returnTypeName;
	long m_timeElapsed;
	ModuleID m_moduleId;
	ClassID m_classId;
	mdToken m_token;
	LPCBYTE m_bytesMethod;
	ULONG m_methodSize;
	ULONG m_argCount;
	mdTypeDef m_methodDef;
	CorElementType m_type;
	std::vector<ParamMethodInfo> paraminfo;
public:
	FunctionInvokationInfo(FunctionID id, std::wstring name);
	~FunctionInvokationInfo();
	std::wstring getName();
	FunctionID GetFunctionID();
	ClassID GetClassID();
	mdToken GetToken();
	ModuleID GetModuleID();
	void SetFunctionInfo(ICorProfilerInfo2* ptr);
	LPCBYTE GetMethodBytes();
	ULONG GetMethodSize();
	CorElementType GetType(PCCOR_SIGNATURE& sigBlob,
		bool &isByRef,
		mdTypeDef &typeDef,
		bool &isArray);
	int GetArgCount();
	ParamMethodInfo GetParamInfo(int index);
	HRESULT GetTypeName(
		PWCHAR name,
		ULONG size,
		CorElementType type,
		mdTypeDef typeDef,
		IMetaDataImport *mdi);

	HRESULT _GetNameFromElementType(CorElementType elementType, __out_ecount(buflen) WCHAR *buffer, size_t buflen);
};

