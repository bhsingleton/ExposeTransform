//
// File: ExposeTransform.cpp
//
// Dependency Graph Node: exposeTm
//
// Author: Benjamin H. Singleton
//

#include "ExposeTransform.h"

MObject		ExposeTransform::exposeNode;
MObject		ExposeTransform::exposeMatrix;
MObject		ExposeTransform::localReferenceNode;
MObject		ExposeTransform::localReferenceMatrix;
MObject		ExposeTransform::useParent;
MObject		ExposeTransform::eulerXOrder;
MObject		ExposeTransform::eulerYOrder;
MObject		ExposeTransform::eulerZOrder;
MObject		ExposeTransform::stripNUScale;
MObject		ExposeTransform::useTimeOffset;
MObject		ExposeTransform::timeOffset;

MObject		ExposeTransform::localPosition;
MObject		ExposeTransform::localPositionX;
MObject		ExposeTransform::localPositionY;
MObject		ExposeTransform::localPositionZ;
MObject		ExposeTransform::worldPosition;
MObject		ExposeTransform::worldPositionX;
MObject		ExposeTransform::worldPositionY;
MObject		ExposeTransform::worldPositionZ;
MObject		ExposeTransform::localEuler;
MObject		ExposeTransform::localEulerX;
MObject		ExposeTransform::localEulerY;
MObject		ExposeTransform::localEulerZ;
MObject		ExposeTransform::worldEuler;
MObject		ExposeTransform::worldEulerX;
MObject		ExposeTransform::worldEulerY;
MObject		ExposeTransform::worldEulerZ;
MObject		ExposeTransform::distance;
MObject		ExposeTransform::angle;

MTypeId		ExposeTransform::id(0x0013b1c8);
MString		ExposeTransform::classification("drawdb/geometry/transform/exposeTm");
MString		ExposeTransform::outputCategory("Output");


ExposeTransform::ExposeTransform()
{

	this->exposeHandle = MObjectHandle();
	this->localReferenceHandle = MObjectHandle();
	this->parentEnabled = false;

};


ExposeTransform::~ExposeTransform() {};


MStatus ExposeTransform::compute(const MPlug& plug, MDataBlock& data) 
/**
This method should be overridden in user defined nodes.
Recompute the given output based on the nodes inputs.
The plug represents the data value that needs to be recomputed, and the data block holds the storage for all of the node's attributes.
The MDataBlock will provide smart handles for reading and writing this node's attribute values.
Only these values should be used when performing computations!

@param plug: Plug representing the attribute that needs to be recomputed.
@param data: Data block containing storage for the node's attributes.
@return: Return status.
*/
{

	MStatus status;

	// Check requested attribute
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (fnAttribute.hasCategory(ExposeTransform::outputCategory))
	{

		// Edit datablock context if enabled
		//
		MDataHandle useTimeOffsetHandle = data.inputValue(ExposeTransform::useTimeOffset, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle timeOffsetHandle = data.inputValue(ExposeTransform::timeOffset, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool useTimeOffset = useTimeOffsetHandle.asBool();
		MTime timeOffset = timeOffsetHandle.asTime();

		if (useTimeOffset)
		{

			MTime currentTime = MAnimControl::currentTime();
			MDGContext context = MDGContext(currentTime + timeOffset);

			status = data.setContext(context);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

		// Get input data handles
		//
		MDataHandle exposeMatrixHandle = data.inputValue(ExposeTransform::exposeMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localReferenceMatrixHandle = data.inputValue(ExposeTransform::localReferenceMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle eulerXOrderHandle = data.inputValue(ExposeTransform::eulerXOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle eulerYOrderHandle = data.inputValue(ExposeTransform::eulerYOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle eulerZOrderHandle = data.inputValue(ExposeTransform::eulerZOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle stripNUScaleHandle = data.inputValue(ExposeTransform::stripNUScale, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get values from handles
		//
		MMatrix exposeMatrix = exposeMatrixHandle.asMatrix();
		MMatrix localReferenceMatrix = localReferenceMatrixHandle.asMatrix();

		AxisOrder eulerXOrder = AxisOrder(eulerXOrderHandle.asInt());
		AxisOrder eulerYOrder = AxisOrder(eulerYOrderHandle.asInt());
		AxisOrder eulerZOrder = AxisOrder(eulerZOrderHandle.asInt());

		bool stripNUScale = stripNUScaleHandle.asBool();
		
		// Calculate local matrix
		//
		if (stripNUScale)
		{
			
			exposeMatrix = ExposeTransform::normalizeMatrix(exposeMatrix);
			localReferenceMatrix = ExposeTransform::normalizeMatrix(localReferenceMatrix);
			
		}
		
		MMatrix localMatrix = exposeMatrix * localReferenceMatrix.inverse();
		
		// Get transform values
		//
		MVector localPosition = ExposeTransform::matrixToPosition(localMatrix);
		double localEulerX = ExposeTransform::matrixToEulerAngles(localMatrix, eulerXOrder)[0];
		double localEulerY = ExposeTransform::matrixToEulerAngles(localMatrix, eulerYOrder)[1];
		double localEulerZ = ExposeTransform::matrixToEulerAngles(localMatrix, eulerZOrder)[2];

		MVector worldPosition = ExposeTransform::matrixToPosition(exposeMatrix);
		double worldEulerX = ExposeTransform::matrixToEulerAngles(exposeMatrix, eulerXOrder)[0];
		double worldEulerY = ExposeTransform::matrixToEulerAngles(exposeMatrix, eulerYOrder)[1];
		double worldEulerZ = ExposeTransform::matrixToEulerAngles(exposeMatrix, eulerZOrder)[2];

		MDistance distanceBetween = ExposeTransform::distanceBetween(localReferenceMatrix, exposeMatrix);
		MAngle angleBetween = ExposeTransform::angleBetween(localReferenceMatrix, exposeMatrix);

		// Revert context change made to datablock
		//
		if (useTimeOffset)
		{

			MTime currentTime = MAnimControl::currentTime();
			MDGContext context = MDGContext(currentTime);

			status = data.setContext(context);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

		// Get output data handles
		//
		MDataHandle localPositionXHandle = data.outputValue(ExposeTransform::localPositionX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localPositionYHandle = data.outputValue(ExposeTransform::localPositionY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localPositionZHandle = data.outputValue(ExposeTransform::localPositionZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localEulerXHandle = data.outputValue(ExposeTransform::localEulerX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localEulerYHandle = data.outputValue(ExposeTransform::localEulerY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localEulerZHandle = data.outputValue(ExposeTransform::localEulerZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldPositionXHandle = data.outputValue(ExposeTransform::worldPositionX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldPositionYHandle = data.outputValue(ExposeTransform::worldPositionY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldPositionZHandle = data.outputValue(ExposeTransform::worldPositionZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldEulerXHandle = data.outputValue(ExposeTransform::worldEulerX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldEulerYHandle = data.outputValue(ExposeTransform::worldEulerY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldEulerZHandle = data.outputValue(ExposeTransform::worldEulerZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle distanceHandle = data.outputValue(ExposeTransform::distance, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle angleHandle = data.outputValue(ExposeTransform::angle, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update output handles
		//
		MDistance::Unit distanceUnit = MDistance::uiUnit();
		MAngle::Unit angleUnit = MAngle::internalUnit();

		localPositionXHandle.setMDistance(MDistance(localPosition.x, distanceUnit));
		localPositionXHandle.setClean();

		localPositionYHandle.setMDistance(MDistance(localPosition.y, distanceUnit));
		localPositionYHandle.setClean();

		localPositionZHandle.setMDistance(MDistance(localPosition.z, distanceUnit));
		localPositionZHandle.setClean();

		localEulerXHandle.setMAngle(MAngle(localEulerX, angleUnit));
		localEulerXHandle.setClean();

		localEulerYHandle.setMAngle(MAngle(localEulerY, angleUnit));
		localEulerYHandle.setClean();

		localEulerZHandle.setMAngle(MAngle(localEulerZ, angleUnit));
		localEulerZHandle.setClean();

		worldPositionXHandle.setMDistance(MDistance(worldPosition.x, distanceUnit));
		worldPositionXHandle.setClean();

		worldPositionYHandle.setMDistance(MDistance(worldPosition.y, distanceUnit));
		worldPositionYHandle.setClean();

		worldPositionZHandle.setMDistance(MDistance(worldPosition.z, distanceUnit));
		worldPositionZHandle.setClean();

		worldEulerXHandle.setMAngle(MAngle(worldEulerX, angleUnit));
		worldEulerXHandle.setClean();

		worldEulerYHandle.setMAngle(MAngle(worldEulerY, angleUnit));
		worldEulerYHandle.setClean();

		worldEulerZHandle.setMAngle(MAngle(worldEulerZ, angleUnit));
		worldEulerZHandle.setClean();

		distanceHandle.setMDistance(distanceBetween);
		distanceHandle.setClean();

		angleHandle.setMAngle(angleBetween);
		angleHandle.setClean();


		// Mark plug as clean
		//
		status = data.setClean(plug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return MS::kSuccess;

	}
	else
	{

		return MS::kUnknownParameter;

	}

};


bool ExposeTransform::setInternalValue(const MPlug& plug, const MDataHandle& handle)
/**
This method is overridden by nodes that store attribute data in some internal format.
The internal state of attributes can be set or queried using the setInternal and internal methods of MFnAttribute.
When internal attribute values are set via setAttr or MPlug::setValue this method is called.
Another use for this method is to impose attribute limits.

@param plug: The attribute that is being set.
@param handle: The dataHandle containing the value to set.
@return: Success.
*/
{

	// Call parent method
	// Otherwise we won't receive the value change!
	//
	bool success = MPxNode::setInternalValue(plug, handle);

	// Inspect plug attribute
	//
	MObject attribute = plug.attribute();

	if (attribute == ExposeTransform::useParent)
	{

		this->parentEnabled = handle.asBool();
		this->updateLocalReferenceMatrix();

	}

	return success;

};


MStatus ExposeTransform::legalConnection(const MPlug& plug, const MPlug& otherPlug, bool asSrc, bool& isLegal)
/**
This method allows you to check for legal connections being made to attributes of this node.
You should return kUnknownParameter to specify that maya should handle this connection if you are unable to determine if it is legal.

@param plug: Attribute on this node.
@param otherPlug: Attribute on the other node.
@param asSrc: Is this plug a source of the connection.
@param isLegal: Set this to true if the connection is legal, false otherwise.
@return: Return status.
*/
{

	// Inspect plug attribute
	//
	MObject attribute = plug.attribute();

	if ((attribute == ExposeTransform::exposeNode || attribute == ExposeTransform::localReferenceNode) && !asSrc)
	{

		// Evaluate if other node is a transform
		//
		MObject otherNode = otherPlug.node();
		isLegal = otherNode.hasFn(MFn::kTransform);

		return MS::kSuccess;

	}
	else
	{

		return MS::kUnknownParameter;

	}

};


MStatus ExposeTransform::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
/**
This method gets called when connections are made to attributes of this node.
You should return kUnknownParameter to specify that maya should handle this connection or if you want maya to process the connection as well.

@param plug: Attribute on this node.
@param otherPlug: Attribute on the other node.
@param asSrc: Is this plug a source of the connection.
@return: Return status.
*/
{

	// Inspect plug attribute
	//
	MObject attribute = plug.attribute();

	if (attribute == ExposeTransform::exposeNode && !asSrc)
	{

		// Store node handle
		//
		MObject otherNode = otherPlug.node();
		this->exposeHandle = MObjectHandle(otherNode);
		this->updateExposeMatrix();
		this->updateLocalReferenceMatrix();

		return MS::kSuccess;

	}
	else if (attribute == ExposeTransform::localReferenceNode && !asSrc)
	{

		// Store node handle
		//
		MObject otherNode = otherPlug.node();
		this->localReferenceHandle = MObjectHandle(otherNode);
		this->updateLocalReferenceMatrix();

		return MS::kSuccess;

	}
	else
	{

		return MS::kUnknownParameter;

	}

};


MStatus ExposeTransform::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
/**
This method gets called when connections are made to attributes of this node.
You should return kUnknownParameter to specify that maya should handle this connection or if you want maya to process the connection as well.

@param plug: Attribute on this node.
@param otherPlug: Attribute on the other node.
@param asSrc: Is this plug a source of the connection.
@return: Return status.
*/
{

	// Inspect plug attribute
	//
	MObject attribute = plug.attribute();

	if (attribute == ExposeTransform::exposeNode && !asSrc)
	{

		// Destroy node handle
		//
		this->exposeHandle = MObjectHandle();
		this->updateExposeMatrix();
		this->updateLocalReferenceMatrix();

		return MS::kSuccess;

	}
	else if (attribute == ExposeTransform::localReferenceNode && !asSrc)
	{

		// Destroy node handle
		//
		this->localReferenceHandle = MObjectHandle();
		this->updateLocalReferenceMatrix();

		return MS::kSuccess;

	}
	else
	{

		return MS::kUnknownParameter;

	}

};


MStatus ExposeTransform::connectPlugs(const MPlug& plug, const MPlug& otherPlug, const bool force)
/**
Connects the two supplied plugs.
Using force will break any pre-existing connections.

@param plug: The source plug.
@param otherPlug: The destination plug.
@param force: Breaks pre-existing connections.
@return: Status code.
*/
{

	MStatus status;

	// Check if destination already has connections
	//
	bool isConnected = otherPlug.isConnected(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (isConnected && force)
	{

		status = ExposeTransform::breakConnections(otherPlug, true, false);
		CHECK_MSTATUS_AND_RETURN_IT(status);

	}

	// Setup dag modifier
	//
	MDGModifier modifier;

	status = modifier.connect(plug, otherPlug);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Execute modifier
	//
	status = modifier.doIt();
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;

};


MStatus ExposeTransform::disconnectPlugs(const MPlug& plug, const MPlug& otherPlug)
/**
Disconnects the two supplied plugs.

@param plug: The source plug.
@param otherPlug: The destination plug.
@return: Status code.
*/
{

	MStatus status;

	// Setup dag modifier
	//
	MDGModifier modifier;

	status = modifier.disconnect(plug, otherPlug);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Execute modifier
	//
	status = modifier.doIt();
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;

};


MStatus ExposeTransform::breakConnections(const MPlug& plug, bool source, bool destination)
/**
Breaks the connections to the supplied plug.

@param plug: The plug to break connections on.
@param source: Break input connections.
@param destination: Break output connections.
@return: Status code.
*/
{

	MStatus status;

	// Check if source plugs should be broken
	//
	bool isConnected = plug.isConnected(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (source && isConnected)
	{

		MPlug otherPlug = plug.source(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		ExposeTransform::disconnectPlugs(otherPlug, plug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

	}

	// Check if destination plugs should be broken
	//
	MPlugArray otherPlugs = MPlugArray();
	bool hasConnections = plug.destinations(otherPlugs, &status);

	if (destination && hasConnections)
	{

		unsigned int numDestinations = otherPlugs.length();
		MPlug otherPlug;

		for (unsigned int i = 0; i < numDestinations; i++)
		{

			otherPlug = otherPlugs[i];

			ExposeTransform::disconnectPlugs(plug, otherPlug);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

	}

	return status;

};


MObject ExposeTransform::createMatrixData(const MMatrix& matrix, MStatus* status)
/**
Returns a matrix data object from the supplied matrix.

@param matrix: The matrix to convert.
@param status: Status code.
@return: Matrix data object.
*/
{

	// Create new matrix data
	//
	MFnMatrixData fnMatrixData;

	MObject matrixData = fnMatrixData.create(status);
	CHECK_MSTATUS_AND_RETURN(*status, MObject::kNullObj);

	// Assign identity matrix
	//
	*status = fnMatrixData.set(matrix);
	CHECK_MSTATUS_AND_RETURN(*status, MObject::kNullObj);

	return matrixData;

};


MStatus ExposeTransform::resetMatrixPlug(MPlug& plug)
/**
Resets the matrix value on the supplied plug.

@param plug: The plug to reset.
@return: Status code.
*/
{

	MStatus status;

	// Assign identity matrix to plug
	//
	MObject matrixData = ExposeTransform::createMatrixData(MMatrix::identity, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plug.setMObject(matrixData);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;

};


MStatus ExposeTransform::updateExposeMatrix()
{

	MStatus status;

	// Evaluate if file is being opened
	// Any DAG changes will interfere with file IO operations!
	//
	bool isOpeningFile = MFileIO::isOpeningFile();
	bool isImportingFile = MFileIO::isImportingFile();

	if (isOpeningFile || isImportingFile)
	{

		return MS::kSuccess;

	}

	// Check if expose node is valid
	//
	bool isExposeNodeValid = this->exposeHandle.isValid() && this->exposeHandle.isAlive();

	if (isExposeNodeValid)
	{

		// Get expose node's ".worldMatrix" plug
		//
		MObject exposeNode = this->exposeHandle.object();
		MDagPath dagPath = MDagPath::getAPathTo(exposeNode);

		MFnDagNode fnDagNode(dagPath);
		MPlug plug = fnDagNode.findPlug("worldMatrix", true);

		status = plug.selectAncestorLogicalIndex(dagPath.instanceNumber());
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get destination plug
		//
		MPlug otherPlug = MPlug(this->thisMObject(), ExposeTransform::exposeMatrix);

		// Connect plugs
		//
		status = ExposeTransform::connectPlugs(plug, otherPlug, true);
		CHECK_MSTATUS_AND_RETURN_IT(status);

	}
	else
	{

		// Break connections to ".worldMatrix" plug
		//
		MPlug plug = MPlug(this->thisMObject(), ExposeTransform::exposeMatrix);

		status = ExposeTransform::breakConnections(plug, true, false);
		CHECK_MSTATUS_AND_RETURN_IT(status);

	}

	return status;

};


MStatus ExposeTransform::updateLocalReferenceMatrix()
{

	MStatus status;

	// Evaluate if file is being opened
	// Any DAG changes will interfere with file IO operations!
	//
	bool isOpeningFile = MFileIO::isOpeningFile();
	bool isImportingFile = MFileIO::isImportingFile();

	if (isOpeningFile || isImportingFile)
	{

		return MS::kSuccess;

	}

	// Check if parent space is enabled
	//
	bool isExposeNodeValid = this->exposeHandle.isValid() && this->exposeHandle.isAlive();
	bool isLocalReferenceNodeValid = this->localReferenceHandle.isValid() && this->localReferenceHandle.isAlive();

	if (this->parentEnabled)
	{

		// Check if expose node is valid
		//
		if (isExposeNodeValid)
		{

			// Get dag path to expose node
			//
			MObject exposeNode = this->exposeHandle.object();

			MDagPath dagPath = MDagPath::getAPathTo(exposeNode, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			unsigned int instanceNumber = dagPath.instanceNumber(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get expose node's ".parentMatrix" plug
			//
			MFnDagNode fnDagNode(dagPath, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MPlug plug = fnDagNode.findPlug("parentMatrix", true, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MPlug element = plug.elementByLogicalIndex(instanceNumber, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get destination plug
			//
			MPlug otherPlug = MPlug(this->thisMObject(), ExposeTransform::localReferenceMatrix);

			// Connect plugs
			//
			status = ExposeTransform::connectPlugs(element, otherPlug, true);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}
		else
		{

			// Break connections to ".localReferenceMatrix" plug
			//
			MPlug plug = MPlug(this->thisMObject(), ExposeTransform::localReferenceMatrix);

			status = ExposeTransform::breakConnections(plug, true, false);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Reset plug value
			//
			status = ExposeTransform::resetMatrixPlug(plug);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

	}
	else
	{

		// Check if reference node is valid
		//
		if (isLocalReferenceNodeValid)
		{

			// Get dag path to local reference node
			//
			MObject localReferenceNode = this->localReferenceHandle.object();

			MDagPath dagPath = MDagPath::getAPathTo(localReferenceNode, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			unsigned int instanceNumber = dagPath.instanceNumber(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get local reference node's ".worldMatrix" plug
			//
			MFnDagNode fnDagNode(dagPath, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MPlug plug = fnDagNode.findPlug("worldMatrix", true, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MPlug element = plug.elementByLogicalIndex(instanceNumber, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get destination plug
			//
			MPlug otherPlug = MPlug(this->thisMObject(), ExposeTransform::localReferenceMatrix);

			// Connect plugs
			//
			status = ExposeTransform::connectPlugs(element, otherPlug, true);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}
		else
		{

			// Break connections to ".localReferenceMatrix" plug
			//
			MPlug plug = MPlug(this->thisMObject(), ExposeTransform::localReferenceMatrix);

			status = ExposeTransform::breakConnections(plug, true, false);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Reset plug value
			//
			status = ExposeTransform::resetMatrixPlug(plug);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

	}

	return status;

};


MMatrix ExposeTransform::normalizeMatrix(const MMatrix& matrix)
/**
Returns a normalized matrix from the supplied transform matrix.

@param matrix: The matrix to normalize.
@return: Normalized matrix.
*/
{

	// Break matrix
	//
	MVector xAxis, yAxis, zAxis;
	MPoint position;

	ExposeTransform::breakMatrix(matrix, xAxis, yAxis, zAxis, position);

	// Normalize axis vectors
	//
	xAxis.normalize();
	yAxis.normalize();
	zAxis.normalize();

	// Compose new matrix
	//
	return ExposeTransform::createMatrix(xAxis, yAxis, zAxis, position);

};


void ExposeTransform::breakMatrix(const MMatrix& matrix, MVector& xAxis, MVector& yAxis, MVector& zAxis, MPoint& position)
/**
Breaks the supplied matrix into its axis vector and position components.

@param matrix: The matrix to break.
@param xAxis: The x-axis vector.
@param yAxis: The y-axis vector.
@param zAxis: The z-axis vector.
@param position: The positional value.
@return: Void.
*/
{

	xAxis = MVector(matrix[0]);
	yAxis = MVector(matrix[1]);
	zAxis = MVector(matrix[2]);
	position = MVector(matrix[3]);

};


MMatrix ExposeTransform::createMatrix(const MVector& xAxis, const MVector& yAxis, const MVector& zAxis, const MPoint& position)
/**
Returns a matrix from the supplied axis vectors and position.

@param xAxis: The x-axis vector.
@param yAxis: The y-axis vector.
@param zAxis: The z-axis vector.
@param position: The positional value.
@return: Transform matrix.
*/
{

	double rows[4][4] = {
		{xAxis.x, xAxis.y, xAxis.z, 0.0},
		{yAxis.x, yAxis.y, yAxis.z, 0.0},
		{zAxis.x, zAxis.y, zAxis.z, 0.0},
		{position.x, position.y, position.z, 1.0}
	};

	return MMatrix(rows);

};


MDistance ExposeTransform::distanceBetween(const MMatrix& startMatrix, const MMatrix& endMatrix)
/**
Evaluates the distance between the two matrices.

@param startMatrix: The start matrix.
@param endMatrix: The end matrix.
@return: The distance in UI units.
*/
{

	MPoint startPoint = ExposeTransform::matrixToPosition(startMatrix);
	MPoint endPoint = ExposeTransform::matrixToPosition(endMatrix);

	return MDistance(startPoint.distanceTo(endPoint), MDistance::uiUnit());

};


MAngle ExposeTransform::angleBetween(const MMatrix& startMatrix, const MMatrix& endMatrix)
/**
Evaluates the angle between the two matrices.

@param startMatrix: The start matrix.
@param endMatrix: The end matrix.
@return: The angle in radians.
*/
{

	MVector startVector = ExposeTransform::matrixToPosition(startMatrix).normal();
	MVector endVector = ExposeTransform::matrixToPosition(endMatrix).normal();

	return MAngle(startVector.angle(endVector), MAngle::Unit::kRadians);

};


MVector ExposeTransform::matrixToPosition(const MMatrix& matrix)
/**
*/
{

	return MVector(matrix[3]);

};


MVector ExposeTransform::matrixToEulerAngles(const MMatrix& matrix, const AxisOrder axisOrder)
/**
Converts the supplied transform matrix into euler angles using the specified axis order.

@param matrix: The matrix to convert.
@param axis: The axis order for the euler angles.
@return: The angles in radians.
*/
{

	MMatrix normalizedMatrix = ExposeTransform::normalizeMatrix(matrix);

	switch (axisOrder)
	{

	case AxisOrder::xyz:
		return ExposeTransform::matrixToEulerXYZ(normalizedMatrix);

	case AxisOrder::xzy:
		return ExposeTransform::matrixToEulerXZY(normalizedMatrix);

	case AxisOrder::yzx:
		return ExposeTransform::matrixToEulerYZX(normalizedMatrix);

	case AxisOrder::yxz:
		return ExposeTransform::matrixToEulerYXZ(normalizedMatrix);

	case AxisOrder::zxy:
		return ExposeTransform::matrixToEulerZXY(normalizedMatrix);

	case AxisOrder::zyx:
		return ExposeTransform::matrixToEulerZYX(normalizedMatrix);

	case AxisOrder::xyx:
		return ExposeTransform::matrixToEulerXYX(normalizedMatrix);

	case AxisOrder::yzy:
		return ExposeTransform::matrixToEulerYZY(normalizedMatrix);

	case AxisOrder::zxz:
		return ExposeTransform::matrixToEulerZXZ(normalizedMatrix);

	default:
		return ExposeTransform::matrixToEulerXYZ(normalizedMatrix);

	}

};


MVector ExposeTransform::matrixToEulerXYZ(const MMatrix& matrix)
{

	double x, y, z;

	if (matrix(0, 2) < 1.0)
	{

		if (matrix(0, 2) > -1.0)
		{

			y = asin(matrix(0, 2));
			x = atan2(-matrix(1, 2), matrix(2, 2));
			z = atan2(-matrix(0, 1), matrix(0, 0));

		}
		else
		{

			y = -M_PI / 2.0;
			x = -atan2(matrix(1, 0), matrix(1, 1));
			z = 0.0;

		}

	}
	else
	{

		y = M_PI / 2.0;
		x = atan2(matrix(1, 0), matrix(1, 1));
		z = 0.0;

	}

	return MVector(-x, -y, -z);

};


MVector ExposeTransform::matrixToEulerXZY(const MMatrix& matrix)
{

	double x, z, y;

	if (matrix(0, 1) < 1.0)
	{

		if (matrix(0, 1) > -1.0)
		{

			z = asin(-matrix(0, 1));
			x = atan2(matrix(2, 1), matrix(1, 1));
			y = atan2(matrix(0, 2), matrix(0, 0));

		}
		else
		{

			z = M_PI / 2.0;
			x = -atan2(-matrix(2, 0), matrix(2, 2));
			y = 0.0;

		}

	}
	else
	{

		z = -M_PI / 2.0;
		x = atan2(-matrix(2, 0), matrix(2, 2));
		y = 0.0;

	}

	return MVector(-x, -z, -y);

};


MVector ExposeTransform::matrixToEulerYXZ(const MMatrix& matrix)
{

	double y, x, z;

	if (matrix(1, 2) < 1.0)
	{

		if (matrix(1, 2) > -1.0)
		{

			x = asin(-matrix(1, 2));
			y = atan2(matrix(0, 2), matrix(2, 2));
			z = atan2(matrix(1, 0), matrix(1, 1));

		}
		else
		{

			x = M_PI / 2.0;
			y = -atan2(-matrix(0, 1), matrix(0, 0));
			z = 0.0;

		}

	}
	else
	{

		x = -M_PI / 2.0;
		y = atan2(-matrix(0, 1), matrix(0, 0));
		z = 0.0;

	}

	return MVector(-y, -x, -z);;

};


MVector ExposeTransform::matrixToEulerYZX(const MMatrix& matrix)
{

	double y, x, z;

	if (matrix(1, 0) < 1.0)
	{

		if (matrix(1, 0) > -1.0)
		{

			z = asin(matrix(1, 0));
			y = atan2(-matrix(2, 0), matrix(0, 0));
			x = atan2(-matrix(1, 2), matrix(1, 1));

		}
		else
		{

			z = -M_PI / 2.0;
			y = -atan2(matrix(2, 1), matrix(2, 2));
			x = 0.0;

		}

	}
	else
	{

		z = M_PI / 2.0;
		y = atan2(matrix(2, 1), matrix(2, 2));
		x = 0.0;

	}

	return MVector(-y, -z, -x);

};


MVector ExposeTransform::matrixToEulerZXY(const MMatrix& matrix)
{

	double z, x, y;

	if (matrix(2, 1) < 1.0)
	{

		if (matrix(2, 1) > -1.0)
		{

			x = asin(matrix(2, 1));
			z = atan2(-matrix(0, 1), matrix(1, 1));
			y = atan2(-matrix(2, 0), matrix(2, 2));

		}
		else
		{

			x = -M_PI / 2.0;
			z = -atan2(matrix(0, 2), matrix(0, 0));
			y = 0.0;

		}

	}
	else
	{

		x = M_PI / 2.0;
		z = atan2(matrix(0, 2), matrix(0, 0));
		y = 0.0;

	}

	return MVector(-z, -x, -y);

};


MVector ExposeTransform::matrixToEulerZYX(const MMatrix& matrix)
{

	double z, y, x;

	if (matrix(2, 0) < 1.0)
	{

		if (matrix(2, 0) > -1.0)
		{

			y = asin(-matrix(2, 0));
			z = atan2(matrix(1, 0), matrix(0, 0));
			x = atan2(matrix(2, 1), matrix(2, 2));

		}
		else
		{

			y = M_PI / 2.0;
			z = -atan2(-matrix(1, 2), matrix(1, 1));
			x = 0.0;

		}

	}
	else
	{

		y = -M_PI / 2.0;
		z = atan2(-matrix(1, 2), matrix(1, 1));
		x = 0.0;

	}

	return MVector(-z, -y, -x);

};


MVector ExposeTransform::matrixToEulerXYX(const MMatrix& matrix)
{

	double x0, y, x1;

	if (matrix(0, 0) < 1.0)
	{

		if (matrix(0, 0) > -1.0)
		{

			y = acos(matrix(0, 0));
			x0 = atan2(matrix(1, 0), -matrix(2, 0));
			x1 = atan2(matrix(0, 1), matrix(0, 2));

		}
		else
		{

			y = M_PI;
			x0 = -atan2(-matrix(1, 2), matrix(1, 1));
			x1 = 0.0;

		}

	}
	else
	{

		y = 0.0;
		x0 = atan2(-matrix(1, 2), matrix(1, 1));
		x1 = 0.0;

	}

	return MVector(x0, y, x1);

};


MVector ExposeTransform::matrixToEulerYZY(const MMatrix& matrix)
{

	double y0, z, y1;

	if (matrix(1, 1) < 1.0)
	{

		if (matrix(1, 1) > -1.0)
		{

			z = acos(matrix(1, 1));
			y0 = atan2(matrix(2, 1), -matrix(0, 1));
			y1 = atan2(matrix(1, 2), matrix(1, 0));

		}
		else
		{

			z = M_PI;
			y0 = -atan2(-matrix(2, 0), matrix(2, 2));
			y1 = 0.0;

		}

	}
	else
	{

		z = 0.0;
		y0 = atan2(-matrix(2, 0), matrix(2, 2));
		y1 = 0.0;

	}

	return MVector(y0, z, y1);

};


MVector ExposeTransform::matrixToEulerZXZ(const MMatrix& matrix)
{

	double z0, x, z1;

	if (matrix(2, 2) < 1.0)
	{

		if (matrix(2, 2) > -1.0)
		{

			x = acos(matrix(2, 2));
			z0 = atan2(matrix(0, 2), -matrix(1, 2));
			z1 = atan2(matrix(2, 0), matrix(2, 1));

		}
		else
		{

			x = M_PI;
			z0 = -atan2(-matrix(0, 1), matrix(0, 0));
			z1 = 0.0;

		}

	}
	else
	{

		x = 0.0;
		z0 = atan2(-matrix(0, 1), matrix(0, 0));
		z1 = 0.0;

	}

	return MVector(z0, x, z1);

};


void* ExposeTransform::creator() 
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: ExposeTransform
*/
{

	return new ExposeTransform();

};


MStatus ExposeTransform::initialize()
/**
This function is called by Maya after a plugin has been loaded.
Use this function to define any static attributes.

@return: MStatus
*/
{
	
	MStatus status;

	// Initialize function sets
	//
	MFnNumericAttribute fnNumericAttr;
	MFnTypedAttribute fnTypedAttr;
	MFnUnitAttribute fnUnitAttr;
	MFnEnumAttribute fnEnumAttr;
	MFnMatrixAttribute fnMatrixAttr;
	MFnCompoundAttribute fnCompoundAttr;
	MFnMessageAttribute fnMessageAttr;

	// Input attributes:
	// ".exposeNode" attribute
	//
	ExposeTransform::exposeNode = fnMessageAttr.create("exposeNode", "en", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".exposeMatrix" attribute
	//
	ExposeTransform::exposeMatrix = fnMatrixAttr.create("exposeMatrix", "em", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".localReferenceNode" attribute
	//
	ExposeTransform::localReferenceNode = fnMessageAttr.create("localReferenceNode", "lrn", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".localReferenceMatrix" attribute
	//
	ExposeTransform::localReferenceMatrix = fnMatrixAttr.create("localReferenceMatrix", "lrm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".useParent" attribute
	//
	ExposeTransform::useParent = fnNumericAttr.create("useParent", "up", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));

	// ".eulerXOrder" attribute
	//
	ExposeTransform::eulerXOrder = fnEnumAttr.create("eulerXOrder", "exo", short(1), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("xyz", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("xzy", 2));
	CHECK_MSTATUS(fnEnumAttr.addField("yzx", 3));
	CHECK_MSTATUS(fnEnumAttr.addField("yxz", 4));
	CHECK_MSTATUS(fnEnumAttr.addField("zxy", 5));
	CHECK_MSTATUS(fnEnumAttr.addField("zyx", 6));
	CHECK_MSTATUS(fnEnumAttr.addField("xyx", 7));
	CHECK_MSTATUS(fnEnumAttr.addField("yzy", 8));
	CHECK_MSTATUS(fnEnumAttr.addField("zxz", 9));

	// ".eulerYOrder" attribute
	//
	ExposeTransform::eulerYOrder = fnEnumAttr.create("eulerYOrder", "eyo", short(1), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("xyz", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("xzy", 2));
	CHECK_MSTATUS(fnEnumAttr.addField("yzx", 3));
	CHECK_MSTATUS(fnEnumAttr.addField("yxz", 4));
	CHECK_MSTATUS(fnEnumAttr.addField("zxy", 5));
	CHECK_MSTATUS(fnEnumAttr.addField("zyx", 6));
	CHECK_MSTATUS(fnEnumAttr.addField("xyx", 7));
	CHECK_MSTATUS(fnEnumAttr.addField("yzy", 8));
	CHECK_MSTATUS(fnEnumAttr.addField("zxz", 9));

	// ".eulerZOrder" attribute
	//
	ExposeTransform::eulerZOrder = fnEnumAttr.create("eulerZOrder", "ezo", short(1), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("xyz", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("xzy", 2));
	CHECK_MSTATUS(fnEnumAttr.addField("yzx", 3));
	CHECK_MSTATUS(fnEnumAttr.addField("yxz", 4));
	CHECK_MSTATUS(fnEnumAttr.addField("zxy", 5));
	CHECK_MSTATUS(fnEnumAttr.addField("zyx", 6));
	CHECK_MSTATUS(fnEnumAttr.addField("xyx", 7));
	CHECK_MSTATUS(fnEnumAttr.addField("yzy", 8));
	CHECK_MSTATUS(fnEnumAttr.addField("zxz", 9));

	// ".stripNUScale" attribute
	//
	ExposeTransform::stripNUScale = fnNumericAttr.create("stripNUScale", "snus", MFnNumericData::kBoolean, false & status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	// ".useTimeOffset" attribute
	//
	ExposeTransform::useTimeOffset = fnNumericAttr.create("useTimeOffset", "uto", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".timeOffset" attribute
	//
	ExposeTransform::timeOffset = fnUnitAttr.create("timeOffset", "to", MFnUnitAttribute::kTime, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Output attributes:
	// ".localPositionX" attribute
	//
	ExposeTransform::localPositionX = fnUnitAttr.create("localPositionX", "lpx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// ".localPositionY" attribute
	//
	ExposeTransform::localPositionY = fnUnitAttr.create("localPositionY", "lpy",  MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// ".localPositionZ" attribute
	//
	ExposeTransform::localPositionZ = fnUnitAttr.create("localPositionZ", "lpz",  MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// ".localPosition" attribute
	//
	ExposeTransform::localPosition = fnNumericAttr.create("localPosition", "lp", ExposeTransform::localPositionX, ExposeTransform::localPositionY, ExposeTransform::localPositionZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));

	CHECK_MSTATUS(fnNumericAttr.addToCategory(ExposeTransform::outputCategory));

	// ".worldPositionX" attribute
	//
	ExposeTransform::worldPositionX = fnUnitAttr.create("worldPositionX", "wpx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// ".worldPositionY" attribute
	//
	ExposeTransform::worldPositionY = fnUnitAttr.create("worldPositionY", "wpy", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// ".worldPositionZ" attribute
	//
	ExposeTransform::worldPositionZ = fnUnitAttr.create("worldPositionZ", "wpz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// ".worldPosition" attribute
	//
	ExposeTransform::worldPosition = fnNumericAttr.create("worldPosition", "wp", ExposeTransform::worldPositionX, ExposeTransform::worldPositionY, ExposeTransform::worldPositionZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));

	CHECK_MSTATUS(fnNumericAttr.addToCategory(ExposeTransform::outputCategory));

	// ".localEulerX" attribute
	//
	ExposeTransform::localEulerX = fnUnitAttr.create("localEulerX", "lex", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// ".localEulerY" attribute
	//
	ExposeTransform::localEulerY = fnUnitAttr.create("localEulerY", "ley", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// ".localEulerZ" attribute
	//
	ExposeTransform::localEulerZ = fnUnitAttr.create("localEulerZ", "lez", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// ".localEuler" attribute
	//
	ExposeTransform::localEuler = fnNumericAttr.create("localEuler", "le", ExposeTransform::localEulerX, ExposeTransform::localEulerY, ExposeTransform::localEulerZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));

	CHECK_MSTATUS(fnNumericAttr.addToCategory(ExposeTransform::outputCategory));

	// ".worldEulerX" attribute
	//
	ExposeTransform::worldEulerX = fnUnitAttr.create("worldEulerX", "wex", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// ".worldEulerY" attribute
	//
	ExposeTransform::worldEulerY = fnUnitAttr.create("worldEulerY", "wey", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// ".worldEulerZ" attribute
	//
	ExposeTransform::worldEulerZ = fnUnitAttr.create("worldEulerZ", "wez", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// ".worldEuler" attribute
	//
	ExposeTransform::worldEuler = fnNumericAttr.create("worldEuler", "we", ExposeTransform::worldEulerX, ExposeTransform::worldEulerY, ExposeTransform::worldEulerZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));

	CHECK_MSTATUS(fnNumericAttr.addToCategory(ExposeTransform::outputCategory));
	
	// ".distance" attribute
	//
	ExposeTransform::distance = fnUnitAttr.create("distance", "d",  MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));
	
	// ".angle" attribute
	//
	ExposeTransform::angle = fnUnitAttr.create("angle", "a",  MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::outputCategory));

	// Add attributes to node
	//
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::exposeNode));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::exposeMatrix));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::localReferenceNode));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::localReferenceMatrix));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::useParent));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::eulerXOrder));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::eulerYOrder));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::eulerZOrder));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::stripNUScale));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::useTimeOffset));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::timeOffset));
	
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::localPosition));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::worldPosition));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::localEuler));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::worldEuler));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::distance));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::angle));

	// Define attribute relationships
	//
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::exposeMatrix, ExposeTransform::localPositionX));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::exposeMatrix, ExposeTransform::localPositionY));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::exposeMatrix, ExposeTransform::localPositionZ));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::exposeMatrix, ExposeTransform::localEulerX));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::exposeMatrix, ExposeTransform::localEulerY));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::exposeMatrix, ExposeTransform::localEulerZ));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::exposeMatrix, ExposeTransform::distance));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::exposeMatrix, ExposeTransform::angle));

	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::localReferenceMatrix, ExposeTransform::worldPositionX));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::localReferenceMatrix, ExposeTransform::worldPositionY));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::localReferenceMatrix, ExposeTransform::worldPositionZ));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::localReferenceMatrix, ExposeTransform::worldEulerX));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::localReferenceMatrix, ExposeTransform::worldEulerY));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::localReferenceMatrix, ExposeTransform::worldEulerZ));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::localReferenceMatrix, ExposeTransform::distance));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::localReferenceMatrix, ExposeTransform::angle));

	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::eulerXOrder, ExposeTransform::localEulerX));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::eulerYOrder, ExposeTransform::localEulerY));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::eulerZOrder, ExposeTransform::localEulerZ));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::eulerXOrder, ExposeTransform::worldEulerX));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::eulerYOrder, ExposeTransform::worldEulerY));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::eulerZOrder, ExposeTransform::worldEulerZ));

	return status;

};