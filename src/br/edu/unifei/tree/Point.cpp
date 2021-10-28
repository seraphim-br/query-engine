/*
 * Point.cpp
 *
 *  Created on: 25 de set de 2019
 *      Author: windows
 */

#include <br/edu/unifei/tree/Point.h>
#include <br/edu/unifei/tree/PullStream.h>
#include <br/edu/unifei/tree/PushStream.h>

#include <cmath>

#define _USE_MATH_DEFINES

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;


Point::Point() {
	// TODO Auto-generated constructor stub
	this->latitude = 0;
	this->longitude = 0;
	this->preservedDistance = 0;
	this->uuid = new Uuid();
}
//----------------------------------------------------------------------

Point::~Point() {
	// TODO Auto-generated destructor stub
	delete uuid;
}
//----------------------------------------------------------------------

Double Point::distanceTo(Point *obj){
    double lat1 = this->getLatitude() * M_PI / 180;
    double lat2 = obj->getLatitude() * M_PI / 180;
    double lon1 = this->getLongitude() * M_PI / 180;
    double lon2 = obj->getLongitude() * M_PI / 180;
    if (lat1 == lat2 && lon1 == lon2) {
        return 0;
    }
    return acos(
            (sin(lat1) * sin(lat2)
            + cos(lat1) * cos(lat2)) * cos(abs(lon2 - lon1))) * 6378;
//    return sqrt(pow(this->getLatitude() - obj->getLatitude(), 2.0) +
//			pow(this->getLongitude() - obj->getLongitude(), 2.0));
}

//----------------------------------------------------------------------
Bool Point::pullKey(Byte array[], Int position){
	PullStream pull(array, position);
	this->latitude = pull.pullFloat();
	this->longitude = pull.pullFloat();
	return true;
}
//----------------------------------------------------------------------

void Point::pushKey(Byte array[], Int position){
	PushStream push(array, position);
	push.pushFloat(this->latitude);
	push.pushFloat(this->longitude);
}
//----------------------------------------------------------------------

Int Point::sizeOfKey(){
	return Stream::sizeOfFloat + Stream::sizeOfFloat;
}


} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */
