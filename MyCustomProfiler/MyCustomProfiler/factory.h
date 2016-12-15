#pragma once

#include <ObjBase.h>

#include "MyCustomProfilerImpl.h"

class Factory : public IClassFactory
{
public:
	Factory();
	~Factory();
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID id, void **rv);
	virtual HRESULT STDMETHODCALLTYPE LockServer(BOOL v);
	virtual HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown *outer, REFIID id, void **rv);

private:
	ULONG m_ref;
};

