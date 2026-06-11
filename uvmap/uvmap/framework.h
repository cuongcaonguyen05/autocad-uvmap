#pragma once

// Dùng MFC để có CView / CMDIFrameWnd... (ObjectARX rxmfcapi.h cần MFC).
// LƯU Ý: khi dùng MFC, KHÔNG include <windows.h> trực tiếp và KHÔNG định nghĩa
// WIN32_LEAN_AND_MEAN — chính afxwin.h sẽ kéo windows.h vào đúng thứ tự.
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include <afxwin.h>     // MFC lõi: CWinApp, CWnd, CView, CFrameWnd, CMDIFrameWnd
#include <afxext.h>     // MFC mở rộng: CFormView, CScrollView...
