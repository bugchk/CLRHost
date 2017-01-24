// ----------------------------------------------------------------------------------------------
// Copyright (c) Mattias Högström.
// ----------------------------------------------------------------------------------------------
// This source code is subject to terms and conditions of the Microsoft Public License. A 
// copy of the license can be found in the License.html file at the root of this distribution. 
// If you cannot locate the Microsoft Public License, please send an email to 
// dlr@microsoft.com. By using this source code in any fashion, you are agreeing to be bound 
// by the terms of the Microsoft Public License.
// ----------------------------------------------------------------------------------------------
// You must not remove this notice, or any other, from this software.
// ----------------------------------------------------------------------------------------------

using System;
using System.Diagnostics;

namespace SampleAssembly
{
    public class Program
    {
        public static void Main(string[] args)
        {
            Trace.WriteLine("SampleApp1: Enter - Program::Main");
            try
            {
                Trace.WriteLine("SampleApp1: Throwing Exception");
                throw new NotImplementedException();
            }
            catch (System.Exception)
            {
                Trace.WriteLine("SampleApp1: Catching Exception");
            }
            Trace.WriteLine("SampleApp1: Leave - Program::Main");
        }

        public static IntPtr Start(IntPtr arg)
        {
            Trace.WriteLine("Entering Method Start");
            const int a = 21;
            const int b = 21;
            Trace.WriteLine("Leaving Method Start");
            return new IntPtr(a + b);
        }
    }
}
