// ----------------------------------------------------------------------------------------------
// Copyright (c) Chris Carr, spinlockdevelopment.com
// ----------------------------------------------------------------------------------------------
// This source code is subject to terms and conditions of the Microsoft Public License. A 
// copy of the license can be found in the License.html file at the root of this distribution. 
// If you cannot locate the Microsoft Public License, please send an email to 
// dlr@microsoft.com. By using this source code in any fashion, you are agreeing to be bound 
// by the terms of the Microsoft Public License.
// ----------------------------------------------------------------------------------------------
// You must not remove this notice, or any other, from this software.
// ----------------------------------------------------------------------------------------------

#pragma once

#include <windows.h>

typedef HRESULT (*_fCLRHostInitialize)();
typedef HRESULT (*_fCLRHostCreateAppDomain)(OUT int * pDomainId);
typedef HRESULT (*_fCLRHostDestroyAppDomain)(IN int DomainId);
typedef HRESULT (*_fCLRHostLoadAssembly)(IN int DomainId, IN LPSTR assemblyPath);
typedef LONG_PTR (*_fCLRHostRun)(IN int DomainId, IN LPSTR symbolName, IN LONG_PTR parameter);
typedef HRESULT (*_fCLRHostExecute)(IN int DomainId, IN LPCWSTR assemblyName);

static PVOID * pApi = nullptr;

FORCEINLINE 
HRESULT 
CLRHostInitialize()
{
    if (pApi != nullptr) return ERROR_SUCCESS;

    // leak this on purpose so it does not unload and destroy the runtime context
    HMODULE hClrHost = LoadLibraryA("CLRHost.dll");

    if (hClrHost == NULL) return E_NOINTERFACE;

    pApi = (PVOID*)GetProcAddress(hClrHost, (LPCSTR)1);

    if (pApi == nullptr) return E_FAIL;

    return ((_fCLRHostInitialize)pApi[0])();
}

FORCEINLINE 
HRESULT 
CLRHostCreateAppDomain(OUT int * pDomainId)
{
    return ((_fCLRHostCreateAppDomain)pApi[1])(pDomainId);
}

FORCEINLINE 
HRESULT 
CLRHostDestroyAppDomain(IN int DomainId)
{
    return ((_fCLRHostDestroyAppDomain)pApi[2])(DomainId);
}

FORCEINLINE 
HRESULT 
CLRHostLoadAssembly(IN int DomainId, IN LPSTR assemblyPath)
{
    return ((_fCLRHostLoadAssembly)pApi[3])(DomainId, assemblyPath);
}

FORCEINLINE 
LONG_PTR
CLRHostRun(IN int DomainId, IN LPSTR symbolName, IN LONG_PTR parameter)
{
    return ((_fCLRHostRun)pApi[4])(DomainId, symbolName, parameter);
}

FORCEINLINE 
HRESULT 
CLRHostExecute(IN int DomainId, IN LPCWSTR assemblyName)
{
    return ((_fCLRHostExecute)pApi[5])(DomainId, assemblyName);
}
