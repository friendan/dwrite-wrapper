#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "d2d1.h"

DrawGlyphRunProc GetDrawGlyphRun(ID2D1RenderTarget *renderTarget)
{
  return renderTarget->lpVtbl->DrawGlyphRun;
}
