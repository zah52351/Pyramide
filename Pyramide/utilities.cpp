#include "utilities.h"

Acad::ErrorStatus createLayer(const TCHAR* layerName, AcDbObjectId& layerId) {

	Acad::ErrorStatus errorStatus;
	AcDbLayerTable* currentLayerTbl = nullptr;
	if ((errorStatus = acdbHostApplicationServices()
		->workingDatabase()
		->getLayerTable(currentLayerTbl, AcDb::kForRead)) == Acad::eOk) {
		if ((errorStatus = currentLayerTbl->getAt(layerName, layerId, Adesk::kFalse)) != Acad::eOk) {
			AcDbLayerTableRecord* newLayerTblRecord = new AcDbLayerTableRecord();
			newLayerTblRecord->setName(layerName);
			try {
				if ((errorStatus = currentLayerTbl->upgradeOpen()) == Acad::eOk) {
					currentLayerTbl->add(newLayerTblRecord);
					newLayerTblRecord->close();
				}
				else {
					delete newLayerTblRecord;
				}
			}
			catch (std::exception& ex) {
				newLayerTblRecord->close();
				currentLayerTbl->close();
				ex.what();
			}
		}
		currentLayerTbl->close();
	}
	return errorStatus;
}

Acad::ErrorStatus createBlockRecord(const TCHAR* name) {

	Acad::ErrorStatus errorStatus;
	AcDbBlockTable* currentBlockTbl;

	if ((errorStatus = acdbHostApplicationServices()
		->workingDatabase()
		->getBlockTable(currentBlockTbl, AcDb::kForRead)) != Acad::eOk) {
		return errorStatus;
	}
	if (currentBlockTbl->has(name) == Adesk::kTrue) {
		errorStatus = Acad::eDuplicateKey;
		currentBlockTbl->close();
		return errorStatus;
	}

	AcDbBlockTableRecord* newBlockTblRecord = new AcDbBlockTableRecord();
	try {
		newBlockTblRecord->setName(name);
		newBlockTblRecord->setOrigin(AcGePoint3d::kOrigin);
		if ((errorStatus = currentBlockTbl->upgradeOpen()) != Acad::eOk) {
			delete newBlockTblRecord;
			currentBlockTbl->close();
			return errorStatus;
		}
		if ((errorStatus = currentBlockTbl->add(newBlockTblRecord)) != Acad::eOk) {
			delete newBlockTblRecord;
			currentBlockTbl->close();
			return errorStatus;
		}
	}
	catch (std::exception& ex) {
		ex.what();
		delete newBlockTblRecord;
		currentBlockTbl->close();
	}
	
	currentBlockTbl->close();

	double height;
	int sides;
	double radius;

	acedInitGet(RSG_NONEG | RSG_NOZERO, _T(""));
	if (acedGetInt(_T("\nEnter a number of sides: "), &sides) != RTNORM
		|| acedGetReal(_T("\nEnter a radius of bottom: "), &radius) != RTNORM
		|| acedGetReal(_T("\nEnter a height of pyramide: "), &height) != RTNORM) 
	{
		return errorStatus = Acad::eCreateFailed;
	}
		
	
	std::unique_ptr<AcDb3dSolid> pPyramid = std::make_unique<AcDb3dSolid>();

	//AcDb3dSolid* pPyramid = new AcDb3dSolid();
	if ((errorStatus = pPyramid->createPyramid(height, sides, radius))!= Acad::eOk) {
		return errorStatus;
	}
	
	/*AcGePoint3dArray sixPoints;
	ads_point result{};
	for (int i = 6; i > 0; --i) {
		acedGetPoint(nullptr, _T("Choose the point: "), result);
		AcGePoint3d point(result[0], result[1], result[2]);
		sixPoints.append(point);
	}
	
	AcDb2dPolyline* pPolygon = new AcDb2dPolyline
	(AcDb::k2dSimplePoly, sixPoints, 0.0, Adesk::kFalse, 1.0, 5.0, nullptr, nullptr);*/
	
	
	
	/*AcDbCircle* pLeftEye = new AcDbCircle(AcGePoint3d(0.33, 0.25, 0.0), AcGeVector3d::kZAxis, 0.1);
	AcDbCircle* pRightEye = new AcDbCircle(AcGePoint3d(-0.33, 0.25, 0.0), AcGeVector3d::kZAxis, 0.1);
	AcDbCircle* pYellowFace = new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, 1.0);
	AcDbArc* pRedMouth = new AcDbArc(AcGePoint3d(0, 0.5, 0), 1.0, 3.141592 + (3.141592 * 0.3), 3.141592 + (3.141592 * 0.7));

	pYellowFace->setColorIndex(2);
	pLeftEye->setColorIndex(5);
	pRightEye->setColorIndex(5);
	pRedMouth->setColorIndex(1);*/

	if ((errorStatus = newBlockTblRecord->appendAcDbEntity(pPyramid.get())) != Acad::eOk) {
		newBlockTblRecord->erase();
		newBlockTblRecord->close();
		return errorStatus;
	}
	newBlockTblRecord->close();
	return errorStatus = Acad::eOk;
}
