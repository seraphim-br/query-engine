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

template <typename K>
class Rectangle : public Metric<K> {
public:
	Rectangle() {
		static_assert(is_base_of<Rectangle<K>, K>::value, "Generic type is not derived of the Rectangle class");
	}

	template <typename U>
	operator Rectangle<U>* () const {
		return NULL;
	}

	virtual ~Rectangle() = 0;
	virtual Double getExtension(Int axis) = 0;
	virtual Double getOrigin(Int axis) = 0;
	virtual Int numberOfDimensions() = 0;
	virtual void setOrigin(Int axis, Double value) = 0;
	virtual void setExtension(Int axis, Double value) = 0;
	virtual Uuid* getUuid() = 0;
	virtual void setUuid(Uuid *uuid) = 0;
};

template <typename K>
inline Rectangle<K>::~Rectangle() {

}
} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_RECTANGLE_H_ */
