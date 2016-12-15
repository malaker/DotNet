#include "IILInjector.h"

IILInjector::IILInjector(FunctionInvokationInfo* ptr)
{
	this->p_FunctionInvokationInfo = ptr;
}


IILInjector::~IILInjector()
{
}

void* IILInjector::GetFunctionBody() {
	void* newMethodBytes=this->AllocateNewMethodBody(p_FunctionInvokationInfo->GetFunctionID());
	WriteHeader(newMethodBytes);
	WriteNewILCode(newMethodBytes);
	WriteOldILCode(newMethodBytes);
	WriteExtraILCode(newMethodBytes);
	return newMethodBytes;
}

ULONG IILInjector::GetOldHeaderSize()
{
	return 0;
}

ULONG IILInjector::GetNewMethodSize()
{
	return this->p_FunctionInvokationInfo->GetMethodSize() + sizeof(ILCode);
}

void IILInjector::WriteNewILCode(void* bytes) {

}
void IILInjector::WriteOldILCode(void* bytes) {

}
void IILInjector::WriteExtraILCode(void* bytes) {
	
}
void* IILInjector::AllocateNewMethodBody(FunctionID functionID) {
	return NULL;
}
ULONG IILInjector::GetOldMethodBodySize() {
	return this->p_FunctionInvokationInfo->GetMethodSize();
}


 size_t IILInjector::GetHeaderSize() {
	return 0;
}
