/*
 * RTreeNode.h
 *
 *  Created on: 24 de set de 2019
 *      Author: windows
 */
/**
 * <p>This class implements an R-Tree Node.</p>
 *
 * @param <K>
 *
 * <blockquote><pre>
 * {@code
 * Design:
 *
 * +-------------------+--------+----------------------------------------+
 * |node|par-|prev|next| number |                                        |
 * |type|-ent|page|page|   of   |                                        |
 * |    |node| Id | Id |  keys  |<------------- free space ------------->|
 * |-------------------+--------|                                        |
 * |      header       |features|                                        |
 * +-------------------+--------+----------------------------------------+
 * }
 * </pre></blockquote>
 *
 * @author Enzo Seraphim <seraphim@unifei.edu.br>
 * @author Thatyana de Faria Piola Seraphim <thatyana@unifei.edu.br>
 * @author Carlos Eduardo dos Santos Pereira <eduardocarlos@unifei.edu.br>
 * @author Leandro Moraes Barbosa <lmoraesbarbosa@unifei.edu.br>
 */
#ifndef BR_EDU_UNIFEI_TREE_RTREENODE_H_
#define BR_EDU_UNIFEI_TREE_RTREENODE_H_

#include <br/edu/unifei/tree/RTreeNode.h>

#include <br/edu/unifei/tree/AbstractNode.h>
#include <br/edu/unifei/tree/Node.h>
#include <br/edu/unifei/tree/Types.h>

#include <br/edu/unifei/tree/Rectangle.h>
#include <type_traits>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

template <typename K>
class RTreeNode: public AbstractNode {
protected:
    void decrementNumberOfKeys();

    Int freeSpace(Int sizeOfKey);

    void incrementNumberOfKeys();

    Int getOffset(Int idx, Int sizeOfKey);

    Int sizeOfFeatures() {
        return Node::sizeOfInt;
    }

    virtual Int sizeOfEntry() = 0;


public:

    RTreeNode(Node * node) : AbstractNode(node) {
        static_assert(is_base_of<Rectangle<K>, K>::value, "Generic type is not derived of the Rectangle class");
    }

    virtual ~RTreeNode(){

    }

    K * buildKey(Int idx);

    void clear();

    Int indexOfKey(K * key);

    Int readNumberOfKeys();
};


template <typename K>
K * RTreeNode<K>::buildKey(Int idx) {
    K * key;
    if ((idx >= 0) && (idx < this->readNumberOfKeys())) {
        key = new K();
        //pull the key of the page
        key->pullKey(this->getArray(), this->getOffset(idx, key->sizeOfKey()));
        return key;
    }//endif
    return NULL;
}//buildKey

//----------------------------------------------------------------------

template <typename K>
void RTreeNode<K>::clear() {
    //zering numberOfKeys
    Int pos = Node::sizeOfHeader();
    this->writeInt(pos, 0);
}//clear

//----------------------------------------------------------------------

template <typename K>
void RTreeNode<K>::decrementNumberOfKeys() {
    Int num = this->readNumberOfKeys() - 1;
    Int pos = Node::sizeOfHeader();
    this->writeInt(pos, num);
}//decrementNumberOfKey

//----------------------------------------------------------------------

template <typename K>
Int RTreeNode<K>::freeSpace(Int sizeOfKey) {
    Int num = this->readNumberOfKeys();
    Int free = 0;
    if (num == 0) {
        free = this->sizeOfArray() - Node::sizeOfHeader() - this->sizeOfFeatures();
    } else {
        free = this->sizeOfArray() - Node::sizeOfHeader()
                - this->sizeOfFeatures() - (num * this->sizeOfEntry())
                - (this->sizeOfArray() - this->getOffset((num - 1), sizeOfKey));
    }//endif
    return free;
}//freeSpace

//----------------------------------------------------------------------

template <typename K>
Int RTreeNode<K>::indexOfKey(K * key) {
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
void RTreeNode<K>::incrementNumberOfKeys() {
    Int num = this->readNumberOfKeys() + 1;
    Int pos = Node::sizeOfHeader();
    this->writeInt(pos, num);
}//incrementNumberOfKey

//----------------------------------------------------------------------

template <typename K>
Int RTreeNode<K>::readNumberOfKeys() {
    Int pos = Node::sizeOfHeader();
    return this->readInt(pos);
}//readNumberOfKeys

//----------------------------------------------------------------------

template <typename K>
Int RTreeNode<K>::getOffset(Int idx, Int sizeOfKey) {
    return this->sizeOfArray() - ((idx + 1) * sizeOfKey);
}//readOffset

//----------------------------------------------------------------------

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_RTREENODE_H_ */
