/*
 * MTreePromotion.h
 *
 *  Created on: 1 de out de 2019
 *      Author: windows
 */

#ifndef BR_EDU_UNIFEI_TREE_MTREEPROMOTION_H_
#define BR_EDU_UNIFEI_TREE_MTREEPROMOTION_H_

#include <br/edu/unifei/tree/Types.h>
#include <type_traits>

#include <br/edu/unifei/tree/Metric.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

template <typename K>
class MTreePromotion {
private:
        K * firstKey;
        double firstCoveringRadius;
        Long fistSubPageId;
        K * secondKey;
        double secondCoveringRadius;
        Long secondSubPageId;

    public:

        MTreePromotion(K * firstKey, double firstCoveringRadius, Long fistSubPageId,
                K * secondKey, double secondCoveringRadius, Long secondSubPageId) :
            firstKey(firstKey), firstCoveringRadius(firstCoveringRadius), fistSubPageId(fistSubPageId),
			secondKey(secondKey), secondCoveringRadius(secondCoveringRadius), secondSubPageId(secondSubPageId){
            static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Metric class");
        }

        virtual ~MTreePromotion(){
            delete this->firstKey;
            delete this->secondKey;
        }

        K * getFirstKey() {
            return this->firstKey;
        }

        double getFirstCoveringRadius() {
            return firstCoveringRadius;
        }

        Long getFistSubPageId() {
            return this->fistSubPageId;
        }

        K * getSecondKey() {
            return this->secondKey;
        }

        double getSecondCoveringRadius() {
            return secondCoveringRadius;
        }

        Long getSecondSubPageId() {
            return this->secondSubPageId;
        }

        void setFirstCoveringRadius(double firstCoveringRadius) {
            this->firstCoveringRadius = firstCoveringRadius;
        }

        void setSecondCoveringRadius(double secondCoveringRadius) {
            this->secondCoveringRadius = secondCoveringRadius;
        }
};

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_MTREEPROMOTION_H_ */
