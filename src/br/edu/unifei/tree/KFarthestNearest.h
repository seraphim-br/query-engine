/*
 * KFarthest.h
 *
 *  Created on: 1 de out de 2019
 *      Author: windows
 */

#ifndef BR_EDU_UNIFEI_TREE_KFARTHESTNEAREST_H_
#define BR_EDU_UNIFEI_TREE_KFARTHESTNEAREST_H_

#include <br/edu/unifei/tree/AveragePerformance.h>
#include <br/edu/unifei/tree/Session.h>
#include <type_traits>
#include <queue>
#include <list>
#include <ctime>

#include <br/edu/unifei/tree/MTree.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;
using namespace std::chrono;

template <typename K>
class KFarthestNearest {
private:
	class QualifyComparable{
	public:
		bool operator() (Qualify * o1, Qualify * o2) {
			return o1->getDistanceParentToPointQuery() <
					o2->getDistanceParentToPointQuery();
		}
	};

	MTree<K> * mtree;
    AveragePerformance * performance;

public:
	class ReverseComparable{
	public:
		bool operator() (Metric<K> * o1, Metric<K> * o2) {
			return o1->getPreservedDistance() >= o2->getPreservedDistance();
		}
	};

	typedef priority_queue<K *, std::vector<K *>, ReverseComparable> Collection;

	KFarthestNearest(MTree<K> * mtree) {
	    static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Metric class");
		this->mtree = mtree;
	    this->performance = new AveragePerformance();
	}

	virtual ~KFarthestNearest(){
		delete performance;
	}

	AveragePerformance* getPerformance() const {
		return performance;
	}

	Collection * execute(K * object, Int k){
		clock_t start = clock();
		Long pageId;
		double distParentToQuery, covParente, dist, cov;
		Int total;
		double range = 0;
		Qualify * objQualify;
		Session * se = this->mtree->getWorkspace()->openSession();
		Long diskAccess = se->getBlockAccess();
		Long pageIdDescriptor = se->findPageIdDescriptor(MTreeDescriptor::nodeType);
		MTreeDescriptor * descriptor = new MTreeDescriptor(se->load(pageIdDescriptor));

		priority_queue<Qualify *, std::vector<Qualify *>, QualifyComparable> qualifies;
		Collection * result = new Collection();
		list<K *> repetedList;

		qualifies.push(new Qualify(descriptor->readRootPageId(), DBL_MAX, DBL_MAX));

		do {
			objQualify = qualifies.top();
			qualifies.pop();

			pageId = objQualify->getPageId();
			distParentToQuery = objQualify->getDistanceParentToPointQuery();
			covParente = objQualify->getCoverage();

			Node * node = se->load(pageId);
			//verifica qualificação novamente pois o range pode ter mudado
			if (distParentToQuery + covParente >= range) {

				if (MTreeIndex<K>::matchNodeType(node)) {
					MTreeIndex<K> * index = new MTreeIndex<K>(node);
					total = index->readNumberOfKeys();

					for (Int i = 0; i < total; i++) {
						cov = index->readCoveringRadius(i);
						//triangle inequality
						if (distParentToQuery + index->readDistanceToParent(i) >= range - cov) {
							K * build = (K*) index->buildKey(i);
							dist = build->distanceTo(object);
							this->performance->incrementVerification(1);
							if (dist >= range - cov) {
								qualifies.push(new Qualify(index->readSubPageId(i), dist, cov));
							}
							delete build;
						}
					}
					delete index;
				} else {
					MTreeLeaf<K> * leaf = new MTreeLeaf<K>(node);
					total = leaf->readNumberOfKeys();

					for (Int i = 0; i < total; i++) {
						//triangle inequality
						if (distParentToQuery + leaf->readDistanceToParent(i) >= range) {
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
									range = result->top()->getPreservedDistance();
								}
							}else{
								delete build;
							}
						}
					}
					delete leaf;
				}
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

#endif /* BR_EDU_UNIFEI_TREE_KFARTHESTNEAREST_H_ */
