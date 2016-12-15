#include "MyCustomProfilerImpl.h"

extern volatile unsigned int usageCount;

// this function simply forwards the FunctionEnter call the global profiler object
void __stdcall FunctionEnterGlobal(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *argInfo)
{
	// make sure the global reference to our profiler is valid
	if (MyCustomProfilerImpl::g_pICorProfiler != NULL)
		MyCustomProfilerImpl::g_pICorProfiler->Enter(functionID, clientData, frameInfo, argInfo);
}

// this function is called by the CLR when a function has been entered
void _declspec(naked) FunctionEnterNaked(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
{
	__asm
	{
		push    ebp                 // Create a standard frame
		mov     ebp, esp
		pushad                      // Preserve all registers

		mov     eax, [ebp + 0x14]      // argumentInfo
		push    eax
		mov     ecx, [ebp + 0x10]      // func
		push    ecx
		mov     edx, [ebp + 0x0C]      // clientData
		push    edx
		mov     eax, [ebp + 0x08]      // functionID
		push    eax
		call    FunctionEnterGlobal

		popad                       // Restore all registers
		pop     ebp                 // Restore EBP
		ret     16
	}
}

// this function simply forwards the FunctionLeave call the global profiler object
void __stdcall FunctionLeaveGlobal(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_RANGE *retvalRange)
{
	// make sure the global reference to our profiler is valid
	if (MyCustomProfilerImpl::g_pICorProfiler != NULL)
		MyCustomProfilerImpl::g_pICorProfiler->Leave(functionID, clientData, frameInfo, retvalRange);
}

// this function is called by the CLR when a function is exiting
void _declspec(naked) FunctionLeaveNaked(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *retvalRange)
{
	__asm
	{
		push    ebp                 // Create a standard frame
		mov     ebp, esp
		pushad                      // Preserve all registers

		mov     eax, [ebp + 0x14]      // argumentInfo
		push    eax
		mov     ecx, [ebp + 0x10]      // func
		push    ecx
		mov     edx, [ebp + 0x0C]      // clientData
		push    edx
		mov     eax, [ebp + 0x08]      // functionID
		push    eax
		call    FunctionLeaveGlobal

		popad                       // Restore all registers
		pop     ebp                 // Restore EBP
		ret     16
	}
}

// this function simply forwards the FunctionLeave call the global profiler object
void __stdcall FunctionTailcallGlobal(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo)
{
	if (MyCustomProfilerImpl::g_pICorProfiler != NULL)
		MyCustomProfilerImpl::g_pICorProfiler->Tailcall(functionID, clientData, frameInfo);
}

// this function is called by the CLR when a tailcall occurs.  A tailcall occurs when the 
// last action of a method is a call to another method.
void _declspec(naked) FunctionTailcallNaked(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO func)
{
	__asm
	{
		push    ebp                 // Create a standard frame
		mov     ebp, esp
		pushad                      // Preserve all registers

		mov     eax, [ebp + 0x14]      // argumentInfo
		push    eax
		mov     ecx, [ebp + 0x10]      // func
		push    ecx
		mov     edx, [ebp + 0x0C]      // clientData
		push    edx
		mov     eax, [ebp + 0x08]      // functionID
		push    eax
		call    FunctionTailcallGlobal

		popad                       // Restore all registers
		pop     ebp                 // Restore EBP
		ret     16
	}
}


MyCustomProfilerImpl::MyCustomProfilerImpl()
{
	InterlockedIncrement(&usageCount);
	m_callstackLevel = 0;

}
void MyCustomProfilerImpl::FinalRelease()
{

}

// called when the profiler is being terminated by the CLR
STDMETHODIMP MyCustomProfilerImpl::Shutdown()
{
	using namespace std::chrono_literals;
	while (ready.load()) {
		{
			std::lock_guard<std::mutex> lk(g_q_mutex);
			ready.store(msgs.size() > 0);
		}
		std::this_thread::sleep_for(1s);
	}
	g_done = true;
	this->consumer->join();
	delete this->consumer;
	for (auto iter = mapper.begin(); iter != mapper.end(); iter++) {
		delete iter->second;
	}

	return S_OK;
}

MyCustomProfilerImpl::~MyCustomProfilerImpl()
{
	InterlockedDecrement(&usageCount);
}

// this function is called by the CLR when a function has been mapped to an ID
UINT_PTR _stdcall FunctionMapper(FunctionID functionID, BOOL *pbHookFunction)
{
	// make sure the global reference to our profiler is valid.  Forward this
	// call to our profiler object
	if (MyCustomProfilerImpl::g_pICorProfiler != NULL)
		MyCustomProfilerImpl::g_pICorProfiler->MapFunction(functionID, pbHookFunction);

	// we must return the function ID passed as a parameter
	return (UINT_PTR)functionID;
}

HRESULT STDMETHODCALLTYPE MyCustomProfilerImpl::Initialize(IUnknown * pICorProfilerInfoUnk)
{
	HRESULT hr;

	DWORD eventMask = (DWORD)(COR_PRF_MONITOR_ENTERLEAVE);
	eventMask |= (DWORD)(COR_PRF_ENABLE_FUNCTION_ARGS);
	eventMask |= (DWORD)(COR_PRF_ENABLE_FRAME_INFO);
	eventMask |= (DWORD)(COR_PRF_ENABLE_FUNCTION_RETVAL);


	MyCustomProfilerImpl::g_pICorProfiler = this;
	hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo2, (LPVOID*)&this->corProfiler2Ptr);
	if (SUCCEEDED(hr)) {
		corProfiler2Ptr->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FunctionEnterNaked,
			(FunctionLeave2*)&FunctionLeaveNaked, (FunctionTailcall2*)&FunctionTailcallNaked);
		corProfiler2Ptr->SetEventMask(eventMask);
		corProfiler2Ptr->SetFunctionIDMapper((FunctionIDMapper*)&FunctionMapper);
	}

	consumer = new std::thread(&MyCustomProfilerImpl::Send, this);

	if (SUCCEEDED(hr)) {
		return S_OK;
	}
	return E_NOINTERFACE;
}


std::map<FunctionID, FunctionInvokationInfo*>::iterator MyCustomProfilerImpl::Find(FunctionID id) {

	return this->mapper.find(id);
}

std::map<FunctionID, FunctionInvokationInfo*>::iterator MyCustomProfilerImpl::GetMapperEnd() {

	return this->mapper.end();
}

void MyCustomProfilerImpl::AddFunctionToMap(FunctionID key, FunctionInvokationInfo* val) {

	if (g_pICorProfiler->MethodsToHook.size() == 0 || g_pICorProfiler->MethodsToHook.find(val->GetToken()) != g_pICorProfiler->MethodsToHook.end()) {
		this->mapper.insert(std::pair<FunctionID, FunctionInvokationInfo*>(key, val));
	}
}

void MyCustomProfilerImpl::Enter(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo) {

	auto findIter = this->mapper.find(functionID);

	if (findIter != this->mapper.end()) {
		Msg msg;
		if (this->MethodsToHook.find(findIter->second->GetToken()) != this->MethodsToHook.end()) {
			TraceParameters(frameInfo, argumentInfo, findIter->second);
		}
		msg.Fname = bstr_t(findIter->second->getName().data());
		msg.callStackLevel = m_callstackLevel + 1;
		AddMsg(msg);
	}
	this->m_callstackLevel++;
}

void MyCustomProfilerImpl::TraceParameters(COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo, FunctionInvokationInfo* functionInfo) {

	// output parameters
	int offset = argumentInfo->numRanges - functionInfo->GetArgCount();
	HRESULT hr;
	mdMethodDef methodDef;
	IMetaDataImport *mdi = NULL;

	hr = corProfiler2Ptr->GetTokenAndMetaDataFromFunction(
		functionInfo->GetFunctionID(),
		IID_IMetaDataImport,
		(IUnknown**)&mdi,
		&methodDef);

	for (ULONG i = 0;
		i < functionInfo->GetArgCount();
		i++)
	{

		auto paramInfo = functionInfo->GetParamInfo(i);

		TraceParameter(
			&(argumentInfo->ranges[i + offset]), &paramInfo, this->corProfiler2Ptr, mdi);

	}
	mdi->Release();
}

void MyCustomProfilerImpl::TraceParameter(COR_PRF_FUNCTION_ARGUMENT_RANGE *range,
	ParamMethodInfo *pParameterInfo,
	ICorProfilerInfo2 *cpi,
	IMetaDataImport *mdi) {

	this->TraceParameter(
		pParameterInfo->m_isByRef ? *(PUINT_PTR)(range->startAddress) : range->startAddress,
		cpi, mdi,
		pParameterInfo);
}

void MyCustomProfilerImpl::TraceParameter(UINT_PTR startAddress, ICorProfilerInfo2 *cpi, IMetaDataImport* mdi, ParamMethodInfo *pParameterInfo) {
	switch (pParameterInfo->m_type)
	{
	case ELEMENT_TYPE_STRING:
		return TraceString(*(PUINT_PTR)startAddress);
	case ELEMENT_TYPE_I4:
		return TraceInt(startAddress);
		/*case ELEMENT_TYPE_BOOLEAN:
			return TraceBoolean(startAddress);
		case ELEMENT_TYPE_CHAR:
			return TraceChar(startAddress);

		case ELEMENT_TYPE_I8:
			return TraceLong(startAddress);
		case ELEMENT_TYPE_R4:
			return TraceFloat(startAddress);
		case ELEMENT_TYPE_R8:
			return TraceDouble(startAddress);*/
	}
}
void MyCustomProfilerImpl::TraceString(ObjectID oid) {
	if (oid == mdTokenNil || oid == 0xffffffff)
	{
		//got null;
		return;
	}

	ULONG bufferOffset = 0;
	ULONG stringLengthOffset = 0;
	ULONG bufferLengthOffset = 0;
	HRESULT hr = corProfiler2Ptr->GetStringLayout(&bufferLengthOffset, &stringLengthOffset, &bufferOffset);
	if (SUCCEEDED(hr))
	{
		LPWSTR buffer = (LPWSTR)(oid + bufferOffset);
		DWORD bufferLength = *((DWORD *)(oid + bufferLengthOffset));
		DWORD stringLength = *((DWORD *)(oid + stringLengthOffset));
		this->ReplaceString(buffer, stringLength);
	}
}

void MyCustomProfilerImpl::ReplaceString(LPWSTR srcBuffer, DWORD stringLength) {

	WCHAR* charSet =
		L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n";

	WCHAR *newString = new WCHAR[stringLength];
	for (int i = 0; i < stringLength; i++) {
		newString[i] = charSet[rand() % (50 - 1)];
	}
	wmemcpy(srcBuffer, newString, stringLength);
	delete[] newString;
}

void MyCustomProfilerImpl::TraceInt(UINT_PTR startAddress)
{
	int*ptr = (int *)startAddress;
	srand(startAddress);
	*ptr = rand();
}

void MyCustomProfilerImpl::Leave(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange) {

	if (this->m_callstackLevel > 0)
		this->m_callstackLevel--;
}
void MyCustomProfilerImpl::Tailcall(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo) {
	if (this->m_callstackLevel > 0)
		this->m_callstackLevel--;
}

HRESULT MyCustomProfilerImpl::GetFullMethodName(FunctionID functionID, LPWSTR wszMethod, int cMethod)
{
	IMetaDataImport* pIMetaDataImport = 0;
	HRESULT hr = S_OK;
	mdToken funcToken = 0;
	WCHAR szFunction[NAME_BUFFER_SIZE];
	WCHAR szClass[NAME_BUFFER_SIZE];

	// get the token for the function which we will use to get its name
	hr = this->corProfiler2Ptr->GetTokenAndMetaDataFromFunction(functionID, IID_IMetaDataImport, (LPUNKNOWN *)&pIMetaDataImport, &funcToken);
	if (SUCCEEDED(hr))
	{
		mdTypeDef classTypeDef;
		ULONG cchFunction;
		ULONG cchClass;

		// retrieve the function properties based on the token
		hr = pIMetaDataImport->GetMethodProps(funcToken, &classTypeDef, szFunction, NAME_BUFFER_SIZE, &cchFunction, 0, 0, 0, 0, 0);
		if (SUCCEEDED(hr))
		{
			// get the function name
			hr = pIMetaDataImport->GetTypeDefProps(classTypeDef, szClass, NAME_BUFFER_SIZE, &cchClass, 0, 0);
			if (SUCCEEDED(hr))
			{
				// create the fully qualified name
				_snwprintf_s(wszMethod, cMethod, cMethod, L"%s.%s", szClass, szFunction);
			}
		}
		// release our reference to the metadata
		pIMetaDataImport->Release();
	}

	return hr;
}

// the static function called by .Net when a function has been mapped to an ID
FunctionID MyCustomProfilerImpl::MapFunction(FunctionID functionID, BOOL *pbHookFunction)
{
	if (g_pICorProfiler != NULL) {
		// see if this function is in the map
		FunctionInvokationInfo* functionInfo = NULL;
		auto iter = g_pICorProfiler->Find(functionID);
		if (iter == g_pICorProfiler->GetMapperEnd())
		{
			// declared in this block so they are not created if the function is found
			WCHAR szMethodName[NAME_BUFFER_SIZE];
			const WCHAR* p = NULL;

			// get the method name
			HRESULT hr = g_pICorProfiler->GetFullMethodName(functionID, szMethodName, NAME_BUFFER_SIZE);
			if (FAILED(hr))
			{
				// if we couldn't get the function name, then log it

				return functionID;
			}
			// add it to the map
			auto name = std::wstring(szMethodName);
			functionInfo = new FunctionInvokationInfo(functionID, name);
			functionInfo->SetFunctionInfo(g_pICorProfiler->corProfiler2Ptr);		
			g_pICorProfiler->AddFunctionToMap(functionID, functionInfo);
			
		}
	}
	return functionID;
}

HRESULT MyCustomProfilerImpl::SendToDest(
	WCHAR *wszComputerName,
	WCHAR *wszQueueName,
	Msg &msg
)
{
	HRESULT hr = S_OK;

	// Validate the input strings.  
	if ((wszQueueName == NULL) || (wszComputerName == NULL))
	{
		return MQ_ERROR_INVALID_PARAMETER;
	}
	try
	{
		// Create smart pointers to IMSMQDestination and IMSMQMessage3 interfaces.  
		IMSMQDestinationPtr pDest(L"MSMQ.MSMQDestination");
		IMSMQMessage3Ptr pMsg(L"MSMQ.MSMQMessage");

		// Create a direct format name for the queue.  
		WCHAR * wszFormatName = NULL;
		DWORD dwFormatNameLength = 0;
		std::wstring name = std::wstring(L"DIRECT=OS:").append(wszComputerName).append(L"\\").append(L"PRIVATE$\\").append(wszQueueName);

		// Set the FormatName property of the MSMQDestination object to  
		// the direct format name created.  

		pDest->FormatName = name.c_str();

		// Set message properties, send the message, and close the MSMQDestination object.  

		pMsg->Label = L"Profiler Info";
		pMsg->Body = _variant_t(msg.toString().c_str());
		pMsg->Send(pDest);
		pDest->Close();

	}
	catch (const _com_error& comerr)
	{
		wprintf(L"Error Code = 0x%X\nError Description = %s\n",
			comerr.Error(), (WCHAR *)comerr.Description());
		return comerr.Error();
	}
	return hr;
}

void  MyCustomProfilerImpl::AddMsg(Msg msg) {
	std::lock_guard<std::mutex> lk(g_q_mutex);
	msgs.push(msg);
}

void  MyCustomProfilerImpl::Dequeue(Msg &msg) {
	std::lock_guard<std::mutex> lk(g_q_mutex);
	if (msgs.size() > 0) {
		msg = msgs.front();
		msgs.pop();
	}
	else {
		msg.Fname = L"";
	}
}

void  MyCustomProfilerImpl::Send() {
	CoInitialize(NULL);
	while (!g_done) {

		Msg msg;
		Dequeue(msg);

		if (msg.Fname.length() > 0) {
			SendToDest(L"kbalcero-MOBL1", L"testq", msg);

		}
	}
	CoUninitialize();
}

STDMETHODIMP MyCustomProfilerImpl::JITCompilationStarted(FunctionID functionId, BOOL fIsSafeToBlock)
{
	auto iterFunctionInfoInvokation = this->mapper.find(functionId);
	if (iterFunctionInfoInvokation != this->mapper.end()) {

		IMetaDataImport* meta;


		auto functionInfo = iterFunctionInfoInvokation->second;
		ILInjectorFactory *factory = new ILInjectorFactory();
		IILInjector *injector = NULL;
		injector = factory->CreateInjector(this->corProfiler2Ptr, functionInfo);
		void* newMethodBody = injector->GetFunctionBody();
		mdMethodDef methodDef;//TODO
		this->corProfiler2Ptr->SetILFunctionBody(functionInfo->GetModuleID(), methodDef, (LPCBYTE)newMethodBody);
		delete injector;
		delete factory;
	}
	return S_OK;
}




