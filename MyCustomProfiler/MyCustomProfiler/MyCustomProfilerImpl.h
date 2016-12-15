
#pragma once

#include "CorProfilerCallbackImpl.h"
#include <wchar.h>
#define NAME_BUFFER_SIZE 1024
#include <map>
#include <string>
#include "FunctionInvokationInfo.h"
#import "mscorlib.tlb"

// {0ADAFE70-8F6C-4900-9B22-DEC4FAC5DD6A}
DEFINE_GUID(CLSID_MyCustomProfiler,
	0xadafe70, 0x8f6c, 0x4900, 0x9b, 0x22, 0xde, 0xc4, 0xfa, 0xc5, 0xdd, 0x6a);

#include <thread>
#include <chrono>
#include <mutex>
#include <queue>
#include <atomic>
#include <set>
#import "mqoa.dll"
#include <string> 
#include <sstream>

#include "ILInjectorFactory.h"

using namespace MSMQ;
using namespace std;
struct Msg {
	wstring Fname;
	int callStackLevel;
	wstring toString() {
		wstringstream ss;
		ss << "{";

		ss << "\"Fname\"" << ":" << "\"" << Fname << "\"";

		ss << ",";


		ss << "\"callStackLevel\"" << ":" << callStackLevel;

		ss << "}";
		wstring r;
		ss >> r;
		return r;
	}
};

extern std::mutex g_log_ptr_mutex;
extern std::mutex g_q_mutex;
extern std::queue<Msg> msgs;

extern std::condition_variable cv;
extern std::atomic<bool> ready;
extern std::atomic<bool> g_done;

class MyCustomProfilerImpl : public CorProfilerCallbackImpl
{
	int m_callstackLevel;
	std::map<FunctionID, FunctionInvokationInfo*> mapper;
	
	std::thread *consumer;
public:
	//tokens function to hook	
	std::set<mdToken> MethodsToHook;// = { 0x06000002 };
	void FinalRelease();
	static MyCustomProfilerImpl* g_pICorProfiler;

	HRESULT SendToDest(WCHAR *wszComputerName, WCHAR *wszQueueName, Msg &msg);
	void  AddMsg(Msg msg);
	void  Dequeue(Msg &msg);
	void  Send();
	virtual HRESULT STDMETHODCALLTYPE JITCompilationStarted(FunctionID functionId, BOOL fIsSafeToBlock) override;
	MyCustomProfilerImpl();
	~MyCustomProfilerImpl();

	void AddFunctionToMap(FunctionID key, FunctionInvokationInfo* val);
	std::map<FunctionID, FunctionInvokationInfo*>::iterator Find(FunctionID id);
	std::map<FunctionID, FunctionInvokationInfo*>::iterator GetMapperEnd();
	void Enter(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo);
	void Leave(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange);
	void Tailcall(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo);
	HRESULT GetFullMethodName(FunctionID functionID, LPWSTR wszMethod, int cMethod);
	virtual HRESULT STDMETHODCALLTYPE Initialize(IUnknown * pICorProfilerInfoUnk) override;
	virtual HRESULT STDMETHODCALLTYPE Shutdown() override;

	// the static function called by .Net when a function has been mapped to an ID
	static FunctionID MapFunction(FunctionID functionID, BOOL *pbHookFunction);
	void TraceParameters(COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo, FunctionInvokationInfo* functionInfo);
	void TraceParameter(COR_PRF_FUNCTION_ARGUMENT_RANGE *range,
		ParamMethodInfo *pParameterInfo,
		ICorProfilerInfo2 *cpi,
		IMetaDataImport *mdi);
	void TraceParameter(UINT_PTR startAddress, ICorProfilerInfo2 *cpi, IMetaDataImport* mdi, ParamMethodInfo *pinfo);
	void TraceString(ObjectID startAddress);
	void TraceInt(UINT_PTR startAddress);
	void ReplaceString(LPWSTR srcBuffer, DWORD stringLength);
};


