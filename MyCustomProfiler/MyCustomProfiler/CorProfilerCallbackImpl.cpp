#include "CorProfilerCallbackImpl.h"

CorProfilerCallbackImpl::CorProfilerCallbackImpl() {}

// Inherited via ICorProfilerCallback2

STDMETHODIMP CorProfilerCallbackImpl::QueryInterface(REFIID riid, void ** ptr)
{
	if (ptr == NULL) return E_POINTER;
	if (IsBadWritePtr(ptr, sizeof(void*))) return E_POINTER;
	*ptr = NULL;
	if (riid == IID_IUnknown) *ptr = this;
	if (riid == IID_ICorProfilerCallback2) *ptr = this;
	if (riid == IID_ICorProfilerCallback4) *ptr = this;

	if (riid == IID_IClassFactory) *ptr = this;
	if (*ptr != NULL) { 
		this->riid = riid;
		AddRef(); 
		return S_OK; 
	}
	return E_NOINTERFACE;
}
ULONG CorProfilerCallbackImpl::AddRef(void)
{
	ULONG rv = InterlockedIncrement(&counter);
	return rv;
}
ULONG CorProfilerCallbackImpl::Release(void)
{
	ULONG rv = InterlockedDecrement(&counter);
	if (rv == 0) delete this;
	return rv;
}

STDMETHODIMP CorProfilerCallbackImpl::Shutdown(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::AppDomainCreationStarted(AppDomainID appDomainId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::AppDomainCreationFinished(AppDomainID appDomainId, HRESULT hrStatus)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::AppDomainShutdownStarted(AppDomainID appDomainId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::AppDomainShutdownFinished(AppDomainID appDomainId, HRESULT hrStatus)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::AssemblyLoadStarted(AssemblyID assemblyId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::AssemblyLoadFinished(AssemblyID assemblyId, HRESULT hrStatus)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::AssemblyUnloadStarted(AssemblyID assemblyId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::AssemblyUnloadFinished(AssemblyID assemblyId, HRESULT hrStatus)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ModuleLoadStarted(ModuleID moduleId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ModuleLoadFinished(ModuleID moduleId, HRESULT hrStatus)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ModuleUnloadStarted(ModuleID moduleId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ModuleUnloadFinished(ModuleID moduleId, HRESULT hrStatus)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ModuleAttachedToAssembly(ModuleID moduleId, AssemblyID AssemblyId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ClassLoadStarted(ClassID classId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ClassLoadFinished(ClassID classId, HRESULT hrStatus)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ClassUnloadStarted(ClassID classId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ClassUnloadFinished(ClassID classId, HRESULT hrStatus)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::FunctionUnloadStarted(FunctionID functionId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::JITCompilationStarted(FunctionID functionId, BOOL fIsSafeToBlock)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::JITCompilationFinished(FunctionID functionId, HRESULT hrStatus, BOOL fIsSafeToBlock)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::JITCachedFunctionSearchStarted(FunctionID functionId, BOOL * pbUseCachedFunction)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::JITCachedFunctionSearchFinished(FunctionID functionId, COR_PRF_JIT_CACHE result)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::JITFunctionPitched(FunctionID functionId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::JITInlining(FunctionID callerId, FunctionID calleeId, BOOL * pfShouldInline) 
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ThreadCreated(ThreadID threadId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ThreadDestroyed(ThreadID threadId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ThreadAssignedToOSThread(ThreadID managedThreadId, DWORD osThreadId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RemotingClientInvocationStarted(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RemotingClientSendingMessage(GUID * pCookie, BOOL fIsAsync)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RemotingClientReceivingReply(GUID * pCookie, BOOL fIsAsync)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RemotingClientInvocationFinished(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RemotingServerReceivingMessage(GUID * pCookie, BOOL fIsAsync)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RemotingServerInvocationStarted(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RemotingServerInvocationReturned(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RemotingServerSendingReply(GUID * pCookie, BOOL fIsAsync)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::UnmanagedToManagedTransition(FunctionID functionId, COR_PRF_TRANSITION_REASON reason)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ManagedToUnmanagedTransition(FunctionID functionId, COR_PRF_TRANSITION_REASON reason)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RuntimeSuspendStarted(COR_PRF_SUSPEND_REASON suspendReason)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RuntimeSuspendFinished(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RuntimeSuspendAborted(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RuntimeResumeStarted(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RuntimeResumeFinished(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RuntimeThreadSuspended(ThreadID threadId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RuntimeThreadResumed(ThreadID threadId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::MovedReferences(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ObjectAllocated(ObjectID objectId, ClassID classId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ObjectsAllocatedByClass(ULONG cClassCount, ClassID classIds[], ULONG cObjects[])
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ObjectReferences(ObjectID objectId, ClassID classId, ULONG cObjectRefs, ObjectID objectRefIds[])
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RootReferences(ULONG cRootRefs, ObjectID rootRefIds[])
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionThrown(ObjectID thrownObjectId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionSearchFunctionEnter(FunctionID functionId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionSearchFunctionLeave(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionSearchFilterEnter(FunctionID functionId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionSearchFilterLeave(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionSearchCatcherFound(FunctionID functionId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionOSHandlerEnter(UINT_PTR __unused)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionOSHandlerLeave(UINT_PTR __unused)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionUnwindFunctionEnter(FunctionID functionId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionUnwindFunctionLeave(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionUnwindFinallyEnter(FunctionID functionId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionUnwindFinallyLeave(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionCatcherEnter(FunctionID functionId, ObjectID objectId)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionCatcherLeave(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::COMClassicVTableCreated(ClassID wrappedClassId, REFGUID implementedIID, void * pVTable, ULONG cSlots)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::COMClassicVTableDestroyed(ClassID wrappedClassId, REFGUID implementedIID, void * pVTable)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionCLRCatcherFound(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ExceptionCLRCatcherExecute(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::ThreadNameChanged(ThreadID threadId, ULONG cchName, WCHAR name[])
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::SurvivingReferences(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::GarbageCollectionFinished(void)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID)
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::RootReferences2(ULONG cRootRefs, ObjectID rootRefIds[], COR_PRF_GC_ROOT_KIND rootKinds[], COR_PRF_GC_ROOT_FLAGS rootFlags[], UINT_PTR rootIds[])
{
	return S_OK;
}
STDMETHODIMP CorProfilerCallbackImpl::HandleCreated(GCHandleID handleId, ObjectID initialObjectId)
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallbackImpl::HandleDestroyed(GCHandleID handleId)
{
	return S_OK;
}

// Inherited via ICorProfilerCallback4
STDMETHODIMP CorProfilerCallbackImpl::InitializeForAttach(IUnknown * pCorProfilerInfoUnk, void * pvClientData, UINT cbClientData)
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallbackImpl::ProfilerAttachComplete(void)
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallbackImpl::ProfilerDetachSucceeded(void)
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallbackImpl::ReJITCompilationStarted(FunctionID functionId, ReJITID rejitId, BOOL fIsSafeToBlock)
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallbackImpl::GetReJITParameters(ModuleID moduleId, mdMethodDef methodId, ICorProfilerFunctionControl * pFunctionControl)
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallbackImpl::ReJITCompilationFinished(FunctionID functionId, ReJITID rejitId, HRESULT hrStatus, BOOL fIsSafeToBlock)
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallbackImpl::ReJITError(ModuleID moduleId, mdMethodDef methodId, FunctionID functionId, HRESULT hrStatus)
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallbackImpl::MovedReferences2(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], SIZE_T cObjectIDRangeLength[])
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallbackImpl::SurvivingReferences2(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], SIZE_T cObjectIDRangeLength[])
{
	return S_OK;
}
