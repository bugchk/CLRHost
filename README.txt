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

--- Compiling ---

SampleAppDomainManager is a COM library
COM is a platform independent binary format.
The DLL is usable from many types of languages.
The DLL should be used indirectly through a COM type library.

The AppLauncher (C++) needs to have a AppDomainManagers.tlb (type library) generated.

1. Start by compiling AppDomainManagers.dll
2. Open a Visual Studio Command prompt
3. Change directory to the debug directory of the project
4. Generate the type library by running the following command "TlbExp AppDomainManagers.dll"

Now you will find AddDomainManagers.tlb

5. Build rest of the solution

--- Running ---
From a command prompt

C:\temp> SampleApp1.exe
C:\temp> SampleAppLauncher SampleApp1.exe

