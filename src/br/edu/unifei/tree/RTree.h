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

    AveragePerformance * averageForAdd;
    AveragePerformance * averageForFind;

    RTreePromotion<K> * splitIndex(Session * se, RTreeIndex<K> * fullIndex,
            K * firstKey, Long firstSubId, Double firstCov, 
            K * secondKey, Long secondSubId, Double secondCov);

    RTreePromotion<K> * splitLeaf(Session * se, RTreeLeaf<K> * fullLeaf, K * key, Uuid * uuid);


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
}//~RTree

//----------------------------------------------------------------------

template <typename K>
Bool RTree<K>::add(K * key) {
    steady_clock::time_point t1 = steady_clock::now();
    Session * se = this->workspace->openSession();
    Long diskAccess = se->getBlockAccess();
    Long pageIdDescriptor = se->findPageIdDescriptor(RTreeDescriptor::nodeType);
    RTreeDescriptor * descriptor = new RTreeDescriptor(se->load(pageIdDescriptor));
    Long rootPageId = descriptor->readRootPageId();

    if (rootPageId == 0) // tree is empty
    {
        RTreeLeaf<K> * leaf = new RTreeLeaf<K>(se->create());
        leaf->addKey(key, DBL_MAX, key->getUuid());
        // Circularly link
        leaf->writePreviousPageId(leaf->getPageId());
        leaf->writeNextPageId(leaf->getPageId());
        descriptor->writeRootPageId(leaf->getPageId());
        descriptor->incTreeHeight();
        averageForAdd->incrementVerification(leaf->getCalculatedDistance());
        //clean home
        delete leaf;
    } else {
        Int pathLength = descriptor->readTreeHeight();
        Long * path = new Long[pathLength];
        Node * node = se->load(rootPageId);
        double dist = DBL_MAX; // Distance to parent

        Int level = 1;
        Int qualify;
        Long subPageId;
        path[0] = 0;

        while (RTreeIndex<K>::matchNodeType(node)) {
            RTreeIndex<K> * index = new RTreeIndex<K>(node);
            path[level] = index->getPageId();
            level++;
            qualify = index->indexOfInsertion(key);
            subPageId = index->readSubPageId(qualify);
            dist = index->distanceToQualify(qualify, key);
            node = se->load(subPageId);
            averageForAdd->incrementVerification(index->getCalculatedDistance());
            //clean home
            delete index;
        }

        RTreeLeaf<K> * leaf = new RTreeLeaf<K>(node);

        if (leaf->addKey(key, dist, key->getUuid()) == false) {
            Int idxSub, idxParent;
            Long pageIdSub;
            Long pageIdActual;
            double distPromoParent1, distPromoParent2;
            double coveringSub = 0;
            double cov1, cov2;
            bool promote = true;
            RTreeIndex<K> * actualIndex, * parentIndex;
            K * parentKey;
            //split leaf
            RTreePromotion<K> * objPromote = splitLeaf(se, leaf, key, key->getUuid());
            RTreePromotion<K> * objIndexPromote;

            level = pathLength - 1;
            Long parent = path[level];

            if (parent != 0) {
                //pageId of the sub node
                pageIdSub = leaf->getPageId();
                actualIndex = new RTreeIndex<K>(se->load(parent));

                while (level > 0) {
                    if (path[level - 1] != 0) {
                        //pageId of actual node
                        pageIdActual = actualIndex->getPageId();
                        parentIndex = new RTreeIndex<K>(se->load(path[level - 1]));

                        //calc dist
                        idxParent = parentIndex->indexOfSubPageId(pageIdActual);
                        parentKey = parentIndex->buildKey(idxParent);
                        distPromoParent1 = objPromote->getFirstKey()->distanceTo(parentKey);
                        distPromoParent2 = objPromote->getSecondKey()->distanceTo(parentKey);

                        averageForAdd->incrementVerification(parentIndex->getCalculatedDistance()+2);
                        //clean home
                        delete parentKey;
                    } else {
                        parentIndex = NULL;
                        distPromoParent1 = DBL_MAX;
                        distPromoParent2 = DBL_MAX;
                    }

                    //search the index
                    idxSub = actualIndex->indexOfSubPageId(pageIdSub);
                    if (promote == true) {
                        //remove old promoted key
                        actualIndex->remove(idxSub);

                        if (actualIndex->addKey(objPromote->getFirstKey(), distPromoParent1, objPromote->getFistSubPageId(), objPromote->getFirstCoveringRadius(),
                                objPromote->getSecondKey(), distPromoParent2, objPromote->getSecondSubPageId(), objPromote->getSecondCoveringRadius())) {
                            promote = false;
                        } else {
                            //split index
                            objIndexPromote = splitIndex(se, actualIndex,
                                    objPromote->getFirstKey(), objPromote->getFistSubPageId(), objPromote->getFirstCoveringRadius(),
                                    objPromote->getSecondKey(), objPromote->getSecondSubPageId(), objPromote->getSecondCoveringRadius());
                            promote = true;
                            //clean home
                            delete objPromote;
                            //update objPromote
                            objPromote = objIndexPromote;
                        }//endif

                        cov1 = distPromoParent1 + objPromote->getFirstCoveringRadius();
                        cov2 = distPromoParent2 + objPromote->getSecondCoveringRadius();
                        if (cov1 > cov2) {
                            coveringSub = cov1;
                        } else {
                            coveringSub = cov2;
                        }
                    } else {
                        if (actualIndex->readCoveringRadius(idxSub) < coveringSub) {
                            actualIndex->writeCoveringRadius(idxSub, coveringSub);
                        }
                        coveringSub = actualIndex->readDistanceToParent(idxSub)
                                + actualIndex->readCoveringRadius(idxSub);
                    }
                    pageIdSub = actualIndex->getPageId();

                    averageForAdd->incrementVerification(actualIndex->getCalculatedDistance());
                    //clean home
                    delete actualIndex;
                    //update actual
                    actualIndex = parentIndex;
                    level--;
                }
                //clean home
                delete actualIndex;
            }
            if (promote == true) {
                //promote
                RTreeIndex<K> * newRoot = new RTreeIndex<K>(se->create());
                newRoot->addKey(objPromote->getFirstKey(), DBL_MAX, objPromote->getFistSubPageId(), objPromote->getFirstCoveringRadius());
                newRoot->addKey(objPromote->getSecondKey(), DBL_MAX, objPromote->getSecondSubPageId(), objPromote->getSecondCoveringRadius());
                // Circularly link
                newRoot->writePreviousPageId(newRoot->getPageId());
                newRoot->writeNextPageId(newRoot->getPageId());
                //ajust rootPageId
                descriptor->writeRootPageId(newRoot->getPageId());
                descriptor->incTreeHeight();

                averageForAdd->incrementVerification(newRoot->getCalculatedDistance());
                delete newRoot;
            }
            //clean home
            delete objPromote;
        }
        averageForAdd->incrementVerification(leaf->getCalculatedDistance());
        //clean home
        delete [] path;
        delete leaf;
    }

    //statistic for add
    diskAccess = se->getBlockAccess() - diskAccess;
    averageForAdd->incrementDiskAccess(diskAccess);
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
Workspace * RTree<K>::getWorkspace(){
	return workspace;
}

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
        RTreeIndex<K> * fullIndex,
        K * firstKey, Long firstSubId, Double firstCov,
        K * secondKey, Long secondSubId, Double secondCov) {
    Int i, k1, k2;
    Long nextPageId;
    double dist1, dist2;
    bool failAddKey;
    Int total = fullIndex->readNumberOfKeys() + 2;
    RTreePromotion<K> * objPromote = NULL;
    RTreeIndex<K> * newIndex = new RTreeIndex<K>(se->create());
    RTreeIndex<K> * nextIndex;
    K * objRep1;
    K * objRep2;
    Byte * serialRep1;
    Byte * serialRep2;

    //copy orded of keys and subPageId
    K ** vectKey = new K*[total];
    Long * vectSub = new Long[total + 1];
    double * vectCoverage = new double[total];
    //coping key and subPageId
    for (i = 0; i < total - 2; i++) {
        vectKey[i] = fullIndex->buildKey(i);
        vectSub[i] = fullIndex->readSubPageId(i);
        vectCoverage[i] = fullIndex->readCoveringRadius(i);
    }//endfor
    //copy key, subPageId and coverage
    vectKey[i] = firstKey;
    vectSub[i] = firstSubId;
    vectCoverage[i] = firstCov;
    vectKey[i + 1] = secondKey;
    vectSub[i + 1] = secondSubId;
    vectCoverage[i + 1] = secondCov;

    do {
        k1 = rand() % total;
        do {
            k2 = rand() % total;
        } while (k1 == k2);
        //zering
        fullIndex->clear();
        newIndex->clear();
        //choose promotion
        failAddKey = false;

        //copy representative 1
        objRep1 = new K();
        serialRep1 = new Byte[vectKey[k1]->sizeOfKey()];
        vectKey[k1]->pushKey(serialRep1, 0);
        objRep1->pullKey(serialRep1, 0);
        //copy representative 2
        objRep2 = new K();
        serialRep2 = new Byte[vectKey[k2]->sizeOfKey()];
        vectKey[k2]->pushKey(serialRep2, 0);
        objRep2->pullKey(serialRep2, 0);

        objPromote = new RTreePromotion<K>(objRep1, 0, fullIndex->getPageId(), objRep2, 0, newIndex->getPageId());

        //distributing keys
        for (i = 0; i < total; i++) {
            averageForAdd->incrementVerification(2);
            dist1 = objPromote->getFirstKey()->distanceTo(vectKey[i]);
            dist2 = objPromote->getSecondKey()->distanceTo(vectKey[i]);
            if (dist1 < dist2) {
                if (fullIndex->addKey(vectKey[i], dist1, vectSub[i], vectCoverage[i])) {
                    //adjusting covering radius of the promoted Key1
                    if (objPromote->getFirstCoveringRadius() < (dist1 + vectCoverage[i])) {
                        objPromote->setFirstCoveringRadius(dist1 + vectCoverage[i]);
                    }
                } else {
                    failAddKey = true;
                    break;
                }
            } else {
                if (newIndex->addKey(vectKey[i], dist2, vectSub[i], vectCoverage[i])) {
                    //adjusting covering radius of the promoted Key2
                    if (objPromote->getSecondCoveringRadius() < (dist2 + vectCoverage[i])) {
                        objPromote->setSecondCoveringRadius(dist2 + vectCoverage[i]);
                    }
                } else {
                    failAddKey = true;
                    break;
                }
            }
        }//endfor
        //clean home
        delete [] serialRep1;
        delete [] serialRep2;
        if(failAddKey){
            //deleting objPromote(objRep1, objRep2)
            delete objPromote;
        }
    } while (failAddKey);

    // Circularly linked list
    newIndex->writeNextPageId(fullIndex->readNextPageId());
    fullIndex->writeNextPageId(newIndex->getPageId());
    newIndex->writePreviousPageId(fullIndex->getPageId());
    nextPageId = newIndex->readNextPageId();
    if (nextPageId != fullIndex->getPageId()) {
        nextIndex = new RTreeIndex<K>(se->load(nextPageId));
        nextIndex->writePreviousPageId(newIndex->getPageId());
        averageForAdd->incrementVerification(nextIndex->getCalculatedDistance());
        delete nextIndex;
    } else {
        fullIndex->writePreviousPageId(newIndex->getPageId());
    }
    //ajusting Covering Radius
    objPromote->setFirstCoveringRadius(objPromote->getFirstCoveringRadius() + precisionError);
    objPromote->setSecondCoveringRadius(objPromote->getSecondCoveringRadius() + precisionError);
    //clean home
    for (i = 0; i < total - 2; i++) {
        delete vectKey[i];
    }//endfor
    delete [] vectKey;
    delete [] vectSub;
    delete [] vectCoverage;
    delete newIndex;
    return objPromote;
}//splitIndex

//----------------------------------------------------------------------

template <typename K>
RTreePromotion<K> * RTree<K>::splitLeaf(Session * se,
        RTreeLeaf<K> * fullLeaf, K * key, Uuid * uuid) {

    Int i, k1, k2;
    double dist1, dist2;
    Long nextPageId;
    bool failAddKey;
    Int total = fullLeaf->readNumberOfKeys() + 1;
    RTreePromotion<K> * objPromote = NULL;
    RTreeLeaf<K> * nextLeaf;
    RTreeLeaf<K> * newLeaf = new RTreeLeaf<K>(se->create());
    K * objRep1;
    K * objRep2;
    Byte * serialRep1;
    Byte * serialRep2;
    Long verifications = 0;

    //copy all keys and subPageId
    Uuid ** vectUuid = new Uuid*[total];
    K ** vectKey = new K*[total];
    //coping key and subPageId
    for (i = 0; i < total - 1; i++) {
        vectKey[i] = fullLeaf->buildKey(i);
        vectUuid[i] = fullLeaf->readEntityUuid(i);
    }//endfor
    //copy key and pageId in the correct position
    vectKey[i] = key;
    //copy the pageId in the correct position
    vectUuid[i] = uuid;

    do {
        k1 = rand() % total;
        do {
            k2 = rand() % total;
        } while (k1 == k2);
        //zering
        fullLeaf->clear();
        newLeaf->clear();
        //choose promotion
        failAddKey = false;

        //copy representative 1
        objRep1 = new K();
        serialRep1 = new Byte[vectKey[k1]->sizeOfKey()];
        vectKey[k1]->pushKey(serialRep1, 0);
        objRep1->pullKey(serialRep1, 0);
        //copy representative 2
        objRep2 = new K();
        serialRep2 = new Byte[vectKey[k2]->sizeOfKey()];
        vectKey[k2]->pushKey(serialRep2, 0);
        objRep2->pullKey(serialRep2, 0);

        objPromote = new RTreePromotion<K>(objRep1, 0, fullLeaf->getPageId(), objRep2, 0, newLeaf->getPageId());

        //distributing keys
        for (i = 0; i < total; i++) {
            averageForAdd->incrementVerification(2);
            dist1 = objPromote->getFirstKey()->distanceTo(vectKey[i]);
            dist2 = objPromote->getSecondKey()->distanceTo(vectKey[i]);
            // increment verifications
            verifications += 2;

            if (dist1 < dist2) {
                if (fullLeaf->addKey(vectKey[i], dist1, vectUuid[i])) {
                    if (objPromote->getFirstCoveringRadius() < dist1) {
                        objPromote->setFirstCoveringRadius(dist1);
                    }
                } else {
                    failAddKey = true;
                    break;
                }
            } else {
                if (newLeaf->addKey(vectKey[i], dist2, vectUuid[i])) {
                    if (objPromote->getSecondCoveringRadius() < dist2) {
                        objPromote->setSecondCoveringRadius(dist2);
                    }
                } else {
                    failAddKey = true;
                    break;
                }
            }
        }//endfor
        //clean home
        delete [] serialRep1;
        delete [] serialRep2;
        if(failAddKey){
            //deleting objPromote(objRep1, objRep2)
            delete objPromote;
        }
    } while (failAddKey);

    // Circularly link
    newLeaf->writeNextPageId(fullLeaf->readNextPageId());
    fullLeaf->writeNextPageId(newLeaf->getPageId());
    newLeaf->writePreviousPageId(fullLeaf->getPageId());
    nextPageId = newLeaf->readNextPageId();
    if (nextPageId != fullLeaf->getPageId()) {
        nextLeaf = new RTreeLeaf<K>(se->load(nextPageId));
        nextLeaf->writePreviousPageId(newLeaf->getPageId());
        averageForAdd->incrementVerification(nextLeaf->getCalculatedDistance());
        delete nextLeaf;
    } else {
        fullLeaf->writePreviousPageId(newLeaf->getPageId());
    }
    //ajusting Covering Radius
    objPromote->setFirstCoveringRadius(objPromote->getFirstCoveringRadius() + precisionError);
    objPromote->setSecondCoveringRadius(objPromote->getSecondCoveringRadius() + precisionError);

    averageForAdd->incrementVerification(newLeaf->getCalculatedDistance());

    //clean home
    for (i = 0; i < total - 1; i++) {
        delete vectKey[i];
        delete vectUuid[i];
    }//endfor
    delete [] vectKey;
    delete [] vectUuid;
    delete newLeaf;
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




