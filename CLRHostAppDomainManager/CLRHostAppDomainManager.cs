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
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Policy;
using System.Threading;

namespace CLRHostAppDomainManager
{
    [GuidAttribute("5982cd23-b603-4987-8e0b-b04da55e3583"), ComVisible(true)]
    public interface ICLRHostAppDomainManager
    {
        void InitializeNewDomain(AppDomainSetup appDomainInfo);
        int CreateAppDomain();
        bool DestroyAppDomain(int domainId);
        bool LoadAssembly(int domainId, string assemblyFilename);
        void Execute(string assemblyFilename);
        IntPtr Run(int domainId, string symbolName, IntPtr parameter);
    }

    [GuidAttribute("0a437314-63e5-43f9-a912-56c1a8bd65ae"), ComVisible(true)]
    public sealed class CLRHostAppDomainManager : AppDomainManager, ICLRHostAppDomainManager
    {
        private static int _NextDomainId = 0;
        private static Dictionary<int, AppDomain> _Domains = new Dictionary<int, AppDomain>();

        public override void InitializeNewDomain(AppDomainSetup appDomainInfo)
        {
            Trace.WriteLine("*** InitializeNewDomain");
            this.InitializationFlags = AppDomainManagerInitializationOptions.RegisterWithHost;
        }

        public override AppDomain CreateDomain(string friendlyName, Evidence securityInfo, AppDomainSetup appDomainInfo)
        {
            var appDomain = base.CreateDomain(friendlyName, securityInfo, appDomainInfo);
            Trace.WriteLine("*** Created AppDomain {0}", friendlyName);
            return appDomain;
        }

        public int CreateAppDomain()
        {
            int domainId = Interlocked.Increment(ref _NextDomainId);
            _Domains[domainId] = AppDomain.CreateDomain("Domain" + domainId.ToString());
            return domainId;
        }

        public bool DestroyAppDomain(int domainId)
        {
            if (!_Domains.ContainsKey(domainId)) return false;

            AppDomain.Unload(_Domains[domainId]);
            _Domains.Remove(domainId);
            return true;
        }

        public bool LoadAssembly(int domainId, string assemblyFilename)
        {
            Assembly asm = _Domains[domainId].Load(assemblyFilename);
            return asm != null;
        }

        public IntPtr Run(int domainId, string symbolName, IntPtr parameter)
        {
            try
            {
                string[] syms = symbolName.Split('.');
                Assembly asm = _Domains[domainId].GetAssemblies().
                                SingleOrDefault(a => a.GetName().Name == syms[0]);
                string typeName = string.Join(".", syms.Take(syms.Length - 1).ToArray());
                Type t = asm.GetType(typeName);
                var mi = t.GetMethod(syms[syms.Length - 1]);
                return (IntPtr)mi.Invoke(null, new object[] { parameter });
            }
            catch (Exception){}
            return IntPtr.Zero;
        }

        public void Execute(string assemblyFilename)
        {
            if (!System.IO.File.Exists(assemblyFilename))
            {
                Trace.WriteLine("Application cannot be found");
                return;
            }

            int domainId = -1;
            try
            {
                domainId = CreateAppDomain();
                int exitCode = _Domains[domainId].ExecuteAssembly(assemblyFilename);
                Trace.WriteLine(string.Format("ExitCode={0}", exitCode));
            }
            catch (System.Exception)
            {
                string message = string.Format("Unhandled Exception in {0}",
                                                System.IO.Path.GetFileNameWithoutExtension(
                                                    assemblyFilename));
                Trace.TraceError(message);
            }
            finally
            {
                if (domainId != -1)
                {
                    DestroyAppDomain(domainId);
                    Trace.WriteLine(string.Format("*** Unloaded AppDomain {0}", domainId));
                }
            }
        }
    }
}

