#ifndef _LOCAL_COMMON_HPP_
#define _LOCAL_COMMON_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

//
template <class T> void SafeDelete(T *&p)
{
  if (p) {
    delete p;
    p = NULL;
  }
}

template <class T> void SafeRelease(T *&p)
{
  if (p) {
    p->Release();
    p = NULL;
  }
}

#endif // _LOCAL_COMMON_HPP_
