# autocad-uvmap

Plugin/DLL cho **AutoCAD** (ObjectARX 2022) thực hiện **UV mapping** cho bề mặt đối tượng Face, Solid: sinh lưới UV, mapping theo line/polyline/..., và unwrap UV bằng [xatlas](https://github.com/jpcy/xatlas). Xuất dữ liệu hình học ra JSON/binary.

> Build ra `uvmap.dll` (custom entity AcDbEntity + C API). Chỉ hỗ trợ **x64** (ObjectARX 2022 là 64-bit).

---

## Cấu trúc thư mục

`xatlas` được đưa kèm trong repo. **`ObjectARX` KHÔNG có trong repo** (SDK độc quyền Autodesk) — bạn tự tải và đặt vào đúng vị trí:

```
autocad-uvmap/                 # gốc repo
├── ObjectARX/2022/            # ⚠️ TỰ TẢI (không có trong repo)
│   ├── inc/  inc-x64/
│   └── lib-x64/
├── xatlas/                    # có trong repo (MIT, đã custom)
│   └── build/vs2019/bin/x86_64/{Debug,Release}/xatlas_static.lib
└── uvmap/                     # solution
    ├── uvmap.sln
    └── uvmap/                 # project (.vcxproj + source)
```

> Project tham chiếu phụ thuộc bằng đường dẫn tương đối (`..\..\ObjectARX`, `..\..\xatlas`) tính từ thư mục project, nên **phải giữ nguyên cây thư mục trên**.

### Lấy ObjectARX 2022
Tải **ObjectARX SDK 2022** từ trang Autodesk Developer (cần đăng ký), giải nén và đặt sao cho có `ObjectARX/2022/inc`, `inc-x64`, `lib-x64` như cây thư mục trên. Đây là điều kiện bắt buộc để build (không kèm trong repo vì lý do license).

## Yêu cầu

| Thành phần | Phiên bản |
|-----------|-----------|
| Visual Studio | 2022 (toolset **v143**) |
| Use of MFC | Use MFC in a Shared DLL |
| AutoCAD | 2022 (để nạp/test DLL) |

## Build

1. Mở `uvmap/uvmap.sln` bằng Visual Studio 2022.
2. Chọn cấu hình **Debug | x64** (hoặc Release | x64).
3. Build (Ctrl+Shift+B) → `uvmap/uvmap/x64/Debug/uvmap.dll`.

> **CRT phải khớp**: Debug link xatlas bản **Debug**, Release link bản **Release** (trộn → lỗi LNK2038). Hiện đã set sẵn cho **Debug|x64**; build Release|x64 cần thêm include/lib path + `xatlas_static.lib` cho cấu hình Release.

## Cấu trúc source (project `uvmap`)

| Nhóm (filter) | Nội dung |
|---------------|----------|
| **AutoCadLib** | Tiện ích chung: `CAcadUtil`, `CGeometryUtil`, `CommonDefined` |
| **GE** | Hình học: `CAcGeUVMap`, `CAcGeUVGrid`, `CAcGeUVFace`, `CAcGeQuadrangle2d`, `CAcGeTriangle`, `CvGePolygon2D` |
| **DB** | Custom entity + JSON: `CAcDbUVMap`, `CAcDbUVGrid`, `CAcDbUVFace`, `CAcDbQuadrangle2d`, `CAcDbTriangle`, `CGeoJson`, `C3dFaceJson`, `CReadWriteJson` |
| **UVMap** | Các tính năng dùng cho lần lượt các đối tượng trong AutoCad hoặc Cad tương đương |
| (gốc) | Hạ tầng: `dllmain`, `pch`, `uvmap_api` (C API export), `FeatureManager`, `IFeature` |

- `pch.h` gom sẵn header ObjectARX + MFC + tiện ích dùng chung → file mới chỉ cần `#include "pch.h"`.
- Mọi `.cpp` phải có `#include "pch.h"` ở dòng đầu (project bật "Use PCH").
- Đã định nghĩa `_CRT_SECURE_NO_WARNINGS` (code port dùng `strncpy`/`memcpy`...).

## License / lưu ý

- **ObjectARX** KHÔNG đi kèm repo (SDK độc quyền Autodesk, cấm phân phối lại) → bạn tự tải.
- **xatlas**: giấy phép MIT (giữ nguyên file LICENSE trong thư mục `xatlas/`).
