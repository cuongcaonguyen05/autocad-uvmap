// uvmap_api.cpp : Hiện thực C API + nơi đăng ký tính năng cho DLL uvmap.
#include "pch.h"
#include "uvmap_api.h"

#include "FeatureManager.h"

// --- Các tính năng của bạn: thêm #include header tại đây ---
#include "SampleFeature.h"
#include "UVMap/AutoCadUVMap.h"
// #include "MyFeatureB.h"

#include <memory>
#include <string>

namespace {

    // Manager dùng chung của DLL. Tạo trong uvmap_initialize().
    std::unique_ptr<FeatureManager> g_manager;

    // ============================================================
    //  ĐIỂM "MAIN": nơi bạn cắm các tính năng tự viết vào hệ thống.
    //  Mỗi lần thêm tính năng mới chỉ cần:
    //    1. #include header của nó ở trên.
    //    2. Thêm một dòng mgr.Register(...) tại đây.
    //  Phần còn lại (chạy, liệt kê, gọi qua tên) đã có sẵn.
    // ============================================================
    void RegisterFeatures(FeatureManager& mgr)
    {
        mgr.Register(std::make_unique<SampleFeature>(1.0f, 0.0f, 0.0f));
        mgr.Register(std::make_unique<AutoCadUVMap>());
        // mgr.Register(std::make_unique<MyFeatureB>(...));
    }

    // Giữ lại tên tính năng trả ra ngoài để con trỏ char* còn hợp lệ.
    std::string g_nameCache;

} // namespace

extern "C" {

    const char* uvmap_version()
    {
        return "uvmap 1.0.0";
    }

    void uvmap_initialize()
    {
        g_manager = std::make_unique<FeatureManager>();
        RegisterFeatures(*g_manager);
    }

    int uvmap_run(const char* featureName)
    {
        if (!g_manager || featureName == nullptr)
            return -1;
        return g_manager->Run(featureName);
    }

    int uvmap_feature_count()
    {
        return g_manager ? static_cast<int>(g_manager->Count()) : 0;
    }

    const char* uvmap_feature_name(int index)
    {
        if (!g_manager)
            return nullptr;

        const auto names = g_manager->Names();
        if (index < 0 || index >= static_cast<int>(names.size()))
            return nullptr;

        g_nameCache = names[index];
        return g_nameCache.c_str();
    }

    void uvmap_shutdown()
    {
        g_manager.reset();
    }

}
