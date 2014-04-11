#ifndef _LOCAL_DXGI_H_
#define _LOCAL_DXGI_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <dxgi.h>

//
HRESULT DXGICreateSurface(IDXGISurface **ppDxgiSurface);

#endif // _LOCAL_DXGI_H_
