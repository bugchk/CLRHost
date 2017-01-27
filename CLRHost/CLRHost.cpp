// ----------------------------------------------------------------------------------------------
// Original      : Copyright (c) Mattias Högström.
// Modifications : Copyright (c) Chris Carr, spinlockdevelopment.com
// ----------------------------------------------------------------------------------------------
// This source code is subject to terms and conditions of the Microsoft Public License. A 
// copy of the license can be found in the License.html file at the root of this distribution. 
// If you cannot locate the Microsoft Public License, please send an email to 
// dlr@microsoft.com. By using this source code in any fashion, you are agreeing to be bound 
// by the terms of the Microsoft Public License.
// ----------------------------------------------------------------------------------------------
// You must not remove this notice, or any other, from this software.
// ----------------------------------------------------------------------------------------------


#include <stdio.h>
#include <windows.h>

#include <metahost.h>
#include <mscoree.h>
#include <corerror.h>
#include <comdef.h>
#include <comutil.h>  

#import "CLRHostAppDomainManager.tlb"

using namespace CLRHostAppDomainManager;

#include "MinimalHostControl.h"

#define RUNTIME_VERSION L"v4.0.30319"

#define RETURN_ON_EFAIL(x) {if(FAILED(x)){return x;}}

ICLRMetaHost       *pMetaHost       = nullptr;
ICLRMetaHostPolicy *pMetaHostPolicy = nullptr;
ICLRRuntimeHost    *pRuntimeHost    = nullptr;
ICLRRuntimeInfo    *pRuntimeInfo    = nullptr;
ICLRControl        *pCLRControl     = nullptr;

MinimalHostControl          *pMyHostControl     = nullptr;
ICLRHostAppDomainManager    *pAppDomainManager  = nullptr;

HRESULT 
CLRHostInitialize()
{
   LPWSTR runtimeVersion = RUNTIME_VERSION;
   HRESULT hr;

   hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&pMetaHost);
   RETURN_ON_EFAIL(hr);

   hr = pMetaHost->GetRuntime(RUNTIME_VERSION, IID_PPV_ARGS(&pRuntimeInfo));
   RETURN_ON_EFAIL(hr);

   hr = pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost,IID_PPV_ARGS(&pRuntimeHost));         
   RETURN_ON_EFAIL(hr);

   pCLRControl = nullptr;
   hr = pRuntimeHost->GetCLRControl(&pCLRControl);
   RETURN_ON_EFAIL(hr);

   pMyHostControl = new MinimalHostControl();
   hr = pRuntimeHost->SetHostControl(pMyHostControl);
   RETURN_ON_EFAIL(hr);

   LPCWSTR assemblyName = L"CLRHostAppDomainManager";
   LPCWSTR appDomainManagerTypename = L"CLRHostAppDomainManager.CLRHostAppDomainManager";
   hr = pCLRControl->SetAppDomainManagerType(assemblyName, appDomainManagerTypename);
   RETURN_ON_EFAIL(hr);

   hr = pRuntimeHost->Start();
   RETURN_ON_EFAIL(hr);

   pAppDomainManager = pMyHostControl->GetDomainManagerForDefaultDomain();

   return ERROR_SUCCESS;
}

void
CLRHostDestroy()
{
	if (pRuntimeInfo != nullptr)
	{
		(VOID)pRuntimeHost->Stop();
		pRuntimeInfo->Release();
		pRuntimeInfo = nullptr;
	}
	if (pRuntimeHost != nullptr)
	{
		pRuntimeHost->Release();
		pRuntimeHost = nullptr;
	}
	if (pMetaHost != nullptr)
	{
		pMetaHost->Release();
		pMetaHost = nullptr;
	}
}

int 
CLRHostCreateAppDomain()
{
    return pAppDomainManager->CreateAppDomain();
}

BOOL
CLRHostDestroyAppDomain(IN int DomainId)
{
    return pAppDomainManager->DestroyAppDomain(DomainId);
}

BOOL
CLRHostLoadAssembly(IN int DomainId, IN LPSTR assemblyPath)
{
    return pAppDomainManager->LoadAssembly(DomainId, assemblyPath);
}

LONG_PTR
CLRHostRun(IN int DomainId, IN LPSTR symbolName, IN LONG_PTR parameter)
{
    return (LONG_PTR)pAppDomainManager->Run(DomainId, symbolName, parameter);
}

BSTR _char_bstr(LPCSTR str)
{
	_bstr_t bstr1(str);
	return bstr1.copy();
}

HRESULT 
CLRHostExecute(IN LPCSTR assemblyName, IN LPCSTR arguments)
{
   BSTR assemblyFilename = _char_bstr(assemblyName);
   BSTR execArgs = _char_bstr(arguments);
   return pAppDomainManager->Execute(assemblyFilename, execArgs);
}

HRESULT WINAPI
__declspec(dllexport)
DllCanUnloadNow(void) {	return S_FALSE; }

BOOLEAN 
WINAPI 
DllMain( 
    IN HINSTANCE hDllHandle, 
    IN DWORD     nReason, 
    IN LPVOID    Reserved
)
{
    switch ( nReason )
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls( hDllHandle );
            break;

        case DLL_PROCESS_DETACH:
            // THIS BELL CANNOT BE UNRUNG 
			CLRHostDestroy();
           break;
    }

    return TRUE;
}

extern PVOID pApi[] = {
    (PVOID)CLRHostInitialize, 
    (PVOID)CLRHostCreateAppDomain, 
    (PVOID)CLRHostDestroyAppDomain, 
    (PVOID)CLRHostLoadAssembly, 
    (PVOID)CLRHostRun, 
    (PVOID)CLRHostExecute, 
};