/*
 * MTreeLeaf.h
 *
 *  Created on: 25 de set de 2019
 *      Author: windows
 */

#ifndef BR_EDU_UNIFEI_TREE_MTREELEAF_H_
#define BR_EDU_UNIFEI_TREE_MTREELEAF_H_

#include <br/edu/unifei/tree/Node.h>
#include <br/edu/unifei/tree/Types.h>

#include <br/edu/unifei/tree/MTreeNode.h>
#include <type_traits>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

template <typename K>
class MTreeLeaf : public MTreeNode<K> {
protected:

    Int getNodeType() {
        return nodeType;
    }//getNodeType

    Int sizeOfEntry() {
        return Node::sizeOfUuid + Node::sizeOfDouble + Node::sizeOfInt;
    }//sizeOfEntry

    void writeEntityUuid(Int idx, Uuid * uuid);
public:
    static const Int nodeType = 6;

    MTreeLeaf(Node * node) : MTreeNode<K>(node) {
        static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Order class");
        this->initialize(node);
    }

    virtual ~MTreeLeaf() {
    }

    Bool addKey(K * key, Double dist, Uuid * uuid);

    static Bool matchNodeType(Node * node) {
        return node->readNodeType() == MTreeLeaf<K>::nodeType;
    }//matchNodeType

    Uuid * readEntityUuid(Int idx);

    void remove(Int idx);

};

//----------------------------------------------------------------------

template <typename K>
Bool MTreeLeaf<K>::addKey(K * key, Double dist, Uuid * uuid) {
    Int total = this->readNumberOfKeys();

    Int size = key->sizeOfKey();

    if (size + this->sizeOfEntry() > this->freeSpace()) // It there is not space.
    {
        return false;
    } else {
        Int off = total == 0 ? this->sizeOfArray() - size : this->readOffset(total - 1) - size;
        key->pushKey(this->getArray(), off); // Adding
        this->writeOffset(total, off);
        this->writeDistanceToParent(total, dist);
        this->writeEntityUuid(total, uuid);
        this->incrementNumberOfKeys();

        return true;
    }
}//addKey

//----------------------------------------------------------------------

template <typename K>
Uuid * MTreeLeaf<K>::readEntityUuid(Int idx) {
    Int pos = Node::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    return this->readUuid(pos);
}//readEntityUuid

//----------------------------------------------------------------------

template <typename K>
void MTreeLeaf<K>::remove(Int idx) {
    Int sizeKey = idx == 0
            ? this->sizeOfArray() - this->readOffset(0)
            : this->readOffset(idx - 1) - this->readOffset(idx);
    Int total = this->readNumberOfKeys();
    Int length = this->readOffset(idx) - this->readOffset(total - 1);
    Int source = this->readOffset(total - 1);
    Int dest = source + sizeKey;

    this->move(dest, source, length);

    for (Int k = idx; k < total - 1; k++) {
        this->writeOffset(k, this->readOffset(k + 1) + sizeKey);
        this->writeDistanceToParent(k, this->readDistanceToParent(k + 1));
        this->writeEntityUuid(k, this->readEntityUuid(k + 1));
    }
    this->decrementNumberOfKeys();
}

//----------------------------------------------------------------------

template <typename K>
void MTreeLeaf<K>::writeEntityUuid(Int idx, Uuid * uuid) {
    Int pos = Node::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    this->writeUuid(pos, uuid);
}//writeEntityUuid

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_MTREELEAF_H_ */
