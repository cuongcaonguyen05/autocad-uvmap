# cuongcaonguyen05 — Project conventions

File này định nghĩa quy tắc bắt buộc khi viết/review code trong repo. Áp dụng cho mọi task.

## Workflow

1. **Điều tra trước khi sửa** — đọc code, hiểu context, đưa ra phương án.
2. **Trong quá trình điều tra: user pre-approve MỌI THỨ** — đọc file, grep, glob, spawn agent, chạy bash/powershell command, chạy python script, build project, chạy test, chạy git read-only (log/diff/status/show), fetch web, gọi MCP tool... cứ làm tự do, KHÔNG hỏi xin phép, KHÔNG dừng lại confirm từng bước. User đã allow hết trong phase điều tra.
3. **Chỉ hỏi khi sắp edit code** — sau khi đưa phương án xong, hỏi user confirm rồi mới edit file.
4. Khi user đã confirm "ok, làm đi" / "OK" / "được" → edit luôn, không hỏi tiếp từng bước.
5. **Sau khi edit xong** — hỏi user "bạn có cần build lại để kiểm tra không?" KHÔNG tự build, KHÔNG tự chạy test. Việc build/test do user quyết định.
6. Không tự ý refactor code ngoài scope của task.

## C++ code style

### 1. Không dùng lambda

Tuyệt đối **không** dùng `[&](){...}`, `[=](){...}`, `auto fn = [](){...}`, template callback `F&&`.

Thay bằng:
- **Static file-scope function** trong file `.cpp`
- **Member function** của class (private/public)

Pattern sai:
```cpp
template<typename F>
static void __apply(const AcDbObjectId& id, F&& fn) { ... fn(p); ... }   // ❌
__apply(id, [&](CAcDbXxx* p) { p->Do(); });                              // ❌
auto fn = [&](int x) { return x + n; };                                  // ❌
```

Pattern đúng — inline trực tiếp hoặc tách thành function có tên:
```cpp
AcDbEntity* dbEnt = CAcadUtil::GetInstance()->GetEntytifromObjectID(id, AcDb::kForWrite);
if (!dbEnt) return;
CAcDbXxx* p = CAcDbXxx::cast(dbEnt);
if (!p) { dbEnt->close(); return; }
p->assertWriteEnabled();
p->Do();
p->close();
```

Không dùng `std::function` (thường đi kèm lambda).

### 2. Class style

Mọi **class** (bao gồm MFC dialog, custom entity, controller, ...):

- **Member variables = `private`** — không `public`, không `protected` trừ khi subclass thực sự cần.
- **Khởi tạo đầy đủ trong constructor initializer list** — mọi field có giá trị default rõ ràng, không để uninit.
- **Expose qua `SetXxx` / `GetXxx const`** — không cho caller chạm `dlg.m_xxx` trực tiếp.
- **Header chỉ declare, body trong `.cpp`** — KHÔNG inline body trong header kể cả 1-line setter/getter.

Header `.h` — chỉ declare:
```cpp
class CMyDlg : public CDialog {
public:
    CMyDlg();
    void    SetEnabled(BOOL b);
    BOOL    GetEnabled() const;
    void    SetName(const CString& s);
    CString GetName() const;
private:
    BOOL    m_bEnabled;
    CString m_strName;
};
```

Implementation `.cpp`:
```cpp
CMyDlg::CMyDlg()
    : m_bEnabled(FALSE)
    , m_strName(_T(""))
{}

void    CMyDlg::SetEnabled(BOOL b)        { m_bEnabled = b; }
BOOL    CMyDlg::GetEnabled() const        { return m_bEnabled; }
void    CMyDlg::SetName(const CString& s) { m_strName = s; }
CString CMyDlg::GetName() const           { return m_strName; }
```

Caller — luôn qua Set/Get:
```cpp
dlg.SetEnabled(TRUE);      // ✅
v = dlg.GetName();         // ✅
dlg.m_bEnabled = TRUE;     // ❌
```

### 3. Tên hàm — camelCase, không dùng `_` tiền tố

Private/protected helper functions đặt tên theo camelCase, **không** dùng underscore tiền tố:

```cpp
// ❌ Sai
void _InitLayout();
void _SetupGrid();
static CString _FieldToStr(...);

// ✅ Đúng
void initLayout();
void setupGrid();
static CString fieldToStr(...);
```

Quy tắc: ký tự đầu tiên của tên hàm viết thường. Áp dụng cho cả public, private, static.

### 4. Tách nhỏ hàm

- Không gom logic lớn trong 1 hàm — tách thành nhiều hàm nhỏ có tên rõ ràng.
- Nếu thấy mình sắp viết `auto` + lambda để tái sử dụng → tách thành static helper.
- Nếu cần nhóm data + behavior → dùng **class**, không dùng `struct`.

## ObjectARX rules

### `assertWriteEnabled()` sau `kForWrite`

Khi mở entity để edit, pattern đầy đủ:
```cpp
AcDbEntity* dbEnt = CAcadUtil::GetInstance()->GetEntytifromObjectID(id, AcDb::kForWrite);
if (!dbEnt) return;
CAcDbXxx* p = CAcDbXxx::cast(dbEnt);
if (!p) { dbEnt->close(); return; }
p->assertWriteEnabled();      // ← BẮT BUỘC trước khi mutate
p->SetSomething(val);         // mutate
p->close();                   // đóng sau khi xong
```

Lý do: `assertWriteEnabled()` kiểm tra object đã open `kForWrite` đúng cách. Thiếu → mutate trên read-only object → DB hỏng / runtime crash khó debug.

Không cần cho `kForRead`.

### `close()` và pointer kForRead

Sau `dbEnt->close()` với `kForRead`, pointer vẫn valid để đọc member — KHÔNG phải use-after-free. Chỉ cần tránh gọi method mutate.

### Custom entity checklist

Khi tạo custom entity mới trong `Rds.System.Db.Objects`, phải có đủ:

- `ACRX_DECLARE_MEMBERS` + `ACRX_DXF_DEFINE_MEMBERS`
- Constructor/destructor
- `dwgOutFields` / `dwgInFields` — versioned persistence
- `subWorldDraw` (R18) / `worldDraw` (pre-R18)
- `subGetGripPoints` / `subMoveGripPointsAt`
- `subTransformBy`
- `subExplode`
- `subGetGeomExtents` — cho zoom extents
- `subGetOsnapPoints` (3 overloads, gọi helper `_collectSnapPoints`)
- Đăng ký `rxInit` / `deleteAcRxClass` trong `acrxEntryPoint.cpp`
- Thêm vào `.vcxproj`: `ClCompile`, `ClInclude`, copy header vào `..\include` trong mỗi configuration's PostBuildEvent
- Class khai báo với macro `DLLDBIMPEXP` (từ `System.dbx.header.h`) để export cho project khác

## Design patterns (GoF) — ưu tiên áp dụng

Khi **viết code mới**, **review code**, hoặc **thiết kế hệ thống**, ưu tiên nhận diện và áp dụng 23 mẫu GoF. Đề xuất pattern phù hợp thay vì code ad-hoc.

### 1. Creational (khởi tạo) — 5 mẫu
Singleton, Factory Method, Abstract Factory, Builder, Prototype.

Mục đích: che giấu logic tạo object, tránh `new` trực tiếp rải rác, linh hoạt khi chọn class nào được tạo ra theo context.

### 2. Structural (cấu trúc) — 7 mẫu
Adapter, Bridge, Composite, Decorator, Facade, Flyweight, Proxy.

Mục đích: định nghĩa quan hệ giữa class/object. Hệ thống càng lớn càng quan trọng. Theo dõi bằng class diagram.

### 3. Behavioral (hành vi) — 11 mẫu
Interpreter, Template Method, Chain of Responsibility, Command, Iterator, Mediator, Memento, Observer, State, Strategy, Visitor.

Mục đích: định nghĩa cách các object giao tiếp và phân chia trách nhiệm.

### Cách áp dụng

- **Khi review**: nhìn code thấy pattern nào phù hợp thì nêu ra, giải thích WHY.
- **Khi design**: đề xuất pattern + trade-off trước khi implement.
- **Khi refactor**: nhận diện code smell → map sang pattern tương ứng.
- **Không over-engineer**: chỉ áp dụng khi thực sự giải quyết vấn đề, không ép pattern cho code 10 dòng.

## Debug methodology

Khi gặp bug khó:

1. Thêm `APPMETHOD` / `APPINFO` logging ở các branch nghi ngờ.
2. Chạy **cả case đúng lẫn case sai**.
3. So sánh log để pinpoint điểm rẽ nhánh khác biệt.
4. Không guess — đọc log thật.

## Git workflow

- Không tự commit trừ khi user yêu cầu.
- Không push trừ khi user yêu cầu.
- Build artifacts (`App/`, `build/`, `.vs/`, `x64/`, `Debug/`, `Release/`) đã trong `.gitignore` — không stage.
- Skip-worktree files: nếu `git pull` báo "local changes" mà `git status` sạch → check `git ls-files -v` tìm flag `S`, xoá bằng `git update-index --no-skip-worktree`.

## Communication

- Trả lời ngắn gọn, thẳng vấn đề.
- Tiếng Việt khi user hỏi tiếng Việt.
- Code comments: tiếng Việt OK, giải thích WHY không phải WHAT.
- Không viết comment trivial ("// setter", "// return value").
