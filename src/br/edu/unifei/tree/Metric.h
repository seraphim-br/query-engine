/*
 * Metric.h
 *
 *  Created on: 24 de set de 2019
 *      Author: windows
 */

#ifndef BR_EDU_UNIFEI_TREE_METRIC_H_
#define BR_EDU_UNIFEI_TREE_METRIC_H_

#include <br/edu/unifei/tree/Types.h>
#include <type_traits>
#include <cstddef>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

template <typename K>
class Metric {
public:
	Metric() {
		static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Metric class");
	}

	template <typename U>
	operator Metric<U>* () const {
		return NULL;
	}

	virtual ~Metric() = 0;
	virtual Bool pullKey(Byte array[], Int position) = 0;
	virtual void pushKey(Byte array[], Int position) = 0;
    virtual Int sizeOfKey() = 0;
    virtual Double distanceTo(K *obj) = 0;
    virtual Double getPreservedDistance() = 0;
    virtual void setPreservedDistance(Double distance) = 0;
};

template <typename K>
inline Metric<K>::~Metric() {

}

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_METRIC_H_ */
