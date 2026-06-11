// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define QUADRANGLE_VERTEX 4

// add headers that you want to pre-compile here
#include "framework.h"
#include "CommonDefined.h"
#include <list>
#include <map>
#include <cmath>

// ===================== ObjectARX =====================
// Gom toàn bộ header ObjectARX hay dùng tại đây. Tính năng mới chỉ cần
// #include "pch.h" là dùng được, không phải khai báo lại từng header.
#include <tchar.h>          // _T()

#include "acdb.h"           // kiểu cơ bản (AcDbObjectId...)
#include "dbmain.h"         // AcDbEntity, AcDbObject + RTTI (desc/cast/isKindOf)
#include "dbents.h"         // các entity (AcDbLine, AcDbCircle...)
#include "dbsol3d.h"        // AcDb3dSolid (3D solid)
#include "dbpl.h"           // AcDbPolyline
#include "dbproxy.h"        // AcDbProxyEntity (cờ kEraseAllowed... cho ACRX_DXF_DEFINE_MEMBERS)
#include "dbsymtb.h"        // bảng ký hiệu (BlockTable, LayerTable...)
#include "dbapserv.h"       // acdbHostApplicationServices()
#include "acdocman.h"       // acDocManager, curDoc(), AcAp::kWrite (khóa document)
#include "acedads.h"        // hàm acedXXX (chọn entity, nhập liệu...)
#include "adslib.h"         // ADS / kiểu dữ liệu cũ
#include "geassign.h"       // chuyển đổi giữa các kiểu hình học
#include "gepnt3d.h"        // AcGePoint3d
#include "rxmfcapi.h"        // CView*, CMDIFrameWnd* (acedGetAcadDwgView()...)
#include "dbspline.h"        // CView*, CMDIFrameWnd* (acedGetAcadDwgView()...)
#include "dbxline.h"        // CView*, CMDIFrameWnd* (acedGetAcadDwgView()...)
#include "dbray.h"        // CView*, CMDIFrameWnd* (acedGetAcadDwgView()...)
#include "dbxutil.h"        // CView*, CMDIFrameWnd* (acedGetAcadDwgView()...)

// ===== BRep API (ObjectARX\2022\utils\brep\inc) — dùng cho AcBr* trong solidJson =====
#include "brbrep.h"        // AcBrBrep
#include "brmesh2d.h"      // AcBrMesh2d, AcBrMesh2dFilter, namespace brep
#include "brm2dctl.h"      // AcBrMesh2dControl (điều khiển chia lưới)
#include "brmetrav.h"      // AcBrMesh2dElement2dTraverser
#include "brentrav.h"      // AcBrElement2dNodeTraverser
#include "brnode.h"        // AcBrNode
// =====================================================

// ===== Tiện ích dùng chung toàn project (đặt SAU ObjectARX) =====
// CAcadUtil.h dùng AcDbEntity/AcGePoint3d... nên bắt buộc include sau khối ObjectARX.
// Đường dẫn có folder con vì pch.h ở thư mục gốc project.
#include <string>                      // std::wstring dùng trong CAcadUtil.h
#include "AutoCadLib/CAcadUtil.h"      // CAcadUtil::GetInstance() dùng chung toàn project
#include "AutoCadLib/CGeometryUtil.h"  // các hàm tính toán hình học dùng chung
#include "AutoCadLib/AcadOpenForWrite.h" // RAII khóa document để ghi

#endif //PCH_H
