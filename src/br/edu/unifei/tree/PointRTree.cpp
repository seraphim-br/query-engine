/*
 * PointRTree.cpp
 *
 *  Created on: 25 de set de 2019
 *      Author: windows
 */

#include <br/edu/unifei/tree/PointRTree.h>
#include <br/edu/unifei/tree/PullStream.h>
#include <br/edu/unifei/tree/PushStream.h>

#include <cmath>

#define _USE_MATH_DEFINES

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
	Bool isOverlaped = true;
	for (int i = 0; i < 2; i++) {
		if (this->getOrigin(i) >
				obj->getOrigin(i) + obj->getExtension(i) ) {
			isOverlaped = false;
			break;
		}
		if (this->getOrigin(i) + this->getExtension(i) <
				obj->getOrigin(i)) {
			isOverlaped = false;
			break;
		}
	}
	if (isOverlaped == true) {
		return 0;
	}
	//TODO: Double distanciaA = minima distancia entre todos os vertices do retangulo this para cada reta do retangulo obj
	//TODO: Double distanciaB = minima distancia entre todos os vertices do retangulo obj para cada reta do retangulo this
    //TODO: return menor distancia entre A e B
	return 0;
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
