
/**
 *
 * @author Enzo Seraphim <seraphim@unifei.edu.br>
 * @author Carlos Ferro <carlosferro@gmail.com>
 * @author Thatyana de Faria Piola Seraphim <thatyana@unifei.edu.br>
 * @param <M>
 */

#ifndef BR_EDU_UNIFEI_TREE_KFARTHESTSEQUENTIAL_H_
#define BR_EDU_UNIFEI_TREE_KFARTHESTSEQUENTIAL_H_

#include <br/edu/unifei/tree/AveragePerformance.h>
#include <br/edu/unifei/tree/Qualify.h>
#include <br/edu/unifei/tree/Session.h>
#include <type_traits>
#include <queue>
#include <list>
#include <ctime>
#include <iostream>

#include <br/edu/unifei/tree/Sequential.h>
#include <br/edu/unifei/tree/SequentialDescriptor.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

template <typename K>
class KFarthestSequential {
private:

	Sequential<K> * sequential;
	AveragePerformance * performance;

public:
	class ReverseComparable{
	public:
		bool operator() (Metric<K> * o1, Metric<K> * o2) {
			return o1->getPreservedDistance() >= o2->getPreservedDistance();
		}
	};

	typedef priority_queue<K *, std::vector<K *>, ReverseComparable> Collection;

	KFarthestSequential(Sequential<K> * sequential) {
	    static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Metric class");
		this->sequential = sequential;
	    this->performance = new AveragePerformance();
	}

	virtual ~KFarthestSequential(){
		delete performance;
	}

	AveragePerformance* getPerformance() const {
		return performance;
	}

	Collection * execute(K * object, Int k){
		clock_t start = clock();
		double dist;
		Int total;
		double range = 0;
		Session * se = this->sequential->getWorkspace()->openSession();

		Long diskAccess = se->getBlockAccess();
		Long pageIdDescriptor = se->findPageIdDescriptor(SequentialDescriptor::nodeType);
		SequentialDescriptor * descriptor = new SequentialDescriptor(se->load(pageIdDescriptor));

		Collection * result = new Collection();
		list<K *> repetedList;
		Long actualPageId = descriptor->readBeginPageId();
		Long rootPageId = descriptor->readBeginPageId();
		do {
			SequentialNode<K> * actualSeq = new SequentialNode<K>(se->load(actualPageId));

			total = actualSeq->readNumberOfEntities();

			for (Int i = 0; i < total; i++) {
				K * build = (K *) actualSeq->buildKey(i);
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
					}
				}else{
					delete build;
				}
			}

			actualPageId = actualSeq->readNextPageId();

			delete actualSeq;
		} while ((actualPageId != rootPageId));
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

#endif /* BR_EDU_UNIFEI_TREE_KFARTHESTSEQUENTIAL_H_ */
