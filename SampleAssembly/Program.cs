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

using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace SampleAssembly
{
    public class Program
    {
        public static void Main(string[] args)
        {
            Trace.WriteLine("SampleAssembly: Enter - Program::Main");
            try
            {
                if (args.Length != 0)
                {
                    int i = int.Parse(args[0]);
                    Console.WriteLine(string.Format("SampleAssembly.Main: val={0} i/2={1}", i, i / 2));
                }

                Trace.WriteLine("SampleAssembly: Throwing Exception");
                throw new NotImplementedException();
            }
            catch (System.Exception)
            {
                Trace.WriteLine("SampleAssembly: Catching Exception");
            }
            Trace.WriteLine("SampleAssembly: Leave - Program::Main");
        }

        public static IntPtr MorePower(IntPtr arg)
        {
            Trace.WriteLine("SampleAssembly: Entering Method MorePower");
            IntPtr val = new IntPtr(arg.ToInt32() * arg.ToInt32());
            Trace.WriteLine("SampleAssembly: Leaving Method MorePower");
            return val;
        }

        [GuidAttribute("e654f98f-7596-4219-9fcf-c96b88b5ec1a"), ComVisible(true)]
        public interface IHalfPower
        {
            IntPtr HalfPower(IntPtr parameter);
        }

        [GuidAttribute("24ac89b9-d9dd-4665-8854-d853e4aad9c8"), ComVisible(true)]
        public sealed class HalfPowerManager : IHalfPower
        {
            public IntPtr HalfPower(IntPtr x)
            {
                return new IntPtr(x.ToInt32() / 2);
            }
        }
    }
}
