/*
 * Rectangle.h
 *
 *  Created on: 24 de set de 2019
 * @author Enzo Seraphim <seraphim@unifei.edu.br>
 * @author Thatyana de Faria Piola Seraphim <thatyana@unifei.edu.br>
 * @author Carlos Eduardo dos Santos Pereira <eduardocarlos@unifei.edu.br>
 * @author Leandro Moraes Barbosa <lmoraesbarbosa@unifei.edu.br> */

#ifndef BR_EDU_UNIFEI_TREE_RECTANGLE_H_
#define BR_EDU_UNIFEI_TREE_RECTANGLE_H_

#include <br/edu/unifei/tree/Types.h>

#include <br/edu/unifei/tree/Metric.h>
#include <br/edu/unifei/tree/Uuid.h>
#include <type_traits>
#include <cstddef>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

class Rectangle : public Metric<Rectangle> {
private:
	Uuid * uuid;
	Float extensionLatitude;
	Float extensionLongitude;
	Float latitude;
	Float longitude;
	Double preservedDistance;

public:
	Rectangle();

	virtual ~Rectangle();

    Double getExtension(Int axis) {
    	if (axis == 0) {
    		return extensionLatitude;
    	} else {
    		return extensionLongitude;
    	}
    }
    Double getOrigin(Int axis) {
    	if (axis == 0) {
    		return latitude;
    	} else {
    		return longitude;
    	}
    }
    Int numberOfDimensions() {
    	return 2;
    }
    void setExtension(Int axis, Double value) {
    	if (axis == 0) {
    		extensionLatitude = value;
    	} else {
    		extensionLongitude = value;
    	}
    }
    void setOrigin(Int axis, Double value) {
    	if (axis == 0) {
    		latitude = value;
    	} else {
    		longitude = value;
    	}
    }

    Double distanceTo(Rectangle *obj);

	Bool pullKey(Byte array[], Int position);

	void pushKey(Byte array[], Int position);

	Int sizeOfKey();
};
} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_RECTANGLE_H_ */
