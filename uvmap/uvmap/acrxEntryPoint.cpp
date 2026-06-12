// acrxEntryPoint.cpp : Điểm vào ObjectARX cho module uvmap (nạp bằng APPLOAD).
//
// Vai trò BẮT BUỘC: đăng ký các custom entity với runtime ARX bằng rxInit() lúc
// load và gỡ bằng deleteAcRxClass() lúc unload. Thiếu việc này, AutoCAD không
// dispatch được subWorldDraw / dwgInFields / dwgOutFields cho entity → entity
// không vẽ được và khi save sẽ biến thành proxy.
//
// File cũng đăng ký lệnh người dùng (UVMAPPATH) ánh xạ sang tính năng tương ứng.
// C API (uvmap_*) trong uvmap_api.cpp vẫn export song song bình thường.
#include "pch.h"

#include "AcDbUVMap.h"
#include "AcDbUVGrid.h"
#include "AcDbUVFace.h"
#include "AcDbTriangle.h"
#include "UVMap/AutoCadUVMap.h"

#include <aced.h>          // acedRegCmds, ACRX_CMD_MODAL
#include <rxregsvc.h>      // acrxRegisterAppMDIAware, acrxUnlockApplication
#include <rxdlinkr.h>      // acrxDynamicLinker

// Nhóm lệnh dùng để add/remove command qua acedRegCmds.
static const ACHAR* kUvmapCmdGroup = ACRX_T("UVMAP");

// ----- Các lệnh -----
// Tạo UV map dọc theo polyline/alignment người dùng chọn.
static void uvmap_cmdFollowPath()
{
	AutoCadUVMap feature;
	feature.uvmapFollowPath();
}

// ----- Đăng ký / gỡ custom entity -----
static void uvmap_registerClasses()
{
	// Đăng ký lớp nền trước lớp dẫn xuất. Cả 4 đều kế thừa AcDbEntity (đã có sẵn),
	// nên thứ tự giữa chúng không quan trọng; gọi acrxBuildClassHierarchy() sau cùng.
	CAcDbTriangle::rxInit();
	CAcDbUVFace::rxInit();
	CAcDbUVGrid::rxInit();
	CAcDbUVMap::rxInit();
	acrxBuildClassHierarchy();
}

static void uvmap_unregisterClasses()
{
	// Gỡ theo thứ tự ngược với đăng ký.
	deleteAcRxClass(CAcDbUVMap::desc());
	deleteAcRxClass(CAcDbUVGrid::desc());
	deleteAcRxClass(CAcDbUVFace::desc());
	deleteAcRxClass(CAcDbTriangle::desc());
}

// ----- Entry point ARX -----
extern "C" __declspec(dllexport)
AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
	switch (msg)
	{
	case AcRx::kInitAppMsg:
		acrxDynamicLinker->unlockApplication(appId);
		acrxRegisterAppMDIAware(appId);
		uvmap_registerClasses();
		acedRegCmds->addCommand(kUvmapCmdGroup,
			ACRX_T("UVMAPPATH"), ACRX_T("UVMAPPATH"),
			ACRX_CMD_MODAL, &uvmap_cmdFollowPath);
		break;

	case AcRx::kUnloadAppMsg:
		acedRegCmds->removeGroup(kUvmapCmdGroup);
		uvmap_unregisterClasses();
		break;

	default:
		break;
	}
	return AcRx::kRetOK;
}
