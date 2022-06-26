//
// File: pluginMain.cpp
//
// Author: Benjamin H. Singleton
//

#include "ExposeTransform.h"
#include <maya/MFnPlugin.h>


MStatus initializePlugin(MObject obj) 
{

	MStatus status;

	MFnPlugin plugin(obj, "Ben Singleton", "2017", "Any");
	status = plugin.registerTransform("exposeTm", ExposeTransform::id, ExposeTransform::creator, ExposeTransform::initialize, MPxTransformationMatrix::creator, MPxTransformationMatrix::baseTransformationMatrixId, &ExposeTransform::classification);
	
	if (!status) 
	{

		status.perror("registerNode");
		return status;

	}

	return status;

}

MStatus uninitializePlugin(MObject obj) 
{

	MStatus status;

	MFnPlugin plugin(obj);
	status = plugin.deregisterNode(ExposeTransform::id);

	if (!status) 
	{

		status.perror("deregisterNode");
		return status;

	}

	return status;

}
