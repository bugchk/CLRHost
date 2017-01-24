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

#include "..\CLRHost\CLRHost.h"

BOOL FileExists(LPCWSTR szPath) 
{ 
   DWORD dwAttrib = GetFileAttributesW(szPath);
   return (dwAttrib != INVALID_FILE_ATTRIBUTES); 
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

        int domainId = CLRHostCreateAppDomain();

        CLRHostLoadAssembly(domainId, "SampleAssembly");

        LONG_PTR result = CLRHostRun(domainId, "SampleAssembly.Program.Start", NULL);

        wprintf_s(L"result %lld\n", result);

        CLRHostDestroyAppDomain(domainId);

		domainId = CLRHostCreateAppDomain();

        CLRHostLoadAssembly(domainId, "SampleAssembly");

        result = CLRHostRun(domainId, "SampleAssembly.Program.Start", NULL);

        wprintf_s(L"result %lld\n", result);

        CLRHostDestroyAppDomain(domainId);


        return 0;
   }
   else
   {
      wprintf_s(L"%s is not found\n", argv[1]);
      return 0;
   }
}

