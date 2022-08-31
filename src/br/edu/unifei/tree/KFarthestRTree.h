#ifndef BR_EDU_UNIFEI_TREE_KFARTHESTRTREE_H_
#define BR_EDU_UNIFEI_TREE_KFARTHESTRTREE_H_

#include <br/edu/unifei/tree/AveragePerformance.h>
#include <br/edu/unifei/tree/Qualify.h>
#include <br/edu/unifei/tree/Session.h>
#include <type_traits>
#include <queue>
#include <list>
#include <ctime>
#include <iostream>

#include <br/edu/unifei/tree/RTree.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;
using namespace std::chrono;

template <typename K>
class KFarthestRTree {
private:
	class QualifyComparable{
	public:
		bool operator() (Qualify * o1, Qualify * o2) {
			return o1->getDistanceParentToPointQuery() <
					o2->getDistanceParentToPointQuery();		}
	};

	RTree<K> * rtree;
    AveragePerformance * performance;
	EuclideanGeometry<K> * geometry;

public:
	class ReverseComparable{
	public:
		bool operator() (Metric<K> * o1, Metric<K> * o2) {
			return o1->getPreservedDistance() >= o2->getPreservedDistance();
		}
	};

	typedef priority_queue<K *, std::vector<K *>, ReverseComparable> Collection;

	KFarthestRTree(RTree<K> * rtree) {
	    static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Metric class");
		this->rtree = rtree;
	    this->performance = new AveragePerformance();
	    geometry = new EuclideanGeometry<K>();
	}

	virtual ~KFarthestRTree(){
		delete performance;
	}

	AveragePerformance* getPerformance() const {
		return performance;
	}

	Collection * execute(K * object, Int k){
		clock_t start = clock();
		Long pageId;
		double dist, dist1, dist2, dist3, dist4;
		Int total;
		double range = 0;
		Qualify * objQualify;
		Session * se = this->rtree->getWorkspace()->openSession();
		Long diskAccess = se->getBlockAccess();
		Long pageIdDescriptor = se->findPageIdDescriptor(RTreeDescriptor::nodeType);
		RTreeDescriptor * descriptor = new RTreeDescriptor(se->load(pageIdDescriptor));

		priority_queue<Qualify *, std::vector<Qualify *>, QualifyComparable> qualifies;
		Collection * result = new Collection();
		list<K *> repetedList;

		qualifies.push(new Qualify(descriptor->readRootPageId(), DBL_MAX, DBL_MAX));

		do {
			objQualify = qualifies.top();
			qualifies.pop();

			pageId = objQualify->getPageId();

			Node * node = se->load(pageId);

			if (RTreeIndex<K>::matchNodeType(node)) {
				RTreeIndex<K> * index = new RTreeIndex<K>(node);
				total = index->readNumberOfKeys();

				for (Int i = 0; i < total; i++) {
					K * build1 = (K*) index->buildKey(i);
					K * build2 = (K*) index->buildKey(i);
					build2->setOrigin(0, (build1->getOrigin(0) + build1->getExtension(0)));
					K * build3 = (K*) index->buildKey(i);
					build2->setOrigin(0, (build1->getOrigin(1) + build1->getExtension(1)));
					K * build4 = (K*) index->buildKey(i);
					build2->setOrigin(0, (build1->getOrigin(0) + build1->getExtension(0)));
					build2->setOrigin(0, (build1->getOrigin(1) + build1->getExtension(1)));
					dist1 = build1->distanceTo(object);
					dist2 = build2->distanceTo(object);
					dist3 = build3->distanceTo(object);
					dist4 = build4->distanceTo(object);
					if ((dist1 > dist2) && (dist1 > dist3) && (dist1 > dist4)) {
						dist = dist1;
					} else if ((dist2 > dist3) && (dist2 > dist4)) {
						dist = dist2;
					} else if (dist3 > dist4){
						dist = dist3;
					} else {
						dist = dist4;
					}
					this->performance->incrementVerification(4);
					if (dist >= range) {
						qualifies.push(new Qualify(index->readSubPageId(i), dist, 0));
					}
					delete build1;
					delete build2;
					delete build3;
					delete build4;
				}
				delete index;
			} else {
				RTreeLeaf<K> * leaf = new RTreeLeaf<K>(node);
				total = leaf->readNumberOfKeys();

				for (Int i = 0; i < total; i++) {
					K * build = (K *) leaf->buildKey(i);
					dist = build->distanceTo(object);
					this->performance->incrementVerification(1);
					if (dist >= range) {
						build->setPreservedDistance(dist);
						result->push(build);
						if (result->size() >= (unsigned)k) {
							if (result->size() > (unsigned)k) {
								//remove of result list the upper bounds
								while ((result->size() > 0)
										&& (result->top()->getPreservedDistance() == range)) {
									K * obj = result->top();
									result->pop();
									repetedList.push_back(obj);
								}
								// if list has less than k
								if (result->size() < (unsigned)k) {
									//add list of upper bound
									while (!repetedList.empty()){
										K * obj = repetedList.front();
										repetedList.pop_front();
										result->push(obj);
									}
								}else{
									while (!repetedList.empty()){
										K * obj = repetedList.front();
										repetedList.pop_front();
										delete obj;
									}
								}
							}
							// update range
							range = result->top()->getPreservedDistance();
//							cout << "range = " << range << endl;
						}
					}else{
						delete build;
					}
				}
				delete leaf;
			}
			delete objQualify;
		} while (!qualifies.empty());

		//statistic for add
		diskAccess = se->getBlockAccess() - diskAccess;
		performance->incrementDiskAccess(diskAccess);
		//clean home
		delete descriptor;
		//se->close();
		delete se;
		clock_t end = clock();
		double milli = (double)(end - start) / (CLOCKS_PER_SEC / 1000);
		performance->incrementTime(milli);
		performance->incrementMeasurement();

		return result;
	}

};

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* BR_EDU_UNIFEI_TREE_KFARTHESTRTREE_H_ */
