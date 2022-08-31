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

template<typename K>
class MTree;

using namespace std;

template<typename K>
class RTreeIndex: public RTreeNode<K> {
private:
	static const Int nodeType = 9;

protected:

	Int getNodeType() {
		return nodeType;
	} //getNodeType

	Int sizeOfEntry() {
		return Node::sizeOfLong + Node::sizeOfDouble + Node::sizeOfDouble
				+ Node::sizeOfInt;
	} //sizeOfEntry

	void writeSubPageId(Int idx, Long pageId);

public:

	RTreeIndex(Node *node) :
			RTreeNode<K>(node) {
		static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Order class");
		this->initialize(node);
	}

	virtual ~RTreeIndex() {
	}

	Bool addKey(K *key, Long subPageId);

	Bool addKey(K *key1, Long subPageId1, K *key2, Long subPageId2);

	Int indexOfInsertion(K *key, EuclideanGeometry<K> *geometry);

	Int indexOfSubPageId(Long pageId);

	static Bool matchNodeType(Node *node) {
		return node->readNodeType() == RTreeIndex<K>::nodeType;
	} //matchNodeType

	Long readSubPageId(Int idx);

	void replace(int idx, K *key);

};

//----------------------------------------------------------------------

template<typename K>
Bool RTreeIndex<K>::addKey(K *key, Long subPageId) {
	Int total = this->readNumberOfKeys();
	Int size = key->sizeOfKey();

	if (size + this->sizeOfEntry() > this->freeSpace(size)) // It there is not space.
			{
		return false;
	} else {
		int off = this->getOffset(total, size);
		key->pushKey(this->getArray(), off); // Adding
		this->writeSubPageId(total, subPageId);
		this->incrementNumberOfKeys();

		return true;
	}
} //addKey

//----------------------------------------------------------------------

template<typename K>
Bool RTreeIndex<K>::addKey(K *key1, Long subPageId1, K *key2, Long subPageId2) {
	Int size = key1->sizeOfKey();

	if (size + size + 2 * this->sizeOfEntry() > this->freeSpace(size)) {
		return false;
	} else {
		addKey(key1, subPageId1);
		addKey(key2, subPageId2);
		return true;
	}
} //addKey

//----------------------------------------------------------------------

template<typename K>
Int RTreeIndex<K>::indexOfInsertion(K *key, EuclideanGeometry<K> *geometry) {
	Int total = this->readNumberOfKeys(); // Number of keys in node
	Double minOccupancy = DBL_MAX; // Minimum occupancy (area, volume...) of MBR.
	Double minEnlargement = DBL_MAX; // Minimum enlargement of occupancy (area, volume...) of MBR.
	Int idx = -1; // Qualified sub page
	K *storedKey; // To build keys stored into this node.
	K *newMbr = NULL;
	for (int i = 0; i < total; i++) {
		storedKey = this->buildKey(i);

		// Calc occupancy of stored key
		Double befOcup = geometry->occupancy(storedKey); // Occupancy (area, volume...) BEFORE enlargement.
		K *mbrUnion = geometry->unionRectangles(key, storedKey); // Enlargment
		Double aftOcup = geometry->occupancy(mbrUnion); // Occupancy (area, volume...) AFTER enlargement.

		Double enlarg = aftOcup - befOcup; // Enlargement of occupancy
		// Qualifies key with minimum enlargement. Resolve ties selecting minimum occupancy (area, volume...)
		if (enlarg < minEnlargement) {
			minEnlargement = enlarg;
			minOccupancy = befOcup;
			idx = i; // Qualify
			//clean home
			if (newMbr != NULL) {
				delete newMbr;
			}
			newMbr = mbrUnion;
		} else if (enlarg == minEnlargement && befOcup < minOccupancy) {
			minOccupancy = befOcup;
			idx = i; // Qualify
			//clean home
			if (newMbr != NULL) {
				delete newMbr;
			}
			newMbr = mbrUnion;
		} else {
			delete mbrUnion;
		}
		//clean home
		delete storedKey;
	}
	// If MBR was enlarged:
	if (minEnlargement != 0) {
		// Writes enlargement
		newMbr->pushKey(this->getArray(),
				this->getOffset(idx, newMbr->sizeOfKey()));
	}
	//clean home
	if (newMbr != NULL) {
		delete newMbr;
	}
	return idx;
} //indexOfInsertion

//----------------------------------------------------------------------

template<typename K>
Int RTreeIndex<K>::indexOfSubPageId(Long pageId) {
	Int total = this->readNumberOfKeys();
	for (Int i = 0; i < total; i++) {
		if (this->readSubPageId(i) == pageId) {
			return i;
		}
	}

	return -1;
} //indexOfSubPageId

//----------------------------------------------------------------------

template<typename K>
Long RTreeIndex<K>::readSubPageId(Int idx) {
	Int pos = Node::sizeOfHeader() + this->sizeOfFeatures()
			+ (idx * this->sizeOfEntry());
	return this->readLong(pos);
} //readSubPageId

//----------------------------------------------------------------------

template<typename K>
void RTreeIndex<K>::writeSubPageId(Int idx, Long pageId) {
	Int pos = Node::sizeOfHeader() + this->sizeOfFeatures()
			+ (idx * this->sizeOfEntry());
	this->writeLong(pos, pageId);
} //writeSubPageId

//----------------------------------------------------------------------

template<typename K>
void RTreeIndex<K>::replace(int idx, K *key) {
	int off = this->getOffset(idx, key->sizeOfKey());
	key->pushKey(this->getArray(), off);
}

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_RTREEINDEX_H_ */
