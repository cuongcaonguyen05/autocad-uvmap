#include "pch.h"
#include "CAcadUtil.h"

CAcadUtil::CAcadUtil(void)
{
}

CAcadUtil::~CAcadUtil(void)
{
}

CAcadUtil* CAcadUtil::GetInstance()
{
	static CAcadUtil __sInstance;
	return &__sInstance;
}

AcDbEntity* CAcadUtil::EntSelectEntity(const std::wstring& msg, AcGePoint3d& ptPoint, AcDb::OpenMode objOpenMode)
{
	CView* viewPtr = acedGetAcadDwgView(); if (viewPtr) { viewPtr->SetFocus(); }
	AcDbEntity* pEnt = NULL;
	AcDbObjectId mId;
	ads_point mPt;
	ads_name mEname;

	if (RTNORM == acedEntSel(msg.c_str(), mEname, mPt))
	{
		if (Acad::eOk == acdbGetObjectId(mId, mEname))
		{
			pEnt = CAcadUtil::GetEntytifromHandle(mId.handle(), objOpenMode);
			ptPoint.x = mPt[0];
			ptPoint.y = mPt[1];
			if (pEnt && objOpenMode == AcDb::kForRead)
				pEnt->close();

			return pEnt;
		}
	}
	return pEnt;
}

AcDbEntity* CAcadUtil::GetEntytifromHandle(const AcDbHandle& lHandle, AcDb::OpenMode objOpenMode, AcRxClass* type)
{
	try {
		if (lHandle.isNull())
			return NULL;

		AcDbEntity* pEnt = NULL;
		if (type != AcDbEntity::desc()) {
			pEnt = this->xrefGetEntytifromHandle(lHandle, objOpenMode, type);
			if (pEnt) {
				if (objOpenMode == AcDb::OpenMode::kForRead)
					pEnt->close();

				return pEnt;
			}
		}
		AcDbDatabase* database = acDocManager->curDocument()->database();
		if (!database) 
			database = acdbHostApplicationServices()->workingDatabase();

		if (!database)
			return NULL;

		AcDbObjectId mID = AcDbObjectId::kNull;
		Acad::ErrorStatus es = database->getAcDbObjectId(mID, false, lHandle);
		if (es != Acad::eOk)
			return NULL;

		es = acdbOpenAcDbEntity(pEnt, mID, objOpenMode);
		if (es == Acad::eOk) {
			if (objOpenMode == AcDb::OpenMode::kForRead || objOpenMode == AcDb::OpenMode::kForNotify)
				pEnt->close();

			return	pEnt;
		}
		else return NULL;
	}
	catch (int myNum) {
		return NULL;
	}
}

AcDbEntity* CAcadUtil::xrefGetEntytifromHandle(const AcDbHandle& lHandle, AcDb::OpenMode objOpenMode, AcRxClass* type) {
	AcDbDatabase* pWorkingDB = acDocManager->curDocument()->database();
	if (!pWorkingDB) 
		pWorkingDB = acdbHostApplicationServices()->workingDatabase();

	AcDbEntity* dbEnt = NULL;
	AcDbXrefGraph graph;
	if (acedGetCurDwgXrefGraph(graph) == Acad::eOk)
	{
		int numNodes = graph.numNodes();
		for (int i = 0; i < numNodes; ++i)
		{
			AcDbXrefGraphNode* node = graph.xrefNode(i);
			if (node == NULL)
				continue;

			AcDbDatabase* xrefDb = node->database();
			if (xrefDb == NULL || xrefDb == pWorkingDB)
				continue;

			acdbHostApplicationServices()->setWorkingDatabase(xrefDb);
			AcDbObjectId lID = AcDbObjectId::kNull;
			Acad::ErrorStatus es = xrefDb->getAcDbObjectId(lID, false, lHandle);
			if (es != Acad::eOk)
				continue;

			es = acdbOpenAcDbEntity(dbEnt, lID, objOpenMode);
			if (es == Acad::eOk && dbEnt->isKindOf(type))
				break;

			if (dbEnt) {
				dbEnt->close();
				dbEnt = NULL;
			}
		}
	}

	acdbHostApplicationServices()->setWorkingDatabase(pWorkingDB);

	return dbEnt;
}

bool CAcadUtil::GetDouble(const std::wstring& prompt, double defaultValue, double& outputValue)
{
	double i = 0;
	ads_initget(0, NULL);
	int adsrc = ads_getreal(prompt.c_str(), &i);
	if (adsrc <= RTERROR)
		return false;

	if (adsrc == RTNONE)
		i = defaultValue;

	outputValue = i;
	return true;
}

double CAcadUtil::GetDouble(double dVal, std::wstring& szFormat)
{
	if (szFormat == _T(""))
		szFormat = GetCadDoubleFormat();

	CString szValue;
	szValue.Format(szFormat.c_str(), dVal);
	return _wtof(szValue);
}

std::wstring CAcadUtil::GetCadDoubleFormat()
{
	AcDbDatabase* database = acDocManager->curDocument()->database();
	if (!database) 
		database = acdbHostApplicationServices()->workingDatabase();

	if (!database)
		return _T("");

	int m_nNumberFormat = database->luprec();
	CString szFormat;
	szFormat.Format(_T("0.%df"), m_nNumberFormat);
	szFormat = _T("%") + szFormat;
	return szFormat.GetString();
}

Adesk::Boolean CAcadUtil::Append(AcDbEntity* pEntity)
{
	AcDbObjectId objId;
	return CAcadUtil::Append(pEntity, objId);
}

Adesk::Boolean CAcadUtil::Append(AcDbEntity* pEntity, AcDbObjectId& objId)
{
	try
	{
		AcDbDatabase* database = acDocManager->curDocument()->database();
		if (!database) { database = acdbHostApplicationServices()->workingDatabase(); }
		if (!database) {
			return Adesk::kFalse;
		}
		SetLastError(0);
		this->addItemId(pEntity);
		AcDbBlockTable* pBlockTable = NULL;
		Acad::ErrorStatus	  es;
		es = database->getSymbolTable(pBlockTable, AcDb::kForRead);

		if (es != Acad::eOk) {
			return Adesk::kFalse;
		}

		AcDbBlockTableRecord* pBlockTableRecord;
		es = pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
		pBlockTable->close();

		if (es != Acad::eOk) {
			return Adesk::kFalse;
		}
		assert(es == Acad::eOk);
		es = pBlockTableRecord->appendAcDbEntity(pEntity);
		if (es != Acad::eOk) {
			pBlockTableRecord->close();
			pEntity->close();
			return Adesk::kFalse;
		}
		es = pBlockTableRecord->close();

		pEntity->close();

		objId = pEntity->objectId();

		return Adesk::kTrue;
	}
	catch (const std::exception& esx)
	{
		int lastError = GetLastError();
		if (pEntity) 
			pEntity->close();

		return Adesk::kFalse;
	}
}

bool CAcadUtil::addItemId(AcDbEntity* dbEnt, const unsigned long lId) {
	if (!dbEnt) {
		return false;
	}
	unsigned long itemId = lId;
	if (lId == -1) {
		char* szRandom = CGeometryUtil::getRandomString32();

		itemId = hash((unsigned char*)szRandom);
		free(szRandom);
	}
	const TCHAR* pszId = _T("ItemId");
	struct  resbuf* pRb, * pTemp;

	pRb = dbEnt->xData(pszId);

	if (pRb != NULL) {

		return false;
	}
	else {
		acdbRegApp(pszId);

		pRb = acutNewRb(AcDb::kDxfRegAppName);
		pTemp = pRb;
		const size_t nSize = _tcslen(pszId) + 1;
		pTemp->resval.rstring
			= (TCHAR*)malloc(nSize * sizeof(TCHAR));
		errno_t err = _tcscpy_s(pTemp->resval.rstring, nSize, pszId);
		assert(err == 0);
	}

	pTemp->rbnext = acutNewRb(AcDb::kDxfXdInteger32);
	pTemp = pTemp->rbnext;
	pTemp->resval.rlong = itemId;

	dbEnt->setXData(pRb);

	acutRelRb(pRb);
	return true;
}

unsigned long CAcadUtil::hash(unsigned char* str)
{
	unsigned long __hash = 5381;
	int c;

	while (c = *str++)
		__hash = ((__hash << 5) + __hash) + c; /* hash * 33 + c */

	return __hash;
}

unsigned long CAcadUtil::hash(unsigned char* str, unsigned long __hash)
{
	int c;

	while (c = *str++)
		__hash = ((__hash << 5) + __hash) + c; /* hash * 33 + c */

	return __hash;
}

unsigned long CAcadUtil::whash(const TCHAR* str)
{
	unsigned long __hash = 5381;
	int c;

	while (c = *str++)
		__hash = ((__hash << 5) + __hash) + c; /* hash * 33 + c */

	return __hash;
}

bool CAcadUtil::scanAllDatabase(std::list<AcDbEntity*>& ObjectIds, AcDbDatabase* pDb)
{
	if (!pDb)
		return false;

	AcDbXrefGraph graph;
	if (acedGetCurDwgXrefGraph(graph) == Acad::eOk)
	{
		int numNodes = graph.numNodes();
		for (int i = 0; i < numNodes; ++i)
		{
			AcDbXrefGraphNode* node = graph.xrefNode(i);
			if (node == NULL)
			{
				continue;
			}

			AcDbDatabase* xrefDb = node->database();
			if (xrefDb == NULL)
			{
				continue;
			}
			Acad::ErrorStatus es;
			AcDbBlockTable* pBlkTbl = NULL;
			AcDbBlockTableRecord* pBlkTblRcd = NULL;
			AcDbBlockTableRecordIterator* pBlkTblRcdItr = NULL;
			AcDbEntity* pEnt = NULL;
			AcDbObjectId ObjectId;

			es = xrefDb->getSymbolTable(pBlkTbl, AcDb::kForRead);
			if (es != Acad::ErrorStatus::eOk)
				continue;

			es = pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd,
				AcDb::kForRead);
			if (es != Acad::ErrorStatus::eOk)
				continue;
			pBlkTbl->close();

			pBlkTblRcd->newIterator(pBlkTblRcdItr);
			for (pBlkTblRcdItr->start(); !pBlkTblRcdItr->done();
				pBlkTblRcdItr->step())
			{
				es = pBlkTblRcdItr->getEntity(pEnt, AcDb::kForRead);
				if (es != Acad::eOk)
					continue;

				pEnt->close();
				ObjectIds.push_back(pEnt);
			}
			pBlkTblRcd->close();
		}
	}
	return true;
}

AcDbEntity* CAcadUtil::GetEntytifromObjectID(const AcDbObjectId& lID, AcDb::OpenMode objOpenMode, AcRxClass* type)
{
	if (!lID || !lID.isValid())
		return NULL;

	AcDbEntity* pEnt = NULL;
	if (type != AcDbEntity::desc()) {
		AcDbEntity* pEnt = this->xrefGetEntytifromObjectID(lID, objOpenMode, type);
		if (pEnt) {
			if (objOpenMode == AcDb::OpenMode::kForRead) {
				pEnt->close();
			}
			return pEnt;
		}
	}

	Acad::ErrorStatus es = acdbOpenAcDbEntity(pEnt, lID, objOpenMode);
	if (es == Acad::eOk)
	{
		if (objOpenMode == AcDb::OpenMode::kForRead) {
			pEnt->close();
		}
		return	pEnt;
	}
	else return NULL;
}

AcDbEntity* CAcadUtil::xrefGetEntytifromObjectID(const AcDbObjectId& lID, AcDb::OpenMode objOpenMode, AcRxClass* type)
{
	AcDbDatabase* pWorkingDB = acDocManager->curDocument()->database();
	if (!pWorkingDB) { pWorkingDB = acdbHostApplicationServices()->workingDatabase(); }
	AcDbEntity* dbEnt = NULL;
	AcDbXrefGraph graph;
	if (acedGetCurDwgXrefGraph(graph) == Acad::eOk)
	{
		int numNodes = graph.numNodes();
		for (int i = 0; i < numNodes; ++i)
		{
			AcDbXrefGraphNode* node = graph.xrefNode(i);
			if (node == NULL)
			{
				continue;
			}

			// get the xref database
			AcDbDatabase* xrefDb = node->database();
			if (xrefDb == NULL || xrefDb == pWorkingDB)
			{
				continue;
			}

			acdbHostApplicationServices()->setWorkingDatabase(xrefDb);
			Acad::ErrorStatus es = acdbOpenAcDbEntity(dbEnt, lID, objOpenMode);
			if (dbEnt)
			{
				if (dbEnt->isKindOf(type))
				{
					break;
				}
				dbEnt->close();
				dbEnt = NULL;
			}
		}
	}
	acdbHostApplicationServices()->setWorkingDatabase(pWorkingDB);
	return dbEnt;
}