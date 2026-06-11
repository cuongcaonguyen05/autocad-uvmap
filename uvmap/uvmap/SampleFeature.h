// SampleFeature.h : MẪU một tính năng. Copy file này để tạo tính năng mới của bạn.
//
// Quy tắc viết một tính năng:
//   1. Kế thừa IFeature.
//   2. Đặt Name() trả về tên duy nhất (dùng để gọi qua FeatureManager::Run).
//   3. Lưu đầu vào riêng trong class (qua constructor / setter).
//   4. Viết logic trong Execute(), trả về 0 nếu thành công.
#pragma once

#include "IFeature.h"

#include <cmath>

class SampleFeature : public IFeature
{
public:
    // Đầu vào của tính năng: truyền vào lúc tạo.
    SampleFeature(float x, float y, float z)
        : m_x(x), m_y(y), m_z(z) {}

    std::string Name() const override { return "sample"; }
    std::string Description() const override { return "Vi du: chieu cau mot diem 3D ra UV"; }

    int Execute() override
    {
        const float len = std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
        if (len < 1e-6f)
            return 1;  // điểm không hợp lệ -> báo lỗi

        const float nx = m_x / len, ny = m_y / len, nz = m_z / len;

        // PI là macro dùng chung khai báo trong CommonDefined.h.
        // Kết quả lưu vào member để bên ngoài đọc lại sau khi chạy.
        m_u = 0.5f + static_cast<float>(std::atan2(nz, nx) / (2.0 * PI));
        m_v = 0.5f - static_cast<float>(std::asin(ny) / PI);
        return 0;
    }

    // Kết quả đầu ra.
    float ResultU() const { return m_u; }
    float ResultV() const { return m_v; }

private:
    float m_x, m_y, m_z;
    float m_u = 0.0f, m_v = 0.0f;
};
