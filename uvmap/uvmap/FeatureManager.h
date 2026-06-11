// FeatureManager.h : Class quản lý trung tâm — giữ và điều phối mọi tính năng.
#pragma once

#include "IFeature.h"

#include <memory>
#include <string>
#include <vector>

class FeatureManager
{
public:
    // Đăng ký một tính năng. Manager sẽ sở hữu (own) con trỏ này.
    // Trả về false nếu trùng tên hoặc feature null.
    bool Register(std::unique_ptr<IFeature> feature);

    // Chạy một tính năng theo tên. Trả về mã lỗi của tính năng,
    // hoặc -1 nếu không tìm thấy tên.
    int Run(const std::string& name);

    // Có tính năng với tên này hay không.
    bool Has(const std::string& name) const;

    // Số tính năng đã đăng ký.
    size_t Count() const { return m_features.size(); }

    // Danh sách tên các tính năng (để liệt kê ra ngoài).
    std::vector<std::string> Names() const;

private:
    IFeature* Find(const std::string& name) const;

    std::vector<std::unique_ptr<IFeature>> m_features;
};
