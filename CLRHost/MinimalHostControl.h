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

#pragma once

class MinimalHostControl : public IHostControl
{
public:
   MinimalHostControl()
   {
      m_refCount = 0;
      m_defaultDomainManager = NULL;
   }
   virtual ~MinimalHostControl()
   {
      if (m_defaultDomainManager != nullptr)
      {
         m_defaultDomainManager->Release();
      }
   }
   ICLRHostAppDomainManager* GetDomainManagerForDefaultDomain()
   {
      if (m_defaultDomainManager)
      {
         m_defaultDomainManager->AddRef();
      }
      return m_defaultDomainManager;
   }
   HRESULT STDMETHODCALLTYPE GetHostManager(REFIID riid,void **ppv)
   {
      *ppv = NULL;
      return E_NOINTERFACE; 
   }

   HRESULT STDMETHODCALLTYPE SetAppDomainManager(DWORD dwAppDomainID, IUnknown *pUnkAppDomainManager)
   {
      HRESULT hr = S_OK;
      hr = pUnkAppDomainManager->QueryInterface(__uuidof(ICLRHostAppDomainManager), (PVOID*) &m_defaultDomainManager);
      return hr;
   }
   HRESULT STDMETHODCALLTYPE QueryInterface( const IID &iid, void **ppv)
   {
      if (!ppv) return E_POINTER;
      *ppv= this;
      AddRef();
      return S_OK;
   }
   ULONG STDMETHODCALLTYPE AddRef()
   {
         return InterlockedIncrement(&m_refCount);
   }
   ULONG STDMETHODCALLTYPE Release()
   {
      if(InterlockedDecrement(&m_refCount) == 0)
      {
         delete this;
         return 0;
      }
      return m_refCount;
   }
private:
   long m_refCount; 
   ICLRHostAppDomainManager* m_defaultDomainManager;
};
