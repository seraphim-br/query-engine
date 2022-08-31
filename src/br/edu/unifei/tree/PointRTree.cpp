/*
 * PointRTree.cpp
 *
 *  Created on: 25 de set de 2019
 *      Author: windows
 */

#include <br/edu/unifei/tree/PointRTree.h>
#include <br/edu/unifei/tree/PullStream.h>
#include <br/edu/unifei/tree/PushStream.h>
#include <br/edu/unifei/tree/Types.h>

#include <cmath>
#include <cfloat>

#define _USE_MATH_DEFINES

# define M_PI           3.14159265358979323846

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;


PointRTree::PointRTree() {
	// TODO Auto-generated constructor stub
	this->latitude = 0;
	this->longitude = 0;
	this->extensionLatitude = 0;
	this->extensionLongitude = 0;
	this->preservedDistance = 0;
	this->uuid = new Uuid();
}
//----------------------------------------------------------------------

PointRTree::~PointRTree() {
	// TODO Auto-generated destructor stub
	delete uuid;
}
//----------------------------------------------------------------------

Double PointRTree::distanceTo(PointRTree *obj){
//	if (geometry->isOverlap(this, obj)) {
//		return 0;
//	}
//
//	// vertices retangulo this
//	Double vThis[4][2];
//	vThis[0][0] = this->getOrigin(0);
//	vThis[0][1] = this->getOrigin(1);
//
//	vThis[1][0] = this->getOrigin(0) + this->getExtension(0);
//	vThis[1][1] = this->getOrigin(1);
//
//	vThis[2][0] = this->getOrigin(0) + this->getExtension(0);
//	vThis[2][1] = this->getOrigin(1) + this->getExtension(1);
//
//	vThis[3][0] = this->getOrigin(0);
//	vThis[3][1] = this->getOrigin(1) + this->getExtension(1);
//
//	// vertices retangulo obj
//	Double vObj[4][2];
//	vObj[0][0] = obj->getOrigin(0);
//	vObj[0][1] = obj->getOrigin(1);
//
//	vObj[1][0] = obj->getOrigin(0) + obj->getExtension(0);
//	vObj[1][1] = obj->getOrigin(1);
//
//	vObj[2][0] = obj->getOrigin(0) + obj->getExtension(0);
//	vObj[2][1] = obj->getOrigin(1) + obj->getExtension(1);
//
//	vObj[3][0] = obj->getOrigin(0);
//	vObj[3][1] = obj->getOrigin(1) + obj->getExtension(1);
//
//	Double xo, yo, xa, ya, distAux;
//
//	Double dist = 0;
//	for (int i = 0; i < 4; i++) {
//		xo = vThis[i][0];
//		yo = vThis[i][1];
//
//		for (int j = 0; j < 4; j++){
//			xa = vObj[j][0];
//			ya = vObj[j][1];
//
//			distAux = sqrt(pow(xo - xa, 2) + pow(yo - ya, 2));
//			if (distAux > dist) {
//				dist = distAux;
//			}
//		}
//	}
//	return dist;
	double lat1 = this->getOrigin(0) * M_PI / 180;
	double lat2 = obj->getOrigin(1) * M_PI / 180;
	double lon1 = this->getOrigin(0) * M_PI / 180;
	double lon2 = obj->getOrigin(1) * M_PI / 180;
	if (lat1 == lat2 && lon1 == lon2) {
		return 0;
	}
	return acos(
			(sin(lat1) * sin(lat2)
			+ cos(lat1) * cos(lat2)) * cos(abs(lon2 - lon1))) * 6378;
}

//----------------------------------------------------------------------
Bool PointRTree::pullKey(Byte array[], Int position){
	PullStream pull(array, position);
	this->latitude = pull.pullFloat();
	this->longitude = pull.pullFloat();
	this->extensionLatitude = pull.pullFloat();
	this->extensionLongitude = pull.pullFloat();
	return true;
}
//----------------------------------------------------------------------

void PointRTree::pushKey(Byte array[], Int position){
	PushStream push(array, position);
	push.pushFloat(this->latitude);
	push.pushFloat(this->longitude);
	push.pushFloat(this->extensionLatitude);
	push.pushFloat(this->extensionLongitude);
}
//----------------------------------------------------------------------

Int PointRTree::sizeOfKey(){
	return Stream::sizeOfFloat + Stream::sizeOfFloat + Stream::sizeOfFloat + Stream::sizeOfFloat;
}


} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */
