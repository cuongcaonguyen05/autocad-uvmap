#pragma once
class CAcadUtil
{
protected:
	CAcadUtil(void);
public:
	~CAcadUtil(void);

	static CAcadUtil* GetInstance();

	AcDbEntity* EntSelectEntity(const std::wstring& msg, AcGePoint3d& ptPoint, AcDb::OpenMode objOpenMode = AcDb::kForWrite);
	AcDbEntity* GetEntytifromHandle(const AcDbHandle& lHandle, AcDb::OpenMode objOpenMode = AcDb::kForWrite, AcRxClass* type = AcDbEntity::desc());
	AcDbEntity* xrefGetEntytifromHandle(const AcDbHandle& lHandle, AcDb::OpenMode objOpenMode = AcDb::kForWrite, AcRxClass* type = AcDbEntity::desc());
	bool GetDouble(const std::wstring& prompt, double defaultValue, double& outputValue);
	double GetDouble(double dVal, std::wstring& szFormat);
	std::wstring GetCadDoubleFormat();
	Adesk::Boolean Append(AcDbEntity* pEntity);
	Adesk::Boolean Append(AcDbEntity* pEntity, AcDbObjectId& objId);
	bool addItemId(AcDbEntity* dbEnt, const unsigned long lId = -1);
	unsigned long hash(unsigned char* str);
	unsigned long hash(unsigned char* str, unsigned long __hash);
	unsigned long whash(const TCHAR* str);
	bool scanAllDatabase(std::list<AcDbEntity*>& ObjectIds, AcDbDatabase* pDb);
};