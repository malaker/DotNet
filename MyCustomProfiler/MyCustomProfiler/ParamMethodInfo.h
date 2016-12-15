#include "cor.h"
#include "corprof.h"
#include <string>
#pragma once


class ParamMethodInfo
{

public:
	bool m_isByRef;
	bool m_isArray;
	DWORD m_attributes;
	std::wstring m_paramName;
	std::wstring m_typeName;
	CorElementType m_type;
	ULONG GetElementTypeSize();
	mdParamDef paramToken;
	mdTypeDef typeOfParametrToken;
	ParamMethodInfo();
	~ParamMethodInfo();
};

