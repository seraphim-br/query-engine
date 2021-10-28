/*
 * AbstractNode.cpp
 *
 *  Created on: 25 de set de 2019
 *      Author: windows
 */

#include <br/edu/unifei/tree/AbstractNode.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

void AbstractNode::initialize(Node *node) {
    Int type = node->readNodeType();

    // zero indica que não foi definido o tipo do nó
    if (type == 0) {
        this->writeNodeType(this->getNodeType());
    } else if (type != this->getNodeType()) {
        // TODO exceptionize-it
        // error match type node
        throw new exception();
    }
}

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */
