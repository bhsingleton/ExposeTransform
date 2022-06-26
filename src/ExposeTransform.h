#ifndef _ExposeTransform
#define _ExposeTransform
//
// File: ExposeTm.h
//
// Dependency Graph Node: exposeTm
//
// Author: Benjamin H. Singleton
//

#define _USE_MATH_DEFINES

#include <maya/MPxTransform.h>
#include <maya/MPxTransformationMatrix.h>
#include <maya/MObject.h>
#include <maya/MObjectHandle.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MMatrix.h>
#include <maya/MDistance.h>
#include <maya/MAngle.h>
#include <maya/MTime.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MDGModifier.h>
#include <maya/MDGContext.h>
#include <maya/MFileIO.h>
#include <maya/MAnimControl.h>
#include <maya/MTypeId.h> 
#include <maya/MGlobal.h>
#include <math.h>


enum class AxisOrder
{

	xyz = 1,
	xzy = 2,
	yzx = 3,
	yxz = 4,
	zxy = 5,
	zyx = 6,
	xyx = 7,
	yzy = 8,
	zxz = 9

};

 
class ExposeTransform : public MPxTransform
{

public:

						ExposeTransform();
	virtual				~ExposeTransform();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data);
	virtual	bool		setInternalValue(const MPlug& plug, const MDataHandle& dataHandle);
	virtual	MStatus		legalConnection(const MPlug& plug, const MPlug& otherPlug, bool asSrc, bool& isLegal);
	virtual	MStatus		connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	virtual	MStatus		connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	static  void*		creator();
	static  MStatus		initialize();

	static	MStatus		connectPlugs(const MPlug& plug, const MPlug& otherPlug, const bool force);
	static	MStatus		disconnectPlugs(const MPlug& plug, const MPlug& otherPlug);
	static	MStatus		breakConnections(const MPlug& plug, const bool source, const bool destination);
	static	MObject		createMatrixData(const MMatrix& matrix, MStatus* status);
	static	MStatus		resetMatrixPlug(MPlug& plug);

	static	MMatrix		normalizeMatrix(const MMatrix& matrix);
	static	MMatrix		createMatrix(const MVector& xAxis, const MVector& yAxis, const MVector& zAxis, const MPoint& position);
	static	void		breakMatrix(const MMatrix& matrix, MVector& xAxis, MVector& yAxis, MVector& zAxis, MPoint& position);
	static	MDistance	distanceBetween(const MMatrix& startMatrix, const MMatrix& endMatrix);
	static	MAngle		angleBetween(const MMatrix& startMatrix, const MMatrix& endMatrix);

	static	MVector		matrixToPosition(const MMatrix& matrix);
	static	MVector		matrixToEulerAngles(const MMatrix& matrix, const AxisOrder axisOrder);
	static	MVector		matrixToEulerXYZ(const MMatrix& matrix);
	static	MVector		matrixToEulerXZY(const MMatrix& matrix);
	static	MVector		matrixToEulerYZX(const MMatrix& matrix);
	static	MVector		matrixToEulerYXZ(const MMatrix& matrix);
	static	MVector		matrixToEulerZXY(const MMatrix& matrix);
	static	MVector		matrixToEulerZYX(const MMatrix& matrix);
	static	MVector		matrixToEulerXYX(const MMatrix& matrix);
	static	MVector		matrixToEulerYZY(const MMatrix& matrix);
	static	MVector		matrixToEulerZXZ(const MMatrix& matrix);
	
public:
	
	static	MObject		exposeNode;
	static	MObject		exposeMatrix;
	static	MObject		localReferenceNode;
	static	MObject		localReferenceMatrix;
	static	MObject		useParent;
	static	MObject		useTimeOffset;
	static	MObject		timeOffset;
	
	static	MObject		localPosition;
	static	MObject		localPositionX;
	static	MObject		localPositionY;
	static	MObject		localPositionZ;
	static	MObject		worldPosition;
	static	MObject		worldPositionX;
	static	MObject		worldPositionY;
	static	MObject		worldPositionZ;
	static	MObject		eulerXOrder;
	static	MObject		eulerYOrder;
	static	MObject		eulerZOrder;
	static	MObject		localEuler;
	static	MObject		localEulerX;
	static	MObject		localEulerY;
	static	MObject		localEulerZ;
	static	MObject		worldEuler;
	static	MObject		worldEulerX;
	static	MObject		worldEulerY;
	static	MObject		worldEulerZ;
	static	MObject		stripNUScale;
	static	MObject		distance;
	static	MObject		angle;
	
	static	MTypeId		id;
	static	MString		classification;
	static	MString		outputCategory;
	
private:

			MObjectHandle	exposeHandle;
			MObjectHandle	localReferenceHandle;
			bool			parentEnabled;

	virtual	MStatus			updateExposeMatrix();
	virtual MStatus			updateLocalReferenceMatrix();

};

#endif