/*
 * PointRTree.h
 *
 *  Created on: 24 de set de 2019
 * @author Enzo Seraphim <seraphim@unifei.edu.br>
 * @author Thatyana de Faria Piola Seraphim <thatyana@unifei.edu.br>
 * @author Carlos Eduardo dos Santos Pereira <eduardocarlos@unifei.edu.br>
 * @author Leandro Moraes Barbosa <lmoraesbarbosa@unifei.edu.br> */

#ifndef BR_EDU_UNIFEI_TREE_POINTRTREE_H_
#define BR_EDU_UNIFEI_TREE_POINTRTREE_H_

#include <br/edu/unifei/tree/Types.h>

#include <br/edu/unifei/tree/Rectangle.h>
#include <br/edu/unifei/tree/Uuid.h>
#include <type_traits>
#include <cstddef>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

class PointRTree : public Rectangle<PointRTree> {
private:
	Uuid * uuid;
	Float extensionLatitude;
	Float extensionLongitude;
	Float latitude;
	Float longitude;
	Double preservedDistance;

public:
	PointRTree();

	virtual ~PointRTree();

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

    Double distanceTo(PointRTree *obj);

	Bool pullKey(Byte array[], Int position);

	void pushKey(Byte array[], Int position);

	Double getPreservedDistance() {
		return preservedDistance;
	}

	void setPreservedDistance(Double preservedDistance) {
		this->preservedDistance = preservedDistance;
	}

	Uuid* getUuid() {
		return uuid;
	}

	void setUuid(Uuid *uuid) {
		this->uuid = uuid;
	}

	Int sizeOfKey();
};
} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_POINTRTREE_H_ */
