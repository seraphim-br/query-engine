/*
 * Point.h
 *
 *  Created on: 25 de set de 2019
 *      Author: windows
 */

#ifndef INCLUDE_BR_EDU_UNIFEI_TREE_POINT_H_
#define INCLUDE_BR_EDU_UNIFEI_TREE_POINT_H_

#include <br/edu/unifei/tree/Types.h>
#include <br/edu/unifei/tree/Uuid.h>

#include <br/edu/unifei/tree/Metric.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

# define M_PI           3.14159265358979323846

class Point: public Metric<Point> {
private:
	Uuid * uuid;
	Float latitude;
	Float longitude;
	Double preservedDistance;
public:
	Point();
	virtual ~Point();

	Float getLatitude() const {
		return latitude;
	}

	void setLatitude(Float latitude) {
		this->latitude = latitude;
	}

	Float getLongitude() const {
		return longitude;
	}

	void setLongitude(Float longitude) {
		this->longitude = longitude;
	}

	Uuid* getUuid() const {
		return uuid;
	}

	void setUuid(Uuid *uuid) {
		this->uuid = uuid;
	}

	Double getPreservedDistance() {
		return preservedDistance;
	}

	void setPreservedDistance(Double preservedDistance) {
		this->preservedDistance = preservedDistance;
	}

    Double distanceTo(Point *obj);

	Bool pullKey(Byte array[], Int position);

	void pushKey(Byte array[], Int position);

	Int sizeOfKey();

};

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* INCLUDE_BR_EDU_UNIFEI_TREE_POINT_H_ */
