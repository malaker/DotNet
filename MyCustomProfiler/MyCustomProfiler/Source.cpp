#include <cor.h>
#include <corprof.h>
#include <new>
#include "factory.h"
#include "MyCustomProfilerImpl.h"
#include <Windows.h>


volatile unsigned int usageCount = 0;

MyCustomProfilerImpl* MyCustomProfilerImpl::g_pICorProfiler = NULL;
std::mutex g_log_ptr_mutex;
std::mutex g_q_mutex;
std::queue<Msg> msgs;
std::condition_variable cv;
std::atomic<bool> ready = true;
std::atomic<bool> g_done=false;

HRESULT __stdcall DllCanUnloadNow() {
	return usageCount > 0 ? S_FALSE : S_OK;
};


HRESULT __stdcall DllGetClassObject(REFCLSID cls, REFIID iid, void **ptr) {
	if (ptr == NULL) return E_INVALIDARG;
	*ptr = NULL;
	if (cls != CLSID_MyCustomProfiler) return CLASS_E_CLASSNOTAVAILABLE;
	if (iid != IID_IClassFactory) return CLASS_E_CLASSNOTAVAILABLE;
	Factory *fact = new (std::nothrow) Factory();
	if (fact == NULL) return E_OUTOFMEMORY;
	HRESULT rv = fact->QueryInterface(iid, ptr);
	if (FAILED(rv)) { delete fact; *ptr = NULL; };
	return rv;
};

static int SetRegKey(HKEY root, const wchar_t *keyname, const wchar_t *subname, const wchar_t *value)
{
	int i, len;
	HKEY key;
	DWORD rv;
	i = RegCreateKeyEx(root, keyname, 0, L"", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &rv);
	if (i != 0) return i;
	len = lstrlen(value) + 1;
	i = RegSetValueEx(key, subname, 0, REG_SZ, (const BYTE *)value, len * sizeof(wchar_t));
	RegCloseKey(key);
	return i;
};

HRESULT __stdcall DllRegisterServer()
{

	wchar_t clsid[128];
	wchar_t key[1024];
	StringFromGUID2(CLSID_MyCustomProfiler, clsid, 128);
	lstrcpy(key, L"CLSID\\");
	lstrcat(key, clsid);
	SetRegKey(HKEY_CLASSES_ROOT, key, NULL, L"Klasa");
	lstrcat(key, L"\\InProcServer32");
	SetRegKey(HKEY_CLASSES_ROOT, key, NULL, L"C:\\Users\\kbalcero\\Documents\\Visual Studio 2015\\Projects\\Win32Project1\\Debug\\Win32Project1.dll");
	SetRegKey(HKEY_CLASSES_ROOT, key, L"ThreadingModel", L"Both");
	return S_OK;

}

HRESULT __stdcall DllUnregisterServer()
{
	wchar_t clsid[128];
	wchar_t key[1024];
	StringFromGUID2(CLSID_MyCustomProfiler, clsid, 128);

	lstrcpy(key, L"CLSID\\");
	lstrcat(key, clsid);
	lstrcat(key, L"\\InProcServer32\\ThreadingModel");

	RegDeleteKey(HKEY_CLASSES_ROOT, key);

	lstrcpy(key, L"CLSID\\");
	lstrcat(key, clsid);
	lstrcat(key, L"\\InProcServer32");

	RegDeleteKey(HKEY_CLASSES_ROOT, key);

	lstrcpy(key, L"CLSID\\");
	lstrcat(key, clsid);

	RegDeleteKey(HKEY_CLASSES_ROOT, key);
	return S_OK;
}