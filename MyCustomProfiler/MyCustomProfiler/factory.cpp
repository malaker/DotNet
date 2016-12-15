#include "factory.h"




extern volatile unsigned int usageCount;

Factory::Factory()
{
	InterlockedIncrement(&usageCount);
	m_ref = 0;
}


Factory::~Factory()
{
	InterlockedDecrement(&usageCount);
}

ULONG STDMETHODCALLTYPE Factory::AddRef()
{
	InterlockedIncrement(&m_ref);
	return m_ref;
};

ULONG STDMETHODCALLTYPE Factory::Release()
{
	ULONG rv = InterlockedDecrement(&m_ref);
	if (rv == 0) delete this;
	return rv;
};

HRESULT STDMETHODCALLTYPE Factory::QueryInterface(REFIID id, void **ptr)
{
	if (ptr == NULL) return E_POINTER;
	if (IsBadWritePtr(ptr, sizeof(void*))) return E_POINTER;
	*ptr = NULL;
	if (id == IID_IUnknown) *ptr = this;
	if (id == IID_IClassFactory) *ptr = this;
	if (*ptr != NULL) { AddRef(); return S_OK; }
	return E_NOINTERFACE;
};

HRESULT STDMETHODCALLTYPE Factory::LockServer(BOOL v)
{
	if (v) InterlockedIncrement(&usageCount);//m_lock++;
	else InterlockedDecrement(&usageCount);//m_lock--;
	return S_OK;

};

HRESULT STDMETHODCALLTYPE Factory::CreateInstance(IUnknown *outer, REFIID id, void **ptr)
{
	MyCustomProfilerImpl *k;
	if (ptr == NULL) return E_POINTER;
	if (IsBadWritePtr(ptr, sizeof(void*))) return E_POINTER;
	*ptr = NULL;
	if (id == IID_IUnknown || id == CLSID_MyCustomProfiler || id == IID_ICorProfilerCallback4 || id == IID_ICorProfilerCallback2)
	{
		k = new MyCustomProfilerImpl();
		if (k == NULL) return E_OUTOFMEMORY;
		k->AddRef();
		*ptr = k;
		return S_OK;
	}
	
	return E_NOINTERFACE;
}

