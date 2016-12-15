#include "ParamMethodInfo.h"



ParamMethodInfo::ParamMethodInfo()
{
}


ParamMethodInfo::~ParamMethodInfo()
{
}


ULONG ParamMethodInfo::GetElementTypeSize()
{
	ULONG size = 0;
	switch (this->m_type)
	{
	case ELEMENT_TYPE_BOOLEAN:
		size = sizeof(bool);
		break;
	case ELEMENT_TYPE_CHAR:
	case ELEMENT_TYPE_I1:
	case ELEMENT_TYPE_U1:
		size = 1;
		break;

	case ELEMENT_TYPE_I2:
	case ELEMENT_TYPE_U2:
		size = 2;
		break;

	case ELEMENT_TYPE_I4:
	case ELEMENT_TYPE_U4:
		size = 4;
		break;

	case ELEMENT_TYPE_I8:
	case ELEMENT_TYPE_U8:
		size = 8;
		break;

	case ELEMENT_TYPE_R4:
		size = 4;
		break;

	case ELEMENT_TYPE_R8:
		size = 8;
		break;

	case ELEMENT_TYPE_STRING:
		size = sizeof(ObjectID);
		break;

	case ELEMENT_TYPE_PTR:
		size = sizeof(INT_PTR);
		break;

	case ELEMENT_TYPE_CLASS:
		size = sizeof(ObjectID);
		break;

	case ELEMENT_TYPE_I:
		size = sizeof(INT_PTR);
		break;

	case ELEMENT_TYPE_U:
		size = sizeof(UINT_PTR);
		break;

	case ELEMENT_TYPE_ARRAY:
	case ELEMENT_TYPE_VALUETYPE:
	case ELEMENT_TYPE_OBJECT:
	case ELEMENT_TYPE_SZARRAY:
	case ELEMENT_TYPE_VOID:
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
		size = -1;
		break;
	}

	return size;
} // ProfilerCallback::GetElementTypeSize

