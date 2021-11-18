/*
 * RTreePromotion.h
 *
 *  Created on: 1 de out de 2019
 *      Author: windows
 */

#ifndef BR_EDU_UNIFEI_TREE_RTREEPROMOTION_H_
#define BR_EDU_UNIFEI_TREE_RTREEPROMOTION_H_

#include <br/edu/unifei/tree/Types.h>
#include <type_traits>

#include <br/edu/unifei/tree/Rectangle.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

template <typename K>
class RTreePromotion {
private:
        K * firstKey;
        Long fistSubPageId;
        K * secondKey;
        Long secondSubPageId;

    public:

        RTreePromotion(K * firstKey, Long fistSubPageId,
                K * secondKey, Long secondSubPageId) :
            firstKey(firstKey), fistSubPageId(fistSubPageId),
			secondKey(secondKey), secondSubPageId(secondSubPageId){
            static_assert(is_base_of<Rectangle<K>, K>::value, "Generic type is not derived of the Rectangle class");
        }

        virtual ~RTreePromotion(){
            delete this->firstKey;
            delete this->secondKey;
        }

        K * getFirstKey() {
            return this->firstKey;
        }

        Long getFistSubPageId() {
            return this->fistSubPageId;
        }

        K * getSecondKey() {
            return this->secondKey;
        }

        Long getSecondSubPageId() {
            return this->secondSubPageId;
        }

        void setSecondKey(K *secondKey) {
			this->secondKey = secondKey;
		}


        void setFirstKey(K *firstKey) {
			this->firstKey = firstKey;
		}
};

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_RTREEPROMOTION_H_ */
