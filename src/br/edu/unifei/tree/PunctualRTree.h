/*
 * PunctualRTree.h
 *
 *  Created on: 4 de out de 2019
 *      Author: windows
 */

#ifndef BR_EDU_UNIFEI_TREE_PUNCTUALRTREE_H_
#define BR_EDU_UNIFEI_TREE_PUNCTUALRTREE_H_

#include <br/edu/unifei/tree/AveragePerformance.h>
#include <br/edu/unifei/tree/Qualify.h>
#include <br/edu/unifei/tree/Session.h>
#include <br/edu/unifei/tree/Uuid.h>
#include <type_traits>
#include <queue>
#include <list>

#include <br/edu/unifei/tree/RTree.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;
using namespace std::chrono;

template<typename K>
class PunctualRTree {
private:
	class QualifyComparable {
	public:
		bool operator()(Qualify *o1, Qualify *o2) {
			return o1->getDistanceParentToPointQuery()
					> o2->getDistanceParentToPointQuery();
		}
	};

	EuclideanGeometry<K> * geometry;
	RTree<K> *rtree;
	AveragePerformance *performance;

public:

	PunctualRTree(RTree<K> *rtree) {
		static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Metric class");
		this->rtree = rtree;
		this->performance = new AveragePerformance();
	    geometry = new EuclideanGeometry<K>();
	}

	virtual ~PunctualRTree() {
		delete performance;
		delete geometry;
	}

	Uuid* execute(K *object) {
	    steady_clock::time_point t1 = steady_clock::now();
	    Session * se = this->rtree->getWorkspace()->openSession();
	    Long diskAccess = se->getBlockAccess();
	    Long pageIdDescriptor = se->findPageIdDescriptor(RTreeDescriptor::nodeType);
	    RTreeDescriptor * descriptor = new RTreeDescriptor(se->load(pageIdDescriptor));
	    Int total;
	    double dist = 0;
	    K * storedKey;
	    Qualify * objQualify;
	    Uuid * uuidFinded = NULL;

	    priority_queue<Qualify *, std::vector<Qualify *>, QualifyComparable> qualifies;
	    qualifies.push(new Qualify(descriptor->readRootPageId(), DBL_MAX));
	    do {
	        objQualify = qualifies.top();
	        qualifies.pop();


	        Node * node = se->load(objQualify->getPageId());

	        if (RTreeIndex<K>::matchNodeType(node)) {
	            RTreeIndex<K> * index = new RTreeIndex<K>(node);
	            total = index->readNumberOfKeys();

	            for (Int i = 0; i < total; i++) {
                    storedKey = index->buildKey(i); // Key stored in the node->
					performance->incrementVerification(1);

					if (geometry->isOverlap(storedKey, object)){
						qualifies.push(new Qualify(index->readSubPageId(i), dist));
					}
					//clean home
					delete storedKey;
	            }
	            //clean home
	            delete index;
	        } else {
	            RTreeLeaf<K> * leaf = new RTreeLeaf<K>(node);

	            total = leaf->readNumberOfKeys();

	            for (Int i = 0; i < total; i++) {
					storedKey = leaf->buildKey(i);
					performance->incrementVerification(2);
					dist = object->distanceTo(storedKey);
					if (dist == 0) {
						uuidFinded = leaf->readEntityUuid(i);
						//clean home
						delete storedKey;
						break;
					}
					//clean home
					delete storedKey;

	            }
	            //clean home
	            delete leaf;
	        }
	        //clean home
	        delete objQualify;
	        //free each block after use
	        se->close();
	    } while ((!qualifies.empty()) &&(uuidFinded == NULL));
	    while(!qualifies.empty()){
	        objQualify = qualifies.top();
	        qualifies.pop();
	        delete objQualify;
	    }

	    //statistic for add
	    diskAccess = se->getBlockAccess() - diskAccess;
	    performance->incrementDiskAccess(diskAccess);
	    //clean home
	    delete descriptor;
	    //se->close();
	    delete se;
	    duration<double> time_span = duration_cast<duration<double>>(steady_clock::now() - t1);
	    performance->incrementTime(time_span.count()*1000000);
	    performance->incrementMeasurement();

	    return uuidFinded;
	}
};

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_PUNCTUALRTREE_H_ */
