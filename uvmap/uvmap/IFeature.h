// IFeature.h : Giao diện chung cho mọi tính năng UV.
// Mỗi tính năng bạn tự viết sẽ kế thừa class này và hiện thực các hàm thuần ảo.
#pragma once

#include <string>

class IFeature
{
public:
    virtual ~IFeature() = default;

    // Tên định danh duy nhất của tính năng (dùng để gọi qua FeatureManager).
    virtual std::string Name() const = 0;

    // Mô tả ngắn (tuỳ chọn, có thể trả về chuỗi rỗng).
    virtual std::string Description() const { return {}; }

    // Thực thi tính năng. Trả về 0 nếu thành công, khác 0 nếu lỗi.
    // Tham số/đầu vào riêng của tính năng bạn lưu trong chính class của mình
    // (qua constructor hoặc các setter), nên chữ ký ở đây giữ đơn giản.
    virtual int Execute() = 0;
};
