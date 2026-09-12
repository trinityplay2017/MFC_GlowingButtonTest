# MFC Glowing Button Example

A complete, self-contained Visual C++ / MFC example project that demonstrates a custom owner-drawn button with:

- Smooth color lerp animation on hover / press / disable
- Optional animated rainbow border
- Rainbow text effect
- Rounded corners (window region)
- Dark-theme friendly defaults

## Requirements

- Visual Studio 2019 or 2022 (any edition)
- Desktop development with C++ workload
- MFC for latest v143 build tools (or v142)

## Build & Run

1. Open `MFC_GlowingButtonTest.sln` in Visual Studio.
2. Select **Debug | x64** (or x86).
3. Press **F5** (or Build → Build Solution).

The sample dialog shows several `CGlowingButton` instances with different configurations.

## Project Layout

```
MFC_GlowingButtonTest/
├── MFC_GlowingButtonTest.sln
├── MFC_GlowingButtonTest.vcxproj
├── MFC_GlowingButtonTest.vcxproj.filters
├── src/
│   ├── pch.h / pch.cpp
│   ├── framework.h
│   ├── targetver.h
│   ├── Resource.h
│   ├── MFC_GlowingButtonTest.h / .cpp          (application)
│   ├── MFC_GlowingButtonTestDlg.h / .cpp       (main dialog)
│   ├── GlowingButton.h / .cpp                 (the control)
│   └── GlowingButtonInternal.h                (drawing helpers)
├── res/
│   ├── MFC_GlowingButtonTest.rc
│   └── MFC_GlowingButtonTest.rc2
└── .gitignore
```

## Using the control in your own project

1. Copy `GlowingButton.h`, `GlowingButton.cpp` and `GlowingButtonInternal.h` into your project.
2. Add the three files to your VS project.
3. In a dialog or window:

```cpp
#include "GlowingButton.h"

// member
CGlowingButton m_btn;

// OnInitDialog / Create
m_btn.SubclassDlgItem(IDC_MY_BUTTON, this);
// or
m_btn.Create(_T("Click me"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
             CRect(10,10,200,40), this, IDC_MY_BUTTON);

m_btn.SetBackColor(RGB(40,40,40));
m_btn.SetHoverBackColor(RGB(60,60,60));
m_btn.SetPressedColor(RGB(123,102,240));
m_btn.SetTextColor(RGB(255,255,255));
m_btn.SetBorderColor(RGB(20,20,20));
m_btn.SetRoundRadius(8);
m_btn.EnableRainbowBorder(true);
```

## License

Public domain / do whatever you want.
