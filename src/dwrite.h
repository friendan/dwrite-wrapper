#ifndef _LOCAL_DWRITE_H_
#define _LOCAL_DWRITE_H_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef DWRITE_EXPORTS
#define DWRITE_EXPORT WINAPI
#endif

#include <dwrite.h>

//
typedef struct _DWW_PARAM {
  D2D1_TEXT_ANTIALIAS_MODE AntialiasMode;
  IDWriteRenderingParams *RenderingParams;
  FLOAT Gamma;
  FLOAT EnhancedContrast;
  FLOAT ClearTypeLevel;
  DWRITE_PIXEL_GEOMETRY PixelGeometry;
  DWRITE_RENDERING_MODE RenderingMode;
} DWW_PARAM, *LPDWW_PARAM;

typedef struct _DWW_IPARAM {
  LONG AntialiasMode;
  LONG Gamma;
  LONG EnhancedContrast;
  LONG ClearTypeLevel;
  LONG PixelGeometry;
  LONG RenderingMode;
} DWW_IPARAM, *LPDWW_IPARAM;

//
typedef HRESULT (WINAPI *DWriteCreateFactoryProc) (
  DWRITE_FACTORY_TYPE factoryType,
  REFIID iid,
  IUnknown **factory);

#endif // _LOCAL_DWRITE_H_
