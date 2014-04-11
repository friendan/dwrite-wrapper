DWrite Wrapper
==============

DWrite.dll TextRenderingParams and TextAntialiasMode wrapper.

Build
-----
32bit:

    > nmake -f Makefile clean install

64bit:

    > nmake -f Makefile.x64 clean install

Install
-------
*Run as administrator*

Install:

    > ie-install.bat

Uninstall:

    > ie-uninstall.bat

Configuration
-------------

See DWrite.reg file.

* HKCU\Software\DWrite Wrapper

  Name              | Type  | Description
  ------------------|-------|------------
  FontSizeThreshold | DWORD | You can apply different settings to different size fonts(Default:16).

* HKCU\Software\DWrite Wrapper\SmallFont

  Small font settings below the threshold.

  _Value does not exist or value greater than 0x7FFFFFFF is used system default value._

  Name             | Type  | Description
  -----------------|-------|------------
  Gamma            | DWORD | Set the multiplied by 1,000(ex, 2.2 -> 2200).
  EnhancedContrast | DWORD | Set the percentage.
  ClearTypeLevel   | DWORD | Set the percentage.
  PixelGeometry    | DWORD | 0:FLAT, 1:RGB, 2:BGR
  RenderingMode    | DWORD | 0:Default, 1:Aliased, 2:GDI Classic, 3:GDI Natural, 4:Natural, 5:Natural Symmetric, 6:Outline
  AntialiasMode    | DWORD | 0:Default, 1:ClearType, 2:GreyScale, 3:Aliased

  _Valid Combination_

  AntialiasMode | RenderingMode
  --------------|--------------
  0             | 0,1,2,3,4,5,6
  1             | 0,2,3,4,5
  2             | 0,2,3,4,5,6
  3             | 0,1

* HKCU\Software\DWrite Wrapper\LargeFont

  Large font settings below the threshold.

  _See SmallFont setting._
