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
#ifndef BR_EDU_UNIFEI_TREE_RTREE_H
#define	BR_EDU_UNIFEI_TREE_RTREE_H

#include <limits>
#include <type_traits>
#include <ctime>
#include <ratio>
#include <chrono>
#include <cstdlib>
#include <cfloat>

#include <br/edu/unifei/tree/AveragePerformance.h>
#include <br/edu/unifei/tree/EuclideanGeometry.h>
#include <br/edu/unifei/tree/RTreeDescriptor.h>
#include <br/edu/unifei/tree/Session.h>
#include <br/edu/unifei/tree/Workspace.h>
#include <br/edu/unifei/tree/RTreePromotion.h>
#include <br/edu/unifei/tree/RTreeLeaf.h>
#include <br/edu/unifei/tree/RTreeIndex.h>
#include <br/edu/unifei/tree/Metric.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;
using namespace std::chrono;
using namespace br::edu::unifei::tree;

template <typename K>
class RTree {
private:
	Workspace * workspace;

	EuclideanGeometry<K> * geometry;

    AveragePerformance * averageForAdd;
    AveragePerformance * averageForFind;

    RTreePromotion<K> * splitIndex(Session * se,
            RTreeIndex<K> * fullIndex, RTreeIndex<K> * newIndex,
            K * Key, Long subId);

    RTreePromotion<K> * splitLeaf(Session * se, RTreeLeaf<K> * fullLeaf, RTreeLeaf<K> * newLeaf, K * key, Uuid * uuid);


public:

    RTree(Workspace * workspace);

    virtual ~RTree();

    const static double precisionError;

    Bool add(K * key);

    AveragePerformance * getAverageForAdd() {
        return averageForAdd;
    }//getAverageForAdd

    AveragePerformance * getAverageForFind() {
        return averageForFind;
    }//getAverageForFind

    Workspace * getWorkspace() {
    	return this->workspace;
    }

    Long getRootPageId();

    Bool remove(K * key);

    Int height();

};

//----------------------------------------------------------------------

template <typename K>
const double RTree<K>::precisionError = 0.0001;

//----------------------------------------------------------------------

template <typename K>
RTree<K>::RTree(Workspace *workspace) {
    static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Metric class");
    averageForAdd = new AveragePerformance();
    averageForFind = new AveragePerformance();
    geometry = new EuclideanGeometry<K>();
    this->workspace = workspace;
    Session * se = this->workspace->openSession();
    Long pageIdDescriptor = se->findPageIdDescriptor(RTreeDescriptor::nodeType);
    RTreeDescriptor * descriptor = new RTreeDescriptor(se->load(pageIdDescriptor));
    //clean home
    delete descriptor;
    se->close();
    delete se;
}//RTree

//----------------------------------------------------------------------

template <typename K>
RTree<K>::~RTree() {
    delete averageForAdd;
    delete averageForFind;
    delete geometry;
}//~RTree

//----------------------------------------------------------------------

template <typename K>
Bool RTree<K>::add(K * key) {
    steady_clock::time_point t1 = steady_clock::now();
    Session * se = this->workspace->openSession();
    Long diskAccess = se->getBlockAccess();
    long verifications = this->geometry->getCalculatedOverlap();
    Long pageIdDescriptor = se->findPageIdDescriptor(RTreeDescriptor::nodeType);
    RTreeDescriptor * descriptor = new RTreeDescriptor(se->load(pageIdDescriptor));
    Long rootPageId = descriptor->readRootPageId();

    if (rootPageId == 0) // tree is empty
    {
        RTreeLeaf<K> * leaf = new RTreeLeaf<K>(se->create()); // em java passa dois argumentos
        leaf->addKey(key, key->getUuid());
        // Circularly link
        leaf->writePreviousPageId(leaf->getPageId());
        leaf->writeNextPageId(leaf->getPageId());
        descriptor->writeRootPageId(leaf->getPageId());
        descriptor->incTreeHeight();
        //clean home
        delete leaf;
    } else {
        Int pathLength = descriptor->readTreeHeight();
        Long * path = new Long[pathLength];
        Node * node = se->load(rootPageId);

        Int level = 1;
        Int qualify;
        Long subPageId;
        path[0] = 0;

        while (RTreeIndex<K>::matchNodeType(node)) {
            RTreeIndex<K> * index = new RTreeIndex<K>(node);
            path[level] = index->getPageId();
            level++;
            qualify = index->indexOfInsertion(key, this->geometry);
            subPageId = index->readSubPageId(qualify);
            node = se->load(subPageId);
            //clean home
            delete index;
        }

        RTreeLeaf<K> * leaf = new RTreeLeaf<K>(node);

        if (leaf->addKey(key, key->getUuid()) == false) {
            Bool promote = true;
            RTreeLeaf<K> * newLeaf = new RTreeLeaf<K>(se->create());
            RTreeIndex<K> * index;
            RTreeIndex<K> * newIndex;
            Long pageIdRem = leaf->getPageId();
            //split leaf

			RTreePromotion<K> *objPromote = this->splitLeaf(se, leaf, newLeaf, key, key->getUuid());
			RTreePromotion<K> * objIndexPromote;

			level = pathLength - 1;

            while (level > 0) {
				node = se->load(path[level]);

				if (promote == true) {
					index = new RTreeIndex<K>(node);
					Int idxReplc = index->indexOfSubPageId(pageIdRem);
					index->replace(idxReplc, objPromote->getFirstKey());

					if (index->addKey(objPromote->getSecondKey(), objPromote->getSecondSubPageId())) {

						promote = false;
					} else {
						newIndex = new RTreeIndex<K>(se->create());
						objIndexPromote = this->splitIndex(se, index, newIndex, objPromote->getSecondKey(), objPromote->getSecondSubPageId());
//						nodeLeft = index;
//						nodeRight = newIndex;
						pageIdRem = node->getPageId();
						promote = true;
						//clean home
						delete objPromote;
			            delete newIndex;
						//update objPromote
						objPromote = objIndexPromote;

					}
					//clean home
		            delete index;
				}

				level--;
			}

            if (promote == true) {
				// Promote
				newIndex = new RTreeIndex<K>(se->create());
				newIndex->addKey(objPromote->getFirstKey(), objPromote->getFistSubPageId());
				newIndex->addKey(objPromote->getSecondKey(), objPromote->getSecondSubPageId());
				// Circularly link
				newIndex->writePreviousPageId(newIndex->getPageId());
				newIndex->writeNextPageId(newIndex->getPageId());
				//ajust rootPageId
				descriptor->writeRootPageId(newIndex->getPageId());
				descriptor->incTreeHeight();
			}
            //clean home
            delete objPromote;
        }
        //clean home
        delete [] path;
        delete leaf;
    }

    //statistic for add
    diskAccess = se->getBlockAccess() - diskAccess;
    averageForAdd->incrementDiskAccess(diskAccess);
    verifications = this->geometry->getCalculatedOverlap() - verifications;
    averageForAdd->incrementVerification(verifications);
    //clean home
    delete descriptor;
    se->close();
    delete se;

    duration<double> time_span = duration_cast<duration<double>>(steady_clock::now() - t1);
    averageForAdd->incrementTime(time_span.count()*1000000);
    averageForAdd->incrementMeasurement();
    return true;
}//add

//----------------------------------------------------------------------

template <typename K>
Long RTree<K>::getRootPageId() {
    Session * se = this->workspace->openSession();
    Long pageIdDescriptor = se->findPageIdDescriptor(RTreeDescriptor::nodeType);
    RTreeDescriptor * descriptor = new RTreeDescriptor(se->load(pageIdDescriptor));
    Long rootPageId = descriptor->readRootPageId();
    //clean home
    delete descriptor;
    se->close();
    delete se;
    return rootPageId;
}//getRootPageId

//----------------------------------------------------------------------

template <typename K>
Bool RTree<K>::remove(K * key) {
    return false;
}//remove

//----------------------------------------------------------------------

template <typename K>
RTreePromotion<K> * RTree<K>::splitIndex(Session * se,
        RTreeIndex<K> * fullIndex, RTreeIndex<K> * newIndex,
        K * key, Long subId) {
    Int i, j;
    Int total = fullIndex->readNumberOfKeys() + 1;

    K * objRep1;
	K * objRep2;
	Byte * serialRep1;
	Byte * serialRep2;

    K ** vecKey = new K*[total];
	Long * vecSub = new Long[total];
    for (i = 0; i < total - 1; i++) {
    	vecKey[i] = fullIndex->buildKey(i);
    	vecSub[i] = fullIndex->readSubPageId(i);
	}//endfor
    vecKey[i] = key;
	vecSub[i] = subId;

    RTreePromotion<K> * objPromote = NULL;
    Double ocup = -1.0; //d

    Double d;
	Int idx1 = 0, idx2 = 1;

	for (i = 0; i < total - 1; i++) // Quadratic Split
	{
		for (j = i + 1; j < total; j++) {
			K * mbrUnion = geometry->unionRectangles(vecKey[i], vecKey[j]);
			// Calc Dead space: union(mbr1, mbr2) - (mbr1 + mbr2 - intersec(mbr1, mbr2))
			d = geometry->occupancy(mbrUnion)
					- (geometry->occupancy(vecKey[i])
					+ geometry->occupancy(vecKey[j]));

			if (d > ocup) {
				ocup = d;
				idx1 = i;
				idx2 = j;
			}
			// clean home
			delete mbrUnion;
		}
	}

	//copy representative 1
	objRep1 = new K();
	serialRep1 = new Byte[vecKey[idx1]->sizeOfKey()];
	vecKey[idx1]->pushKey(serialRep1, 0);
	objRep1->pullKey(serialRep1, 0);
	//copy representative 2
	objRep2 = new K();
	serialRep2 = new Byte[vecKey[idx2]->sizeOfKey()];
	vecKey[idx2]->pushKey(serialRep2, 0);
	objRep2->pullKey(serialRep2, 0);

	//cleanning fullIndex and inserting vecKey[idx1]
	fullIndex->clear();
	fullIndex->addKey(vecKey[idx1], vecSub[idx1]);

	//cleanning newIndex and inserting vecKey[idx2]
	newIndex->clear();
	newIndex->addKey(vecKey[idx2], vecSub[idx2]);

	//promoted MBR
	objPromote = new RTreePromotion<K>(objRep1, fullIndex->getPageId(), objRep2, newIndex->getPageId());
	for (i = 0; i < total; i++) {
		if (!(i == idx1 || i == idx2)) {
			K * union1 = geometry->unionRectangles(objPromote->getFirstKey(), vecKey[i]);
			double ocup1 = geometry->occupancy(union1) - geometry->occupancy(objPromote->getFirstKey());
			K * union2 = geometry->unionRectangles(objPromote->getSecondKey(), vecKey[i]);
			double ocup2 = geometry->occupancy(union2) - geometry->occupancy(objPromote->getSecondKey());

			if (ocup1 < ocup2) {
				if (!fullIndex->addKey(vecKey[i], vecSub[i])) {
					throw runtime_error("add fail");
				}
				//clean home
				delete union2;
				if (objPromote->getFirstKey() != NULL) {
					delete objPromote->getFirstKey();
				}
				objPromote->setFirstKey(union1);
			} else if (ocup1 > ocup2) {
				if (!newIndex->addKey(vecKey[i], vecSub[i])) {
					throw runtime_error("add fail");
				}
				//clean home
				delete union1;
				if (objPromote->getSecondKey() != NULL) {
					delete objPromote->getSecondKey();
				}
				objPromote->setSecondKey(union2);
			} else {
				double ocupProm1 = geometry->occupancy(objPromote->getFirstKey());
				double ocupProm2 = geometry->occupancy(objPromote->getSecondKey());
				if (ocupProm1 < ocupProm2) {
					if (!fullIndex->addKey(vecKey[i], vecSub[i])) {
						throw runtime_error("add fail");
					}
					//clean home
					delete union2;
					if (objPromote->getFirstKey() != NULL) {
						delete objPromote->getFirstKey();
					}
					objPromote->setFirstKey(union1);
				} else if (ocupProm1 > ocupProm2) {
					if (!newIndex->addKey(vecKey[i], vecSub[i])) {
						throw runtime_error("add fail");
					}
					//clean home
					delete union1;
					if (objPromote->getSecondKey() != NULL) {
						delete objPromote->getSecondKey();
					}
					objPromote->setSecondKey(union2);
				} else {
					double dist1 = objPromote->getFirstKey()->distanceTo(vecKey[i]);
					double dist2 = objPromote->getSecondKey()->distanceTo(vecKey[i]);
			        averageForAdd->incrementVerification(2);

					if (dist1 < dist2) {
						if (!fullIndex->addKey(vecKey[i], vecSub[i])) {
							throw runtime_error("add fail");
						}
						//clean home
						delete union2;
						if (objPromote->getFirstKey() != NULL) {
							delete objPromote->getFirstKey();
						}
						objPromote->setFirstKey(union1);
					} else if (dist1 > dist2) {
						if (!newIndex->addKey(vecKey[i], vecSub[i])) {
							throw runtime_error("add fail");
						}
						//clean home
						delete union1;
						if (objPromote->getSecondKey() != NULL) {
							delete objPromote->getSecondKey();
						}
						objPromote->setSecondKey(union2);
					} else if (fullIndex->readNumberOfKeys() <= newIndex->readNumberOfKeys()) {
						if (!fullIndex->addKey(vecKey[i], vecSub[i])) {
							throw runtime_error("add fail");
						}
						//clean home
						delete union2;
						if (objPromote->getFirstKey() != NULL) {
							delete objPromote->getFirstKey();
						}
						objPromote->setFirstKey(union1);
					} else {
						if (!newIndex->addKey(vecKey[i], vecSub[i])) {
							throw runtime_error("add fail");
						}
						//clean home
						delete union1;
						if (objPromote->getSecondKey() != NULL) {
							delete objPromote->getSecondKey();
						}
						objPromote->setSecondKey(union2);
					}
				}
			}
		}
	}


    // Circularly linked list
    newIndex->writeNextPageId(fullIndex->readNextPageId());
    fullIndex->writeNextPageId(newIndex->getPageId());
    newIndex->writePreviousPageId(fullIndex->getPageId());
    Long nextPageId = newIndex->readNextPageId();
    if (nextPageId != fullIndex->getPageId()) {
    	RTreeIndex<K> * nextIndex = new RTreeIndex<K>(se->load(nextPageId));
        nextIndex->writePreviousPageId(newIndex->getPageId());
        delete nextIndex;
    } else {
        fullIndex->writePreviousPageId(newIndex->getPageId());
    }
    // clear home
	for (i = 0; i < total - 1 ; i++) {
		delete vecKey[i];
	}//endfor
	delete [] vecSub;
	delete [] vecKey;
	//clean home
    delete [] serialRep1;
    delete [] serialRep2;
    return objPromote;
}//splitIndex

//----------------------------------------------------------------------

template <typename K>
RTreePromotion<K> * RTree<K>::splitLeaf(Session * se, RTreeLeaf<K> * fullLeaf, RTreeLeaf<K> * newLeaf, K * key, Uuid * uuid) {

    Int i, j;
    Int total = fullLeaf->readNumberOfKeys() + 1;

    //copy all keys and subPageId
	Uuid ** vecUuid = new Uuid*[total];
	K ** vecKey = new K*[total];

	K * objRep1;
	K * objRep2;
	Byte * serialRep1;
	Byte * serialRep2;

	//coping key and subPageId
	for (i = 0; i < total - 1; i++) {
		vecKey[i] = fullLeaf->buildKey(i);
		vecUuid[i] = fullLeaf->readEntityUuid(i);
	}//endfor

	//copy key and pageId in the correct position
	vecKey[i] = key;
	//copy the pageId in the correct position
	vecUuid[i] = uuid;

	// Choose promote
	Double ocup = -1.0;

	RTreePromotion<K> * objPromote;

    Double d;
    Int idx1 = 0, idx2 = 1;

    for (i = 0; i < total - 1; i++) // Quadratic Split
	{
		for (j = i + 1; j < total; j++) {
			K * mbrUnion = geometry->unionRectangles(vecKey[i], vecKey[j]);
			// Calc Dead space: union(mbr1, mbr2) - (mbr1 + mbr2 - intersec(mbr1, mbr2))
			d = geometry->occupancy(mbrUnion) //mbrUnion.occupancy()
					- (geometry->occupancy(vecKey[i])
					+ geometry->occupancy(vecKey[j]));

			if (d > ocup) {
				ocup = d;
				idx1 = i;
				idx2 = j;
			}
			// clean home
			delete mbrUnion;
		}
	}

    //copy representative 1
	objRep1 = new K();
	serialRep1 = new Byte[vecKey[idx1]->sizeOfKey()];
	vecKey[idx1]->pushKey(serialRep1, 0);
	objRep1->pullKey(serialRep1, 0);
	//copy representative 2
	objRep2 = new K();
	serialRep2 = new Byte[vecKey[idx2]->sizeOfKey()];
	vecKey[idx2]->pushKey(serialRep2, 0);
	objRep2->pullKey(serialRep2, 0);

    //cleanning fullIndex and inserting vecKey[idx1]
	fullLeaf->clear();
	fullLeaf->addKey(vecKey[idx1], vecUuid[idx1]);
	//cleanning newIndex and inserting vecKey[idx2]
	newLeaf->clear();
	newLeaf->addKey(vecKey[idx2], vecUuid[idx2]);
	//promoted MBR
	objPromote = new RTreePromotion<K>(objRep1, fullLeaf->getPageId(), objRep2, newLeaf->getPageId());


//	selecionar dois objs com maior ocupancia para subir
//	cria mais um leaf
//	dividir os objetos que sobrou entre os dois leafs (
//	criterio1: menor ocupancia da uniao do obj a ser inserido e o promovido;
//	criterio2: menor ocupancia do objeto promovido sem uniao
//	criterio3: menor distancia entre o objeto a ser inserido e o promovido
//	else: insere no novo
//	)

	// Distributing keys
	for (i = 0; i < total; i++) {
		if (!(i == idx1 || i == idx2)) {
			K * union1 = this->geometry->unionRectangles(objPromote->getFirstKey(), vecKey[i]);
			double ocup1 = geometry->occupancy(union1) - geometry->occupancy(objPromote->getFirstKey());
			K * union2 = this->geometry->unionRectangles(objPromote->getSecondKey(), vecKey[i]);
			double ocup2 = geometry->occupancy(union2) - geometry->occupancy(objPromote->getSecondKey());

			if (ocup1 < ocup2) {
				if (!fullLeaf->addKey(vecKey[i], vecUuid[i])) {
					throw runtime_error("add fail");
				}
				//clean home
				delete union2;
				if (objPromote->getFirstKey() != NULL) {
					delete objPromote->getFirstKey();
				}
				objPromote->setFirstKey(union1);
			} else if (ocup1 > ocup2) {
				if (!newLeaf->addKey(vecKey[i], vecUuid[i])) {
					throw runtime_error("add fail");
				}
				//clean home
				delete union1;
				if (objPromote->getSecondKey() != NULL) {
					delete objPromote->getSecondKey();
				}
				objPromote->setSecondKey(union2);
			} else {
				double ocupProm1 = geometry->occupancy(objPromote->getFirstKey());
				double ocupProm2 = geometry->occupancy(objPromote->getSecondKey());
				if (ocupProm1 < ocupProm2) {
					if (!fullLeaf->addKey(vecKey[i], vecUuid[i])) {
						throw runtime_error("add fail");
					}
					//clean home
					delete union2;
					if (objPromote->getFirstKey() != NULL) {
						delete objPromote->getFirstKey();
					}
					objPromote->setFirstKey(union1);
				} else if (ocupProm1 > ocupProm2) {
					if (!newLeaf->addKey(vecKey[i], vecUuid[i])) {
						throw runtime_error("add fail");
					}
					//clean home
					delete union1;
					if (objPromote->getSecondKey() != NULL) {
						delete objPromote->getSecondKey();
					}
					objPromote->setSecondKey(union2);
				} else {
					double dist1 = objPromote->getFirstKey()->distanceTo(vecKey[i]);
					double dist2 = objPromote->getSecondKey()->distanceTo(vecKey[i]);
			        averageForAdd->incrementVerification(2);

					if (dist1 < dist2) {
						if (!fullLeaf->addKey(vecKey[i], vecUuid[i])) {
							throw runtime_error("add fail");
						}
						//clean home
						delete union2;
						if (objPromote->getFirstKey() != NULL) {
							delete objPromote->getFirstKey();
						}
						objPromote->setFirstKey(union1);
					} else if (dist1 > dist2) {
						if (!newLeaf->addKey(vecKey[i], vecUuid[i])) {
							throw runtime_error("add fail");
						}
						//clean home
						delete union1;
						if (objPromote->getSecondKey() != NULL) {
							delete objPromote->getSecondKey();
						}
						objPromote->setSecondKey(union2);
					} else if (fullLeaf->readNumberOfKeys() <= newLeaf->readNumberOfKeys()) {
						if (!fullLeaf->addKey(vecKey[i], vecUuid[i])) {
							throw runtime_error("add fail");
						}
						//clean home
						delete union2;
						if (objPromote->getFirstKey() != NULL) {
							delete objPromote->getFirstKey();
						}
						objPromote->setFirstKey(union1);
					} else {
						if (!newLeaf->addKey(vecKey[i], vecUuid[i])) {
							throw runtime_error("add fail");
						}
						//clean home
						delete union1;
						if (objPromote->getSecondKey() != NULL) {
							delete objPromote->getSecondKey();
						}
						objPromote->setSecondKey(union2);
					}
				}
			}
		}
	}

	// Circularly link
	newLeaf->writeNextPageId(fullLeaf->readNextPageId());
	fullLeaf->writeNextPageId(newLeaf->getPageId());
	newLeaf->writePreviousPageId(fullLeaf->getPageId());
	long nextPageId = newLeaf->readNextPageId();
	if (nextPageId != fullLeaf->getPageId()) {
		RTreeLeaf<K> * nextLeaf = new RTreeLeaf<K>(se->load(nextPageId)); //, this->getObjectClass()
		nextLeaf->writePreviousPageId(newLeaf->getPageId());
		delete nextLeaf;
	} else {
		fullLeaf->writePreviousPageId(newLeaf->getPageId());
	}
	// clear home (vecKey[total-1] and vecUuid[total-1] are parameters)
	for (i = 0; i < total - 1 ; i++) {
		delete vecKey[i];
		delete vecUuid[i];
	}//endfor
	delete [] vecUuid;
	delete [] vecKey;
	//clean home
    delete [] serialRep1;
    delete [] serialRep2;
	return objPromote;


}
//----------------------------------------------------------------------

template <typename K>
Int RTree<K>::height() {
    Session * se = this->workspace->openSession();
    Long pageIdDescriptor = se->findPageIdDescriptor(RTreeDescriptor::nodeType);
    RTreeDescriptor * descriptor = new RTreeDescriptor(se->load(pageIdDescriptor));
    Int total = 0;
    Long rootPageId = descriptor->readRootPageId();

    Node * node = NULL;
    RTreeIndex<K> * index = NULL;
    Long sub;

    if (rootPageId != 0) {
        node = se->load(rootPageId);
        while (RTreeIndex<K>::matchNodeType(node)) {
            index = new RTreeIndex<K>(node);
            sub = index->readSubPageId(0);
            node = se->load(sub);
            total++;
            //clean home
            delete index;
        }//endwhile
        total++;
    }//endif

    // clean home
    delete descriptor;
    se->close();
    delete se;

    return total;
}

//----------------------------------------------------------------------

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif	/* BR_EDU_UNIFEI_TREE_RTREE_H */




