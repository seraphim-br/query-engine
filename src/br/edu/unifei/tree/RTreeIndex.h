/*
 * RTreeIndex.h
 *
 *  Created on: 25 de set de 2019
 *      Author: windows
 */

#ifndef BR_EDU_UNIFEI_TREE_RTREEINDEX_H_
#define BR_EDU_UNIFEI_TREE_RTREEINDEX_H_

#include <br/edu/unifei/tree/Types.h>

#include <br/edu/unifei/tree/RTreeNode.h>
#include <type_traits>
#include <cfloat>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

template <typename K>
class MTree;

using namespace std;

template <typename K>
class RTreeIndex : public RTreeNode<K>{
private:
    static const Int nodeType = 7;
    static const Int numMbrs = 0;

protected:

    Int getNodeType() {
        return nodeType;
    }//getNodeType

    Int sizeOfEntry() {
        return Node::sizeOfLong + Node::sizeOfDouble + Node::sizeOfDouble + Node::sizeOfInt;
    }//sizeOfEntry

    void writeSubPageId(Int idx, Long pageId);

public:

    RTreeIndex(Node * node) : RTreeNode<K>(node) {
        static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Order class");
        this->initialize(node);
    }

    virtual ~RTreeIndex() {
    }

    Bool addKey(K * key, Long subPageId);

    Bool addKey(K * key1, Long subPageId1, K * key2, Long subPageId2);

    Int indexOfInsertion(K * key, EuclideanGeometry<K> * geometry);

    Int indexOfSubPageId(Long pageId);

    static Bool matchNodeType(Node * node) {
        return node->readNodeType() == RTreeIndex<K>::nodeType;
    }//matchNodeType

    Long readSubPageId(Int idx);

    void remove(Int idx);

    static long getNumMbrs(){
    	return this->numMbrs;
    }

    static void resetNumMbrs(){
    	this->numMbrs = 0;
    }

    void replace(int idx, K * key);

};


//----------------------------------------------------------------------

template <typename K>
Bool RTreeIndex<K>::addKey(K * key, Double dist, Long subPageId, Double covRad) {
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
        this->writeSubPageId(total, subPageId);
        this->writeCoveringRadius(total, covRad);
        this->incrementNumberOfKeys();

        return true;
    }
}//addKey

//----------------------------------------------------------------------

template <typename K>
Bool RTreeIndex<K>::addKey(K * key1, Double dist1, Long subPageId1, Double covRad1, K * key2, Double dist2, Long subPageId2, Double covRad2) {
    Int sizeKey1 = key1->sizeOfKey();
    Int sizeKey2 = key2->sizeOfKey();

    if (sizeKey1 + sizeKey2 + 2 * this->sizeOfEntry() > this->freeSpace()) {
        return false;
    } else {
        addKey(key1, dist1, subPageId1, covRad1);
        addKey(key2, dist2, subPageId2, covRad2);
        return true;
    }
}//addKey

//----------------------------------------------------------------------

template <typename K>
Double RTreeIndex<K>::distanceToQualify(Int idx, K * insertKey) {
    K * key = this->buildKey(idx);
    this->calculatedDistance++;
    double dist = insertKey->distanceTo(key);
    delete key;
    return dist;
}//distanceToQualify

//----------------------------------------------------------------------

template <typename K>
Int RTreeIndex<K>::indexOfInsertion(K * key) {
    Double distIn = DBL_MAX; // Minimum distance if key is inside some obj covering radius.
    Double distOut = DBL_MAX; // Minimum distance if key is outside some obj covering radius.
    Double newCoverage = DBL_MAX;
    Int idxIn = 0; // Index of obj in which key has minimum distance (inside cov rad).
    Int idxOut = 0; // Index of obj in which key has minimum distance (outside cov rad).
    Bool in = false; // Weather key is inside some cov rad.
    Int total = this->readNumberOfKeys();

    for (Int i = 0; i < total; i++) {
        K * storedKey = this->buildKey(i); // Key stored in the node.

        Double rad = this->readCoveringRadius(i); // storedKey cov rad: r(Or)

        this->calculatedDistance++;
        Double dist = storedKey->distanceTo(key); // Distance from key to storedKey: d(Or, On)

        if (in) {
            //exists a radii covering
            if (dist <= rad && dist < distIn) // Looking for minimum distance.
            {
                //these radius too cover and is less
                distIn = dist; // Minimum distance between storedKey and key.
                idxIn = i; // storedKey in which distance to key is minimum.
            }
        } else {
            //does not exists a radii covering
            if (dist <= rad) // key is inside storedKey covering radius.
            {
                //these radius cover
                in = true; // !in to set true once.
                distIn = dist; // Minimum distance between storedKey and key.
                idxIn = i; // storedKey in which distance to key is minimum.
            } else {
                //these radius does not cover
                if (dist - rad < distOut) // If key is outside any storedKey cov rad
                {
                    //minimize distOut
                    distOut = dist - rad; // Minimum enlargement of cov rad.
                    idxOut = i; // storedKey to minimum cov rad enlargement.
                    newCoverage = dist;
                }
            }
        }
        //clean home
        delete storedKey;
    }

    if (in) // Key is inside entry[idxIn] covering radius and the distance is minimum
    {
        return idxIn;
    } else // No entry cov rad contains key.
    {
        // Increase the most suitable (minimum enlargement) entry cov rad.
        //this.writeCoveringRadius(idxOut, this.readCoveringRadius(idxOut) + distOut);
        this->writeCoveringRadius(idxOut, newCoverage + MTree<K>::precisionError);
        return idxOut;
    }
}//indexOfInsertion

//----------------------------------------------------------------------

template <typename K>
Int RTreeIndex<K>::indexOfSubPageId(Long pageId) {
    Int total = this->readNumberOfKeys();
    for (Int i = 0; i < total; i++) {
        if (this->readSubPageId(i) == pageId) {
            return i;
        }
    }

    return -1;
}//indexOfSubPageId

//----------------------------------------------------------------------

template <typename K>
void RTreeIndex<K>::remove(Int idx) {
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
        this->writeSubPageId(k, this->readSubPageId(k + 1));

        this->writeCoveringRadius(k, this->readCoveringRadius(k + 1));
    }
    this->decrementNumberOfKeys();
}//remove

//----------------------------------------------------------------------

template <typename K>
Double RTreeIndex<K>::readCoveringRadius(Int idx) {
    Int pos = Node::sizeOfHeader()
            + this->sizeOfFeatures()
            + (idx * this->sizeOfEntry())
            + Node::sizeOfLong; // sub

    return this->readDouble(pos);
}//readCoveringRadius

//----------------------------------------------------------------------

template <typename K>
void RTreeIndex<K>::writeCoveringRadius(Int idx, Double rad) {
    Int pos = Node::sizeOfHeader()
            + this->sizeOfFeatures()
            + (idx * this->sizeOfEntry())
            + Node::sizeOfLong; // sub

    this->writeDouble(pos, rad);
}//writeCoveringRadius

//----------------------------------------------------------------------

template <typename K>
Long RTreeIndex<K>::readSubPageId(Int idx) {
    Int pos = Node::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    return this->readLong(pos);
}//readSubPageId

//----------------------------------------------------------------------

template <typename K>
void RTreeIndex<K>::writeSubPageId(Int idx, Long pageId) {
    Int pos = Node::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    this->writeLong(pos, pageId);
}//writeSubPageId

//----------------------------------------------------------------------

template <typename K>
void RTreeIndex<K>::replace(int idx, K * key){
	int off = this->getOffset(idx, key->sizeOfKey());
	key->pushKey(this->getArray(), off);
}

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */


#endif /* BR_EDU_UNIFEI_TREE_RTREEINDEX_H_ */
