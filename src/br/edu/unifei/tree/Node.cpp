/*
 * Node.cpp
 *
 *  Created on: 24 de set de 2019
 *      Author: windows
 */

#include <br/edu/unifei/tree/Node.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

void Node::moveLeft(Int dest, Int source, Int length) {
    for (Int i = 0; i < length; i++) {
        array[dest + i] = array[source + i];
    }
}

//----------------------------------------------------------------------
void Node::moveRight(Int dest, Int source, Int length) {
    for (Int i = 0; i < length; i++) {
        array[dest + length - i - 1] = array[source + length - i - 1];
    }
}

//----------------------------------------------------------------------
void Node::writeUuid(Int pos, Uuid *value) {
    this->writeLong(pos, value->getMostSignificantBits());
    this->writeLong(pos + sizeOfLong, value->getLeastSignificantBits());
}

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */
