#ifndef _LOCAL_D2D1_H_
#define _LOCAL_D2D1_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <d2d1.h>

#ifdef __cplusplus
extern "C" {
#endif

//
typedef void (WINAPI *DrawGlyphRunProc) (
  ID2D1RenderTarget *This,
  D2D1_POINT_2F baselineOrigin,
  const DWRITE_GLYPH_RUN *glyphRun,
  ID2D1Brush *foregroundBrush,
  DWRITE_MEASURING_MODE measuringMode);

//
DrawGlyphRunProc GetDrawGlyphRun(ID2D1RenderTarget *renderTarget);

#ifdef __cplusplus
}
#endif

#endif // _LOCAL_D2D1_H_
