/*
 * EuclideanGeometry.h
 *
 *  Created on: 24 de set de 2019
 * @author Enzo Seraphim <seraphim@unifei.edu.br>
 * @author Thatyana de Faria Piola Seraphim <thatyana@unifei.edu.br>
 * @author Carlos Eduardo dos Santos Pereira <eduardocarlos@unifei.edu.br>
 * @author Leandro Moraes Barbosa <lmoraesbarbosa@unifei.edu.br> */

#ifndef BR_EDU_UNIFEI_TREE_EUCLIDEANGEOMETRY_H_
#define BR_EDU_UNIFEI_TREE_EUCLIDEANGEOMETRY_H_

#include <br/edu/unifei/tree/Types.h>

#include <br/edu/unifei/tree/Rectangle.h>
#include <type_traits>
#include <cstddef>
#include <algorithm>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

template <typename K>
class EuclideanGeometry {
private:
    Long calculatedOverlap;
    const Double precisionError; //incrementa nona casa

public:
	EuclideanGeometry() : calculatedOverlap(0), precisionError(0.00000001){
		static_assert(is_base_of<Rectangle<K>, K>::value, "Generic type is not derived of the Rectangle class");
	}

	virtual ~EuclideanGeometry(){
	}

	K* unionRectangles(K * rect1, K * rect2);

	Double occupancy(K * rect);

	Bool isOverlap(K * rectOverlap, K * rectOverlaped);

	Long getCalculatedOverlap() {
        return calculatedOverlap;
    }

    void resetCalculatedOverlap() {
        calculatedOverlap = 0;
    }

};

//----------------------------------------------------------------------

template <typename K>
K * EuclideanGeometry<K>::unionRectangles(K * rect1, K * rect2) {
    Int dims = rect1->numberOfDimensions();
    Double minPoint[] = new Double[dims];
    Double maxPoint[] = new Double[dims];
    Double coord1;
    Double coord2;
    K mbrUnion = new K();
    calculatedOverlap++;

    for (int i = 0; i < dims; i++) {
        coord1 = rect1->getOrigin(i);
        coord2 = rect2->getOrigin(i);
        minPoint[i] = std::min(coord1, coord2);
        maxPoint[i] = std::max(coord1 + rect1->getExtension(i), coord2 + rect2->getExtension(i));
        mbrUnion.setOrigin(i, minPoint[i]);
        mbrUnion.setExtension(i, maxPoint[i] - minPoint[i] + this->precisionError);
    }
    return mbrUnion;
}

//----------------------------------------------------------------------

template <typename K>
Double EuclideanGeometry<K>::occupancy(K * rect) {
    Double ocup = 1;
    Int dims = rect->numberOfDimensions();

    for (int i = 0; i < dims; i++) {
        ocup *= rect->getExtension(i);
    }

    return ocup;
}

//----------------------------------------------------------------------

template <typename K>
Bool EuclideanGeometry<K>::isOverlap(K * rectOverlap, K * rectOverlaped) {
    int dims = rectOverlap->numberOfDimensions();
    calculatedOverlap++;
    for (int i = 0; i < dims; i++) {
        if (rectOverlap->getOrigin(i) >
                rectOverlaped->getOrigin(i) + rectOverlaped->getExtension(i) ) {
            return false;
        }
        if (rectOverlap->getOrigin(i) + rectOverlap->getExtension(i) <
                rectOverlaped->getOrigin(i)) {
            return false;
        }
    }

    return true;
}

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_EUCLIDEANGEOMETRY_H_ */
