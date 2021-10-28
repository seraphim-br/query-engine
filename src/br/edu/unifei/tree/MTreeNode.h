/*
 * MTreeNode.h
 *
 *  Created on: 24 de set de 2019
 *      Author: windows
 */

#ifndef BR_EDU_UNIFEI_TREE_MTREENODE_H_
#define BR_EDU_UNIFEI_TREE_MTREENODE_H_

#include <br/edu/unifei/tree/AbstractNode.h>
#include <br/edu/unifei/tree/Node.h>
#include <br/edu/unifei/tree/Types.h>

#include <br/edu/unifei/tree/Metric.h>
#include <type_traits>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

template <typename K>
class MTreeNode: public AbstractNode {
protected:
    Long calculatedDistance;

    void decrementNumberOfKeys();

    Int freeSpace();

    void incrementNumberOfKeys();

    Int readOffset(Int idx);

    Int sizeOfFeatures() {
        return Node::sizeOfInt;
    }

    void writeOffset(Int idx, Int off);

    virtual Int sizeOfEntry() = 0;


public:

    MTreeNode(Node * node) : AbstractNode(node) {
        static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Order class");
        calculatedDistance = 0;
    }

    virtual ~MTreeNode(){

    }

    Long getCalculatedDistance() {
        return calculatedDistance;
    }

    K * buildKey(Int idx);

    void clear();

    Int indexOfKey(K * key);

    Int readNumberOfKeys();

    Double readDistanceToParent(Int idx);

    Bool replace(Int idx, K * replaceKey);

    void writeDistanceToParent(Int idx, Double dist);

    virtual void remove(Int idx) = 0;

};


template <typename K>
K * MTreeNode<K>::buildKey(Int idx) {
    K * key;
    if ((idx >= 0) && (idx < this->readNumberOfKeys())) {
        key = new K();
        //pull the key of the page
        key->pullKey(this->getArray(), this->readOffset(idx));
        return key;
    }//endif
    return NULL;
}//buildKey

//----------------------------------------------------------------------

template <typename K>
void MTreeNode<K>::clear() {
    //zering numberOfKeys
    Int pos = Node::sizeOfHeader();
    this->writeInt(pos, 0);
}//clear

//----------------------------------------------------------------------

template <typename K>
void MTreeNode<K>::decrementNumberOfKeys() {
    Int num = this->readNumberOfKeys() - 1;
    Int pos = Node::sizeOfHeader();
    this->writeInt(pos, num);
}//decrementNumberOfKey

//----------------------------------------------------------------------

template <typename K>
Int MTreeNode<K>::freeSpace() {
    Int num = this->readNumberOfKeys();
    Int free = 0;
    if (num == 0) {
        free = this->sizeOfArray() - Node::sizeOfHeader() - this->sizeOfFeatures();
    } else {
        free = this->sizeOfArray() - Node::sizeOfHeader()
                - this->sizeOfFeatures() - (num * this->sizeOfEntry())
                - (this->sizeOfArray() - this->readOffset(num - 1));
    }//endif
    return free;
}//freeSpace

//----------------------------------------------------------------------

template <typename K>
Int MTreeNode<K>::indexOfKey(K * key) {
    K * objKey;
	Int total = this->readNumberOfKeys();

	for (Int i = 0; i < total; i++)
	{
	    objKey = this->buildKey(i);
	    if (objKey->distanceTo(key) == 0)
	    {
		return i;
	    }
	}
        delete objKey;

	return -1;
}//indexOfKey

//----------------------------------------------------------------------

template <typename K>
void MTreeNode<K>::incrementNumberOfKeys() {
    Int num = this->readNumberOfKeys() + 1;
    Int pos = Node::sizeOfHeader();
    this->writeInt(pos, num);
}//incrementNumberOfKey

//----------------------------------------------------------------------

template <typename K>
Double MTreeNode<K>::readDistanceToParent(Int idx) {
    Int pos = Node::sizeOfHeader()
            + this->sizeOfFeatures()
            + ((idx + 1) * this->sizeOfEntry())
            - Node::sizeOfInt // off
            - Node::sizeOfDouble; // parent distance

    return this->readDouble(pos);
}//readDistanceToParent

//----------------------------------------------------------------------

template <typename K>
Int MTreeNode<K>::readNumberOfKeys() {
    Int pos = Node::sizeOfHeader();
    return this->readInt(pos);
}//readNumberOfKeys

//----------------------------------------------------------------------

template <typename K>
Int MTreeNode<K>::readOffset(Int idx) {
    Int pos = Node::sizeOfHeader() + this->sizeOfFeatures() + ((idx + 1) * this->sizeOfEntry()) - Node::sizeOfInt;
    return this->readInt(pos);
}//readOffset

//----------------------------------------------------------------------

template <typename K>
Bool MTreeNode<K>::replace(Int idx, K * replaceKey) {
    Int total = this->readNumberOfKeys();
    Int offLastKey = this->readOffset(total - 1);
    Int length = this->readOffset(idx) - offLastKey;
    Int sizeOldKey = idx == 0 ? this->sizeOfArray() - this->readOffset(0) : this->readOffset(idx - 1) - this->readOffset(idx);
    Int offSize = sizeOldKey - replaceKey->sizeOfKey();
    if (replaceKey->sizeOfKey() <= this->freeSpace() + sizeOldKey) {
        this->move(offLastKey + offSize, offLastKey, length);
        for (Int i = idx; i <= total - 1; i++) {
            this->writeOffset(i, this->readOffset(i) + offSize);
        }
        replaceKey->pushKey(this->getArray(), this->readOffset(idx));
        return true;
    } else {
        return false;
    }
}//replace

//----------------------------------------------------------------------

template <typename K>
void MTreeNode<K>::writeDistanceToParent(Int idx, Double dist) {
    Int pos = Node::sizeOfHeader()
            + this->sizeOfFeatures()
            + ((idx + 1) * this->sizeOfEntry())
            - Node::sizeOfInt
            - Node::sizeOfDouble;
    this->writeDouble(pos, dist);
}//writeDistanceToParent

//----------------------------------------------------------------------

template <typename K>
void MTreeNode<K>::writeOffset(Int idx, Int off) {
    Int pos = Node::sizeOfHeader() + this->sizeOfFeatures() + ((idx + 1) * this->sizeOfEntry()) - Node::sizeOfInt;
    this->writeInt(pos, off);
}//writeOffset

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_MTREENODE_H_ */
