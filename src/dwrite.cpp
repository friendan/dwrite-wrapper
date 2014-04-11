#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>

#include <easyhook.h>

#include "common.hpp"
#include "d2d1.h"
#include "dxgi.h"
#include "dwrite.h"


// Function pointer
static DWriteCreateFactoryProc RealDWriteCreateFactory = NULL;
static DrawGlyphRunProc RealDrawGlyphRun = NULL;

// Global variables
static TRACED_HOOK_HANDLE hDrawGlyphRun = NULL;

static INT Threshold = 16;
static DWW_PARAM Small = { D2D1_TEXT_ANTIALIAS_MODE_DEFAULT, NULL,
                           2.2f, 0.5f, 1.0f, DWRITE_PIXEL_GEOMETRY_RGB,
                           DWRITE_RENDERING_MODE_DEFAULT };
static DWW_PARAM Large = { D2D1_TEXT_ANTIALIAS_MODE_DEFAULT, NULL,
                           2.2f, 0.5f, 1.0f, DWRITE_PIXEL_GEOMETRY_RGB,
                           DWRITE_RENDERING_MODE_DEFAULT };


// Function prototype
static HRESULT Hook();
static void UnHook();
static void Update();
static void RegToIParams(LPCTSTR lpSubKey, LPDWW_IPARAM lpIParams);
static void IParamsToParams(LPDWW_IPARAM lpIParams, LPDWW_PARAM lpParams);
static BOOL LoadDWriteCreateFactory();


// Macro
#define Throw(msg) OutputDebugString(msg); goto Catch

#ifdef DEBUG
#define DBG WriteDebugInfo
static void WriteDebugInfo(LPCTSTR format, ...)
{
  TCHAR buffer[4096];
  va_list ap;
  va_start(ap, format);
  _vstprintf_s(buffer, 4096, format, ap);
  va_end(ap);
  OutputDebugString(buffer);
}
#else
#define DBG __noop
#endif


//////////////////////////////////////////////////////////////////////////////

static void WINAPI FakeDrawGlyphRun(
  ID2D1RenderTarget *This,
  D2D1_POINT_2F baselineOrigin,
  const DWRITE_GLYPH_RUN *glyphRun,
  ID2D1Brush *foregroundBrush,
  DWRITE_MEASURING_MODE measuringMode = DWRITE_MEASURING_MODE_NATURAL)
{
  IDWriteRenderingParams *pOriginalParams = NULL;

  DBG(_T("FakeDrawGlyphRun: fontEmSize=%f"), glyphRun->fontEmSize);

  // threshold < 0 := small only
  //   modify
  // threshold > 0 := small and large
  //   size > threshold := large
  //     small aa == -1 ? preserve rd + modify : modify
  //   size < threshold := small
  //     small aa == -1 ? no change : modify

  if (glyphRun->fontEmSize < Threshold && Small.AntialiasMode < 0) {
  } else {
    LPDWW_PARAM font;
    font = (Threshold < 0 || glyphRun->fontEmSize < Threshold) ? &Small : &Large;

    This->GetTextRenderingParams(&pOriginalParams);
    This->SetTextRenderingParams(font->RenderingParams);

    if (font->AntialiasMode < 0) {
    } else {
      D2D1_TEXT_ANTIALIAS_MODE a = This->GetTextAntialiasMode();
      D2D1_TEXT_ANTIALIAS_MODE b = font->AntialiasMode;
      if (a == D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE &&
          b == D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE) {
        b = D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
      }
      This->SetTextAntialiasMode(b);
    }
  }

  RealDrawGlyphRun(This, baselineOrigin, glyphRun, foregroundBrush, measuringMode);

  if (pOriginalParams) {
    This->SetTextRenderingParams(pOriginalParams);
  }

  SafeRelease(pOriginalParams);
}

//////////////////////////////////////////////////////////////////////////////

static HRESULT Hook()
{
  HRESULT hr = S_OK;

  ID2D1Factory *pFactory = NULL;
  IDXGISurface *pSurface = NULL;

  const D2D1_PIXEL_FORMAT format =
    D2D1::PixelFormat(
      DXGI_FORMAT_B8G8R8A8_UNORM,
      D2D1_ALPHA_MODE_PREMULTIPLIED);
  const D2D1_RENDER_TARGET_PROPERTIES properties =
    D2D1::RenderTargetProperties(
      D2D1_RENDER_TARGET_TYPE_DEFAULT,
      format,
      0.0f,
      0.0f,
      D2D1_RENDER_TARGET_USAGE_NONE);

  ID2D1RenderTarget *pTarget = NULL;

  ULONG ACLEntries[1] = { 0 };

  DBG(_T("Hook"));
  if (hDrawGlyphRun != NULL) {
    return hr;
  }

  // COM Initialize
  hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
  if (FAILED(hr) && hr == RPC_E_CHANGED_MODE) {
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
  }
  if (FAILED(hr)) {
    Throw(_T("CoInitialize failed"));
  }

  // Create D2D1Factory
  hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&pFactory));
  if (FAILED(hr)) {
    Throw(_T("D2D1CreateFactory failed"));
  }

  // Create DxgiSurfaceRenderTarget
  hr = DXGICreateSurface(&pSurface);
  if (FAILED(hr)) {
    Throw(_T("DXGICreateSurface failed"));
  }
  hr = pFactory->CreateDxgiSurfaceRenderTarget(pSurface, properties, &pTarget);
  if (FAILED(hr)) {
    Throw(_T("D2D1CreateFactory.CreateDxgiSurfaceRenderTarget failed"));
  }

  // Install DrawGlyphRun function hook
  RealDrawGlyphRun = GetDrawGlyphRun(pTarget);
  hDrawGlyphRun = new HOOK_TRACE_INFO();
  hr = HRESULT_FROM_NT(LhInstallHook(RealDrawGlyphRun, FakeDrawGlyphRun, NULL, hDrawGlyphRun));
  if (FAILED(hr)) {
    SafeDelete(hDrawGlyphRun);  // delete handle.
    Throw(_T("LhInstallHook failed"));
  }

  // all threads hooking.
  hr = HRESULT_FROM_NT(LhSetExclusiveACL(ACLEntries, 1, hDrawGlyphRun));
  if (FAILED(hr)) {
    Throw(_T("LhSetExclusiveACL failed"));
  }

  goto Finally;

Catch:
  if (hDrawGlyphRun) {
    LhUninstallAllHooks();
    LhUninstallHook(hDrawGlyphRun);
    LhWaitForPendingRemovals();
    SafeDelete(hDrawGlyphRun);
  }

Finally:
  SafeRelease(pFactory);
  SafeRelease(pSurface);
  SafeRelease(pTarget);

  return hr;
}

static void Update()
{
  DBG(_T("Update"));

  HKEY hKey;
  LONG retval = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\DWrite Wrapper"), 0, KEY_READ, &hKey);
  if (retval == ERROR_SUCCESS) {
    // Read Threshold Value
    DWORD dwType = REG_DWORD;
    DWORD cbData = sizeof(DWORD);
    RegQueryValueEx(hKey, _T("FontSizeThreshold"), NULL, &dwType, (LPBYTE) &Threshold, &cbData);
    RegCloseKey(hKey);
  }

  DBG(_T(" Threshold: %d"), Threshold);

  // Read Font Value
  DWW_IPARAM ISmall, ILarge;
  RegToIParams(_T("Software\\DWrite Wrapper\\SmallFont"), &ISmall);
  RegToIParams(_T("Software\\DWrite Wrapper\\LargeFont"), &ILarge);

  IParamsToParams(&ISmall, &Small);
  IParamsToParams(&ILarge, &Large);
}

static void RegToIParams(LPCTSTR lpSubKey, LPDWW_IPARAM lpIParams)
{
  lpIParams->Gamma = -1;
  lpIParams->EnhancedContrast = -1;
  lpIParams->ClearTypeLevel = -1;
  lpIParams->PixelGeometry = -1;
  lpIParams->RenderingMode = 0;
  lpIParams->AntialiasMode = 0;

  // Read Registry Value
  HKEY hKey;
  LONG retval = RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, KEY_READ, &hKey);
  if (retval != ERROR_SUCCESS) {
    return;
  }

  DWORD dwType = REG_DWORD;
  DWORD cbData = sizeof(DWORD);

  RegQueryValueEx(hKey, _T("Gamma"), NULL, &dwType, (LPBYTE) &lpIParams->Gamma, &cbData);
  RegQueryValueEx(hKey, _T("EnhancedContrast"), NULL, &dwType, (LPBYTE) &lpIParams->EnhancedContrast, &cbData);
  RegQueryValueEx(hKey, _T("ClearTypeLevel"), NULL, &dwType, (LPBYTE) &lpIParams->ClearTypeLevel, &cbData);
  RegQueryValueEx(hKey, _T("PixelGeometry"), NULL, &dwType, (LPBYTE) &lpIParams->PixelGeometry, &cbData);
  RegQueryValueEx(hKey, _T("RenderingMode"), NULL, &dwType, (LPBYTE) &lpIParams->RenderingMode, &cbData);
  RegQueryValueEx(hKey, _T("AntialiasMode"), NULL, &dwType, (LPBYTE) &lpIParams->AntialiasMode, &cbData);

  RegCloseKey(hKey);
}

static void IParamsToParams(LPDWW_IPARAM lpIParams, LPDWW_PARAM lpParams)
{
  IDWriteFactory *pFactory = NULL;
  IDWriteRenderingParams *pRenderingParams = NULL;

  // Create Default RenderingParams
  RealDWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
    __uuidof(IDWriteFactory),
    reinterpret_cast<IUnknown**>(&pFactory));
  pFactory->CreateRenderingParams(&pRenderingParams);

  // Copy Params
  lpParams->Gamma = (lpIParams->Gamma < 0) ?
    pRenderingParams->GetGamma() :
    (FLOAT) lpIParams->Gamma / 1000;

  lpParams->EnhancedContrast = (lpIParams->EnhancedContrast < 0) ?
    pRenderingParams->GetEnhancedContrast() :
    (FLOAT) lpIParams->EnhancedContrast / 100;

  lpParams->ClearTypeLevel = (lpIParams->ClearTypeLevel < 0) ?
    pRenderingParams->GetClearTypeLevel() :
    (FLOAT) lpIParams->ClearTypeLevel / 100;

  lpParams->PixelGeometry = (lpParams->PixelGeometry < 0) ?
    pRenderingParams->GetPixelGeometry() :
    (DWRITE_PIXEL_GEOMETRY) lpParams->PixelGeometry;

  lpParams->RenderingMode =
    (DWRITE_RENDERING_MODE) lpIParams->RenderingMode;

  lpParams->AntialiasMode =
    (D2D1_TEXT_ANTIALIAS_MODE) lpIParams->AntialiasMode;

  if (lpParams->RenderingParams) {
    lpParams->RenderingParams->Release();
  }

  pFactory->CreateCustomRenderingParams(
    lpParams->Gamma,
    lpParams->EnhancedContrast,
    lpParams->ClearTypeLevel,
    lpParams->PixelGeometry,
    lpParams->RenderingMode,
    &lpParams->RenderingParams);

  DBG(_T(" Params: %f %f %f %x %x %x"),
      lpParams->Gamma,
      lpParams->EnhancedContrast,
      lpParams->ClearTypeLevel,
      lpParams->PixelGeometry,
      lpParams->RenderingMode,
      lpParams->AntialiasMode);

  SafeRelease(pFactory);
  SafeRelease(pRenderingParams);
}

static void UnHook()
{
  if (hDrawGlyphRun == NULL) {
    return;
  }

  DBG(_T("UnHook"));

  // Uninstall DrawGlyphRun function hook
  LhUninstallAllHooks();
  LhUninstallHook(hDrawGlyphRun);
  LhWaitForPendingRemovals();
  SafeDelete(hDrawGlyphRun);

  // COM Uninitialize
  CoUninitialize();
}

static BOOL LoadDWriteCreateFactory()
{
  DBG(_T("LoadDWriteCreateFactory"));

  // Create DWrite.dll full-path
  TCHAR fullPath[MAX_PATH];
  TCHAR systemPath[MAX_PATH];

  if (GetSystemDirectory(systemPath, MAX_PATH) == 0) {
    Throw(_T("GetSystemDirectory failed"));
  }
  if (_stprintf_s(fullPath, MAX_PATH, _T("%s\\DWrite.dll"), systemPath) == -1) {
    Throw(_T("_stprintf_s(\"%SYSTEM%\\DWrite.dll\") failed"));
  }

  // Get DWriteCreateFactory function-pointer
  RealDWriteCreateFactory = (DWriteCreateFactoryProc)
    GetProcAddress(LoadLibrary(fullPath), "DWriteCreateFactory");
  if (RealDWriteCreateFactory == NULL) {
    Throw(_T("LoadLibrary(\"DWriteCreateFactory\") failed"));
  }

  return TRUE;

Catch:
  return FALSE;
}

EXTERN_C HRESULT DWRITE_EXPORT DWriteCreateFactory(
  DWRITE_FACTORY_TYPE factoryType,
  REFIID iid,
  IUnknown **factory)
{
  HRESULT hr = Hook();
  if (SUCCEEDED(hr)) {
    DBG(_T(" succeeded"));
    hr = RealDWriteCreateFactory(factoryType, iid, factory);
  }
  return hr;
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    DBG(_T("DLL_PROCESS_ATTACH"));
    DisableThreadLibraryCalls(hinstDLL);
    if (LoadDWriteCreateFactory()) {
      Update();
    } else {
      return FALSE;
    }
  break;
  case DLL_PROCESS_DETACH:
    DBG(_T("DLL_PROCESS_DETACH"));
    UnHook();
  break;
  }
  return TRUE;
}
