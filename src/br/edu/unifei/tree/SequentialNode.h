/*
Copyright (C) 2013     Enzo Seraphim

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
or visit <http://www.gnu.org/licenses/>
 */
#ifndef BR_EDU_UNIFEI_TREE_SEQUENTIALNODE_H_
#define BR_EDU_UNIFEI_TREE_SEQUENTIALNODE_H_

#include <br/edu/unifei/tree/Types.h>
#include <br/edu/unifei/tree/AbstractNode.h>
#include <br/edu/unifei/tree/Metric.h>


using namespace std;

namespace br {
namespace edu {
namespace unifei {
namespace tree {

template <typename K>
class SequentialNode : public AbstractNode {
protected:
    static const Int sizeOfFeatures = Node::sizeOfLong + Node::sizeOfInt;

    static const Int sizeOfEntry = Node::sizeOfInt;

    void decrementNumberOfEntities();

    Int freeSpace();

    void incrementNumberEntities();

    Int getNodeType();

    int readOffset(Int idx);

    void writeOffset(Int idx, Int off);

public:
    static const Int nodeType = 3;

    SequentialNode(Node * node);

    virtual ~SequentialNode() {
    }

    Bool addKey(K * entity);

    K * buildKey(Int idx);

    void clear();

    Uuid* findKey(K* entity);

    K* findUuid(Uuid * uuid);

    static Bool matchNodeType(Node* node);

    Int readNumberOfEntities();

};

//----------------------------------------------------------------------

template <typename K>
SequentialNode<K>::SequentialNode(Node * node) : AbstractNode(node) {
    static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Metric class");
    this->initialize(node);
}//SequentialNode

//----------------------------------------------------------------------

template <typename K>
Int SequentialNode<K>::freeSpace() {
    int size = this->readNumberOfEntities();
    if (size == 0) {
        return this->sizeOfArray() - Node::sizeOfHeader();
    } else {
        return this->sizeOfArray() - Node::sizeOfHeader() -
                sizeOfFeatures - (size * sizeOfEntry) -
                (this->sizeOfArray() - readOffset(size - 1));
    }//endif
}//freeSpace

//----------------------------------------------------------------------

template <typename K>
K * SequentialNode<K>::buildKey(Int idx) {
    if ((idx >= 0) && (idx < this->readNumberOfEntities())) {
        K * entity = new K();
        entity->pullKey(this->getArray(), readOffset(idx));
        return entity;
    } else {
        return NULL;
    }//endif
}//buildKey

//----------------------------------------------------------------------

template <typename K>
Int SequentialNode<K>::readOffset(Int idx) {
    int pos = Node::sizeOfHeader() + sizeOfFeatures + (idx * sizeOfEntry);
    return this->readInt(pos);
}//readOffset

//----------------------------------------------------------------------

template <typename K>
void SequentialNode<K>::writeOffset(Int idx, Int off) {
    int pos = Node::sizeOfHeader() + sizeOfFeatures + (idx * sizeOfEntry);
    this->writeInt(pos, off);
}//writeOffset

//----------------------------------------------------------------------

template <typename K>
void SequentialNode<K>::incrementNumberEntities() {
    int size = this->readNumberOfEntities() + 1;
    int pos = Node::sizeOfHeader() + Node::sizeOfLong;
    this->writeInt(pos, size);
}//incrementNumberEntities

//----------------------------------------------------------------------

template <typename K>
void SequentialNode<K>::decrementNumberOfEntities() {
    int size = this->readNumberOfEntities() - 1;
    int pos = Node::sizeOfHeader() + Node::sizeOfLong;
    this->writeInt(pos, size);
}//decrementNumberOfEntities

//----------------------------------------------------------------------

template <typename K>
void SequentialNode<K>::clear() {
    //zering next node
    int pos = Node::sizeOfHeader();
    this->writeLong(pos, 0);
    //zering numberOfKey
    pos = Node::sizeOfHeader() + Node::sizeOfLong;
    this->writeInt(pos, 0);
}//clear

//----------------------------------------------------------------------

template <typename K>
Bool SequentialNode<K>::addKey(K * metric) {
    int off;
    int idx = this->readNumberOfEntities();

    if (metric->sizeOfKey() + sizeOfEntry < freeSpace()) {
        //if is first position
        if (idx == 0) {
            off = this->sizeOfArray() - metric->sizeOfKey();
        } else {
            off = readOffset(idx - 1) - metric->sizeOfKey();
        }//endif
        //offset
        writeOffset(idx, off);
        //entity
        metric->pushKey(this->getArray(), off);
        //increment
        incrementNumberEntities();
        //clean home
        return true;
    } else {
        //clean home
        return false;
    }//endif
}//addKey

//----------------------------------------------------------------------

template <typename K>
Uuid* SequentialNode<K>::findKey(K* entity) {
    int i;
    Metric<K> * objSer;
    int total = readNumberOfEntities();
    for (i = 0; i < total; i++) {
        objSer = buildKey(i);
        if (objSer->isEqual(entity) == true) {
            //clean home
            Uuid *uuid = new Uuid(
                    objSer->getUuid()->getMostSignificantBits(),
                    objSer->getUuid()->getLeastSignificantBits());
            delete objSer;
            return uuid;
        }//endif
        //clean home
        delete objSer;
    }//endfor
    return NULL;
}//findKey

//----------------------------------------------------------------------

template <typename K>
K* SequentialNode<K>::findUuid(Uuid * uuid) {
    int i;
    K * objSer;
    int total = this->readNumberOfEntities();
    for (i = 0; i < total; i++) {
        objSer = this->buildKey(i);
        if (objSer->getUuid()->equals(uuid)) {
            return objSer;
        }//endif
        delete objSer;
    }//endfor
    return NULL;
}//findUuid

//----------------------------------------------------------------------

template <typename K>
Int SequentialNode<K>::getNodeType() {
    return this->nodeType;
}//getNodeType

//----------------------------------------------------------------------

template <typename K>
Int SequentialNode<K>::readNumberOfEntities() {
    int pos = Node::sizeOfHeader() + Node::sizeOfLong;
    return this->readInt(pos);
}//readNumberOfEntities

//----------------------------------------------------------------------

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_SEQUENTIALNODE_H_ */

