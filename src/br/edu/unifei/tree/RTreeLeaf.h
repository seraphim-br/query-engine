/*
 * RTreeLeaf.h
 *
 *  Created on: 25 de set de 2019
 *      Author: windows
 */
/**
 * <p>This class implements an R-Tree Leaf.</p>
 *
 * @param <K>
 *
 * <blockquote><pre>
 * {@code
 * Design:
 *
 * +-------------------+--------+--------------------------+------------------+--------------+
 * |node|par-|prev|next| number |ent uuid|ent uuid|ent uuid|                  |key2|key1|key0|
 * |type|-ent|page|page|   of   |--------+--------+--------|                  |    |    |    |
 * |    |node| Id | Id |  keys  |entry[0]|entry[1]|entry[2]|<-- free space -->|    |    |    |
 * |-------------------+--------|--------+--------+--------+                  |--------------|
 * |      header       |features|         entries          |                  |     keys     |
 * +-------------------+--------+--------------------------+------------------+--------------+
 * }
 * </pre></blockquote>
 *
 * @author Enzo Seraphim <seraphim@unifei.edu.br>
 * @author Thatyana de Faria Piola Seraphim <thatyana@unifei.edu.br>
 * @author Carlos Eduardo dos Santos Pereira <eduardocarlos@unifei.edu.br>
 * @author Leandro Moraes Barbosa <lmoraesbarbosa@unifei.edu.br>
 */
#ifndef BR_EDU_UNIFEI_TREE_RTREELEAF_H_
#define BR_EDU_UNIFEI_TREE_RTREELEAF_H_

#include <br/edu/unifei/tree/Node.h>
#include <br/edu/unifei/tree/Types.h>

#include <br/edu/unifei/tree/RTreeNode.h>
#include <type_traits>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

template <typename K>
class RTreeLeaf : public RTreeNode<K> {
protected:

    Int getNodeType() {
        return nodeType;
    }//getNodeType

    Int sizeOfEntry() {
        return Node::sizeOfUuid + Node::sizeOfDouble + Node::sizeOfInt;
    }//sizeOfEntry

    void writeEntityUuid(Int idx, Uuid * uuid);
public:
    static const Int nodeType = 8;

    RTreeLeaf(Node * node) : RTreeNode<K>(node) {
        static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Order class");
        this->initialize(node);
    }

    virtual ~RTreeLeaf() {
    }

    Bool addKey(K * key, Uuid * uuid);

    static Bool matchNodeType(Node * node) {
        return node->readNodeType() == RTreeLeaf<K>::nodeType;
    }//matchNodeType

    Uuid * readEntityUuid(Int idx);

};

//----------------------------------------------------------------------

template <typename K>
Bool RTreeLeaf<K>::addKey(K * key, Uuid * uuid) {
    Int total = this->readNumberOfKeys();

    Int size = key->sizeOfKey();

    if (size + this->sizeOfEntry() > this->freeSpace(size)) // It there is not space.
    {
        return false;
    } else {
        Int off = this->getOffset(total, size);
        key->pushKey(this->getArray(), off); // Adding
        this->writeOffset(total, off);
        this->writeEntityUuid(total, uuid);
        this->incrementNumberOfKeys();

        return true;
    }
}//addKey

//----------------------------------------------------------------------

template <typename K>
Uuid * RTreeLeaf<K>::readEntityUuid(Int idx) {
    Int pos = Node::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    return this->readUuid(pos);
}//readEntityUuid

//----------------------------------------------------------------------

template <typename K>
void RTreeLeaf<K>::writeEntityUuid(Int idx, Uuid * uuid) {
    Int pos = Node::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    this->writeUuid(pos, uuid);
}//writeEntityUuid

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_RTREELEAF_H_ */
