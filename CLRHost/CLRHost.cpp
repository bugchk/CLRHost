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
#include <tchar.h>
#include <windows.h>

#include <metahost.h>
#include <mscoree.h>
#include <corerror.h>
#include <comdef.h>

#import "../x64/Debug/CLRHostAppDomainManager.tlb"

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

   LPCWSTR appDomainManagerTypename = L"CLRHostAppDomainManager.CLRHostAppDomainManager";
   LPCWSTR assemblyName = L"CLRHostAppDomainManager";
   hr = pCLRControl->SetAppDomainManagerType(assemblyName, appDomainManagerTypename);
   RETURN_ON_EFAIL(hr);

   wprintf(L"Running runtime version: %s\n", runtimeVersion);
   hr = pRuntimeHost->Start();
   RETURN_ON_EFAIL(hr);

   pAppDomainManager = pMyHostControl->GetDomainManagerForDefaultDomain();

   return ERROR_SUCCESS;
}


HRESULT 
CLRHostCreateAppDomain(OUT int * pDomainId)
{
    *pDomainId = pAppDomainManager->CreateAppDomain();
    return ERROR_SUCCESS;
}

HRESULT 
CLRHostDestroyAppDomain(IN int DomainId)
{
    pAppDomainManager->DestroyAppDomain(DomainId);
    return ERROR_SUCCESS;
}

HRESULT 
CLRHostLoadAssembly(IN int DomainId, IN LPSTR assemblyPath)
{
    pAppDomainManager->LoadAssembly(DomainId, assemblyPath);
    return ERROR_SUCCESS;
}

LONG_PTR
CLRHostRun(IN int DomainId, IN LPSTR symbolName, IN LONG_PTR parameter)
{
    return pAppDomainManager->Run(DomainId, symbolName, parameter);
}

HRESULT 
CLRHostExecute(IN int DomainId, IN LPSTR assemblyName)
{
    HRESULT hr;

   wprintf_s(L"--- Start ---\n");
   BSTR assemblyFilename = (TCHAR*)assemblyName;

   BSTR friendlyname = L"TestApp";
   hr = pAppDomainManager->Execute(assemblyFilename);
   RETURN_ON_EFAIL(hr);

   wprintf_s(L"--- End ---\n");

   return hr;
}

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