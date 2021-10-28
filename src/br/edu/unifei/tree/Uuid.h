/*
 * Uuid.h
 *
 *  Created on: 24 de set de 2019
 *      Author: windows
 */

#ifndef INCLUDE_BR_EDU_UNIFEI_TREE_UUID_H_
#define INCLUDE_BR_EDU_UNIFEI_TREE_UUID_H_

#include <br/edu/unifei/tree/Types.h>
#include <string>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

class Uuid {
private:
    Long mostSigBits;
    Long leastSigBits;
    static string digits(ULong val, Int digits);

public:

    Uuid();

    Uuid(Long mostSigBits, Long leastSigBits) {
        assign(mostSigBits, leastSigBits);
    }

    Uuid(string value){
        fromString(value);
    }


    virtual ~Uuid() {
    }

    void assign(Long mostSigBits, Long leastSigBits) {
        this->mostSigBits = mostSigBits;
        this->leastSigBits = leastSigBits;
    }

    Int compareTo(Uuid* uuid);

    bool equals(Uuid* uuid) {
        return (this->mostSigBits == uuid->mostSigBits)
                && (this->leastSigBits == uuid->leastSigBits);
    }

    void fromString(string value);

    Long getLeastSignificantBits() {
        return this->leastSigBits;
    }

    Long getMostSignificantBits() {
        return this->mostSigBits;
    }

    string toString();

};

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* INCLUDE_BR_EDU_UNIFEI_TREE_UUID_H_ */
