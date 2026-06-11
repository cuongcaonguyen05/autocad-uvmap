// dllmain.cpp : Đối tượng ứng dụng MFC cho DLL.
//
// Khi project bật "Use of MFC = Dynamic", MFC TỰ cung cấp hàm DllMain.
// Vì vậy KHÔNG tự định nghĩa DllMain ở đây (sẽ bị lỗi "DllMain already defined").
// Thay vào đó chỉ cần một đối tượng CWinApp để MFC khởi tạo module state.
#include "pch.h"

class CUvmapApp : public CWinApp
{
public:
    virtual BOOL InitInstance() override
    {
        return CWinApp::InitInstance();
    }
};

CUvmapApp theApp;
