/*
 * AbstractNode.h
 *
 *  Created on: 25 de set de 2019
 *      Author: windows
 */

#ifndef INCLUDE_BR_EDU_UNIFEI_TREE_ABSTRACTNODE_H_
#define INCLUDE_BR_EDU_UNIFEI_TREE_ABSTRACTNODE_H_

#include <br/edu/unifei/tree/Node.h>

#include <br/edu/unifei/tree/Types.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

class AbstractNode: public Node {
protected:

    virtual Int getNodeType() = 0;

    void initialize(Node *node);

public:

    AbstractNode(Node *node) : Node(node->getPageId(), node->getArray(), node->getSize()) {
    }

    virtual ~AbstractNode() = 0;

    virtual void clear() = 0;

};

inline AbstractNode::~AbstractNode() {
}

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* INCLUDE_BR_EDU_UNIFEI_TREE_ABSTRACTNODE_H_ */
