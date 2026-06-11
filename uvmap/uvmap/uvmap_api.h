// uvmap_api.h : Bề mặt công khai (C API) mà DLL uvmap export ra ngoài.
// Toàn bộ đi qua một FeatureManager dùng chung bên trong DLL.
#pragma once

#ifdef UVMAP_EXPORTS
#define UVMAP_API __declspec(dllexport)
#else
#define UVMAP_API __declspec(dllimport)
#endif

extern "C" {

    // Chuỗi phiên bản DLL.
    UVMAP_API const char* uvmap_version();

    // Khởi tạo manager và đăng ký toàn bộ tính năng (gọi 1 lần trước khi dùng).
    UVMAP_API void uvmap_initialize();

    // Chạy một tính năng theo tên. Trả về mã lỗi của tính năng,
    // hoặc -1 nếu chưa initialize / không tìm thấy tên.
    UVMAP_API int uvmap_run(const char* featureName);

    // Liệt kê tính năng.
    UVMAP_API int         uvmap_feature_count();
    UVMAP_API const char* uvmap_feature_name(int index);

    // Giải phóng manager.
    UVMAP_API void uvmap_shutdown();

}
