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


#include <stdio.h>
#include <windows.h>

#include "CLRHost.h"

#import "SampleAssembly.tlb" no_namespace named_guids

BOOL FileExists(LPCWSTR szPath) 
{ 
   DWORD dwAttrib = GetFileAttributesW(szPath);
   return (dwAttrib != INVALID_FILE_ATTRIBUTES); 
} 

LONG_PTR
_getMorePower(int x)
{
	int domainId = CLRHostCreateAppDomain();

	CLRHostLoadAssembly(domainId, "SampleAssembly");

	LONG_PTR result = CLRHostRun(domainId, "SampleAssembly.Program.MorePower", (LONG_PTR)x);

	CLRHostDestroyAppDomain(domainId);

	return (int)result;
}

int wmain(int argc, WCHAR* argv[])
{
   if (argc != 2)
   {
      wprintf_s(L"Usage: TestApp <path to exe>\n");
      return 0;
   }

   if (FileExists(argv[1]))
   {
        CLRHostInitialize();

		CLRHostExecute("SampleAssembly.exe", "42");

		LONG_PTR result;
		result = _getMorePower(5);
		wprintf_s(L"SampleAssembly.MorePower: %Id\n", result);
		result = _getMorePower(7);
		wprintf_s(L"SampleAssembly.MorePower: %Id\n", result);

		IHalfPower *pIHalfPower = NULL;

		CoInitialize(NULL);

		HRESULT hr = CoCreateInstance(	CLSID_HalfPowerManager,
										NULL, CLSCTX_INPROC_SERVER,
										__uuidof(IHalfPower), 
										(PVOID*)&pIHalfPower);
		if (SUCCEEDED(hr))
		{
			result = pIHalfPower->HalfPower(1024);
			wprintf_s(L"SampleAssembly.HalfPower: %Id\n", result);

			pIHalfPower->Release();
		}
		else
		{
			wprintf_s(L"CoCreateInstance failed: 0x%08x\n", hr);
		}

		CoUninitialize();
        return 0;
   }
   else
   {
      wprintf_s(L"%s is not found\n", argv[1]);
      return 0;
   }
}

