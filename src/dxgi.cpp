#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>

#include "common.hpp"
#include "dxgi.h"

HRESULT DXGICreateSurface(IDXGISurface **ppDxgiSurface)
{
  HRESULT hr = S_OK;

  ID3D11Device *pDevice = NULL;
  ID3D11Texture2D *pTexture2D = NULL;

  D3D_FEATURE_LEVEL featureLevel;
  ID3D11DeviceContext *pImmediateContext = NULL;

  // Create device
  hr = D3D11CreateDevice(
    NULL,
    D3D_DRIVER_TYPE_HARDWARE,
    NULL,
    D3D11_CREATE_DEVICE_BGRA_SUPPORT,
    NULL,
    0,
    D3D11_SDK_VERSION,
    &pDevice,
    &featureLevel,
    &pImmediateContext);

  if (FAILED(hr)) {
    hr = D3D11CreateDevice(
      NULL,
      D3D_DRIVER_TYPE_WARP,
      NULL,
      D3D11_CREATE_DEVICE_BGRA_SUPPORT,
      NULL,
      0,
      D3D11_SDK_VERSION,
      &pDevice,
      &featureLevel,
      &pImmediateContext);
  }

  if (SUCCEEDED(hr)) {
    // Allocate a offscreen D3D surface for D2D to render our 2D content into
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = 1;
    desc.Height = 1;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    hr = pDevice->CreateTexture2D(&desc, NULL, &pTexture2D);
  }

  if (SUCCEEDED(hr)) {
    hr = pTexture2D->QueryInterface(ppDxgiSurface);
  }

  SafeRelease(pDevice);
  SafeRelease(pTexture2D);
  SafeRelease(pImmediateContext);

  return hr;
}
