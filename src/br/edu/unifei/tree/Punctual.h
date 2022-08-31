/*
 * Punctual.h
 *
 *  Created on: 4 de out de 2019
 *      Author: windows
 */

#ifndef BR_EDU_UNIFEI_TREE_PUNCTUAL_H_
#define BR_EDU_UNIFEI_TREE_PUNCTUAL_H_

#include <br/edu/unifei/tree/AveragePerformance.h>
#include <br/edu/unifei/tree/Qualify.h>
#include <br/edu/unifei/tree/Session.h>
#include <br/edu/unifei/tree/Uuid.h>
#include <type_traits>
#include <queue>
#include <list>

#include <br/edu/unifei/tree/MTree.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;
using namespace std::chrono;

template<typename K>
class Punctual {
private:
	class QualifyComparable {
	public:
		bool operator()(Qualify *o1, Qualify *o2) {
			return o1->getDistanceParentToPointQuery()
					> o2->getDistanceParentToPointQuery();
		}
	};

	MTree<K> *mtree;
	AveragePerformance *performance;
public:

	Punctual(MTree<K> *mtree) {
		static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Metric class");
		this->mtree = mtree;
		this->performance = new AveragePerformance();
	}

	virtual ~Punctual() {
		delete performance;
	}

	Uuid* execute(K *object) {
	    steady_clock::time_point t1 = steady_clock::now();
	    Session * se = this->mtree->getWorkspace()->openSession();
	    Long diskAccess = se->getBlockAccess();
	    Long pageIdDescriptor = se->findPageIdDescriptor(MTreeDescriptor::nodeType);
	    MTreeDescriptor * descriptor = new MTreeDescriptor(se->load(pageIdDescriptor));
	    Int total, idxOverlap;
	    double distParentToQuery, dist, cov;
	    Qualify * objQualify;
	    K * storedKey;
	    Uuid * uuidFinded = NULL;

	    priority_queue<Qualify *, std::vector<Qualify *>, QualifyComparable> qualifies;
	    qualifies.push(new Qualify(descriptor->readRootPageId(), DBL_MAX));
	    do {
	        objQualify = qualifies.top();
	        qualifies.pop();

	        distParentToQuery = objQualify->getDistanceParentToPointQuery();

	        Node * node = se->load(objQualify->getPageId());

	        if (MTreeIndex<K>::matchNodeType(node)) {
	            MTreeIndex<K> * index = new MTreeIndex<K>(node);
	            total = index->readNumberOfKeys();
	            idxOverlap = 0;

	            for (Int i = 0; i < total; i++) {
	                cov = index->readCoveringRadius(i);

	                //triangle inequality
	                if (distParentToQuery - index->readDistanceToParent(i) <= cov) {
	                    storedKey = index->buildKey(i); // Key stored in the node->

	                    performance->incrementVerification(1);
	                    dist = storedKey->distanceTo(object); // Distance from key to storedKey: d(Or, On)

	                    if (dist <= cov) {
	                        qualifies.push(new Qualify(index->readSubPageId(i), dist));
	                        idxOverlap++;
	                    }
	                    //clean home
	                    delete storedKey;
	                }
	            }

	            performance->incrementVerification(index->getCalculatedDistance());
	            //clean home
	            delete index;
	        } else {
	            MTreeLeaf<K> * leaf = new MTreeLeaf<K>(node);

	            total = leaf->readNumberOfKeys();

	            for (Int i = 0; i < total; i++) {
	                //triangle inequality
	                if (leaf->readDistanceToParent(i) == distParentToQuery) {
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

	            }

	            performance->incrementVerification(leaf->getCalculatedDistance());
	            //clean home
	            delete leaf;
	        }
	        //clean home
	        delete objQualify;
	        //free each block after use
	        se->close();
	    } while ((!qualifies.empty()) &&(uuidFinded == NULL));
	    //clean home
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

#endif /* BR_EDU_UNIFEI_TREE_PUNCTUAL_H_ */
