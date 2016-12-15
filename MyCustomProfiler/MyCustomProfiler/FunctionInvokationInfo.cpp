#include "FunctionInvokationInfo.h"

FunctionInvokationInfo::FunctionInvokationInfo(FunctionID id, std::wstring name)
{
	this->m_id = id;
	this->m_name = name;
}

FunctionInvokationInfo::~FunctionInvokationInfo()
{
}

std::wstring FunctionInvokationInfo::getName() {
	return this->m_name;
}

void FunctionInvokationInfo::SetFunctionInfo(ICorProfilerInfo2* ptr) {
	IMetaDataImport *import;
	mdToken token;
	HRESULT hr;
	bool isfail = false;
	hr = ptr->GetFunctionInfo(this->m_id, &this->m_classId, &this->m_moduleId, &this->m_token);
	if (hr != S_OK) {
		isfail = true;
	}
	hr = ptr->GetILFunctionBody(this->m_moduleId, this->m_token, &m_bytesMethod, &this->m_methodSize);
	if (hr != S_OK) {
		isfail = true;
	}
	hr = ptr->GetTokenAndMetaDataFromFunction(this->m_id, IID_IMetaDataImport, (IUnknown **)&import, &token);
	if (hr != S_OK) {
		isfail = true;
	}
	char buffer[1024];
	mdTypeDef classToken = mdTypeDefNil;
	DWORD methodAttr = 0;
	PCCOR_SIGNATURE sigBlob = NULL;
	ULONG sigBytes;
	const ULONG MAX_LENGTH = 1024;
	WCHAR funName[MAX_LENGTH] = L"UNKNOWN";

	hr = import->GetMethodProps(this->m_token,
		&classToken,
		funName,
		MAX_LENGTH,
		0,
		&methodAttr,
		&sigBlob,
		&sigBytes,
		NULL,
		NULL);

	if (hr != S_OK) {
		isfail = true;
	}

	// calling convention, argument count and return type 
	ULONG callConv = IMAGE_CEE_CS_CALLCONV_MAX;

	sigBlob += CorSigUncompressData(sigBlob, &callConv);
	sigBlob += CorSigUncompressData(sigBlob, &(this->m_argCount));

	HCORENUM paramEnum = NULL;
	mdParamDef* paramDefs = new mdParamDef[this->m_argCount];
	ULONG numParams = 0;
	hr = import->EnumParams(
		&paramEnum,
		this->m_token,
		paramDefs,
		this->m_argCount, &numParams);
	if (hr != S_OK) {
		isfail = true;
	}
	if (paramEnum)
		import->CloseEnum(paramEnum);

	WCHAR FRETURNTYPENAME[1024];
	
	bool fisRef;
	mdTypeDef ftypeDef;
	bool fisArray;

	this->m_type = this->GetType(sigBlob, fisRef, ftypeDef, fisArray);

	hr = this->GetTypeName(FRETURNTYPENAME, 1024, this->m_type, ftypeDef, import);
	this->m_returnTypeName = std::wstring(FRETURNTYPENAME);
	if (hr != S_OK) {
		isfail = true;
	}

	for (int i = 0; i < this->m_argCount; i++) {
		ParamMethodInfo pInfo;

		bool isRef;
		mdTypeDef typeDef;
		bool isArray;

		pInfo.m_type = this->GetType(sigBlob, isRef, typeDef, isArray);

		WCHAR NAME[1024];

		hr = this->GetTypeName(NAME, 1024, pInfo.m_type, typeDef, import);

		if (hr != S_OK) {
			isfail = true;
		}
		WCHAR PNAME[1024];

		hr = import->GetParamProps(
			paramDefs[i], NULL, NULL,
			PNAME, 1024, NULL, &pInfo.m_attributes
			, NULL, NULL, NULL);
		pInfo.paramToken = paramDefs[i];
		pInfo.m_typeName = std::wstring(NAME);
		pInfo.m_paramName = std::wstring(PNAME);
		pInfo.m_isByRef = isRef;
		pInfo.m_isArray = isArray;
		pInfo.typeOfParametrToken = typeDef;
		
		this->paraminfo.push_back(pInfo);
	}

	delete[] paramDefs;
	import->Release();

}

int FunctionInvokationInfo::GetArgCount() {
	return this->m_argCount;
}
ParamMethodInfo FunctionInvokationInfo::GetParamInfo(int index) {
	return this->paraminfo.at(index);
}

FunctionID FunctionInvokationInfo::GetFunctionID() {
	return this->m_id;
}
ClassID FunctionInvokationInfo::GetClassID() {
	return this->m_classId;
}
mdToken FunctionInvokationInfo::GetToken() {
	return this->m_token;
}
ModuleID FunctionInvokationInfo::GetModuleID() {
	return this->m_moduleId;
}

LPCBYTE FunctionInvokationInfo::GetMethodBytes() {
	return this->m_bytesMethod;
}
ULONG FunctionInvokationInfo::GetMethodSize() {
	return this->m_methodSize;
}


CorElementType FunctionInvokationInfo::GetType(
	PCCOR_SIGNATURE& sigBlob,
	bool &isByRef,
	mdTypeDef &typeDef,
	bool &isArray)
{
	CorElementType type = (CorElementType)*sigBlob++;

	isByRef = (ELEMENT_TYPE_BYREF == type);

	if (isByRef)
		type = (CorElementType)*sigBlob++;

	isArray = (ELEMENT_TYPE_SZARRAY == type);

	if (isArray)
		type = (CorElementType)*sigBlob++;

	typeDef = mdTypeDefNil;

	if (ELEMENT_TYPE_VALUETYPE == type || ELEMENT_TYPE_CLASS == type)
	{
		sigBlob += CorSigUncompressToken(sigBlob, &typeDef);
	}

	return type;
}


HRESULT FunctionInvokationInfo::GetTypeName(
	PWCHAR name,
	ULONG size,
	CorElementType type,
	mdTypeDef typeDef,
	IMetaDataImport *mdi)
{
	HRESULT hr = S_OK;
	ULONG MAX_LENGTH = 1024;
	switch (type)
	{
	case ELEMENT_TYPE_VALUETYPE:
	case ELEMENT_TYPE_CLASS:
		if (TypeFromToken(typeDef) == mdtTypeRef)
		{
			hr = mdi->GetTypeRefProps(
				typeDef, NULL,
				name, MAX_LENGTH, NULL);
		}
		else
		{
			hr = mdi->GetTypeDefProps(
				typeDef,
				name, MAX_LENGTH, NULL, NULL, NULL);
		}
		if (!SUCCEEDED(hr))
		{
			name[0] = '\0';
		}
		break;
	default:
	{
		_GetNameFromElementType(type, name, size);
	}
	break;
	}

	return hr;
}


HRESULT FunctionInvokationInfo::_GetNameFromElementType(CorElementType elementType, __out_ecount(buflen) WCHAR *buffer, size_t buflen)
{
	HRESULT hr = S_OK;

	switch (elementType)
	{
	case ELEMENT_TYPE_BOOLEAN:
		//wcscpy_s( buffer, buflen, L"System.Boolean" );
		wcscpy_s(buffer, buflen, L"bool");
		break;

	case ELEMENT_TYPE_CHAR:
		//wcscpy_s( buffer, buflen, L"System.Char" );
		wcscpy_s(buffer, buflen, L"char");
		break;

	case ELEMENT_TYPE_I1:
		//wcscpy_s( buffer, buflen, L"System.SByte" );
		wcscpy_s(buffer, buflen, L"sbyte");
		break;

	case ELEMENT_TYPE_U1:
		//wcscpy_s( buffer, buflen, L"System.Byte" );
		wcscpy_s(buffer, buflen, L"byte");
		break;

	case ELEMENT_TYPE_I2:
		//wcscpy_s( buffer, buflen, L"System.Int16" );
		wcscpy_s(buffer, buflen, L"short");
		break;

	case ELEMENT_TYPE_U2:
		//wcscpy_s( buffer, buflen, L"System.UInt16" );
		wcscpy_s(buffer, buflen, L"ushort");
		break;

	case ELEMENT_TYPE_I4:
		//wcscpy_s( buffer, buflen, L"System.Int32" );
		wcscpy_s(buffer, buflen, L"int");
		break;

	case ELEMENT_TYPE_U4:
		//wcscpy_s( buffer, buflen, L"System.UInt32" );
		wcscpy_s(buffer, buflen, L"uint");
		break;

	case ELEMENT_TYPE_I8:
		//wcscpy_s( buffer, buflen, L"System.Int64" );
		wcscpy_s(buffer, buflen, L"long");
		break;

	case ELEMENT_TYPE_U8:
		//wcscpy_s( buffer, buflen, L"System.UInt64" );
		wcscpy_s(buffer, buflen, L"ulong");
		break;

	case ELEMENT_TYPE_R4:
		//wcscpy_s( buffer, buflen, L"System.Single" );
		wcscpy_s(buffer, buflen, L"float");
		break;

	case ELEMENT_TYPE_R8:
		//wcscpy_s( buffer, buflen, L"System.Double" );
		wcscpy_s(buffer, buflen, L"double");
		break;

	case ELEMENT_TYPE_STRING:
		//wcscpy_s( buffer, buflen, L"System.String" );
		wcscpy_s(buffer, buflen, L"string");
		break;

	case ELEMENT_TYPE_PTR:
		//wcscpy_s( buffer, buflen, L"System.IntPtr" );
		wcscpy_s(buffer, buflen, L"IntPtr");
		break;

	case ELEMENT_TYPE_VALUETYPE:
		wcscpy_s(buffer, buflen, L"struct");
		break;

	case ELEMENT_TYPE_CLASS:
		wcscpy_s(buffer, buflen, L"class");
		break;

	case ELEMENT_TYPE_ARRAY:
		//wcscpy_s( buffer, buflen, L"System.Array" );
		wcscpy_s(buffer, buflen, L"Array");
		break;

	case ELEMENT_TYPE_I:
		wcscpy_s(buffer, buflen, L"INT_PTR");
		break;

	case ELEMENT_TYPE_U:
		wcscpy_s(buffer, buflen, L"UINT_PTR");
		break;

	case ELEMENT_TYPE_OBJECT:
		//wcscpy_s( buffer, buflen, L"System.Object" );
		wcscpy_s(buffer, buflen, L"object");
		break;

	case ELEMENT_TYPE_SZARRAY:
		//wcscpy_s( buffer, buflen, L"System.Array" );
		wcscpy_s(buffer, buflen, L"Array");
		break;

	case ELEMENT_TYPE_VOID:
		wcscpy_s(buffer, buflen, L"void");
		break;
		break;

	case ELEMENT_TYPE_FNPTR:
	case ELEMENT_TYPE_MAX:
	case ELEMENT_TYPE_END:
	case ELEMENT_TYPE_BYREF:
	case ELEMENT_TYPE_PINNED:
	case ELEMENT_TYPE_SENTINEL:
	case ELEMENT_TYPE_CMOD_OPT:
	case ELEMENT_TYPE_MODIFIER:
	case ELEMENT_TYPE_CMOD_REQD:
	case ELEMENT_TYPE_TYPEDBYREF:
	default:
		wcscpy_s(buffer, buflen, L"?");
		break;
	}

	return hr;
} // ProfilerCallback::_GetNameFromElementType
