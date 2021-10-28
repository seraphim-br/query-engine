/*
 * Qualify.h
 *
 *  Created on: 1 de out de 2019
 *      Author: windows
 */

#ifndef INCLUDE_BR_EDU_UNIFEI_TREE_QUALIFY_H_
#define INCLUDE_BR_EDU_UNIFEI_TREE_QUALIFY_H_

#include <br/edu/unifei/tree/Types.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

class Qualify {
private:
	const Long pageId;
	const Double distanceParentToPointQuery;
	const Double coverage;

public:

	Qualify(Long pageId, Double distanceParentToPointQuery, Double coverage) :
			pageId(pageId), distanceParentToPointQuery(distanceParentToPointQuery),
			coverage(coverage){

	}

	Qualify(Long pageId, Double distanceParentToPointQuery) :
			pageId(pageId), distanceParentToPointQuery(distanceParentToPointQuery), coverage(0) {

	}

	virtual ~Qualify(){

	}

	Long getPageId() const {
		return pageId;
	}

	Double getDistanceParentToPointQuery() const {
		return distanceParentToPointQuery;
	}

	Double getCoverage() const {
		return coverage;
	}

};

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* INCLUDE_BR_EDU_UNIFEI_TREE_QUALIFY_H_ */
