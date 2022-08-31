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
#ifndef BR_EDU_UNIFEI_TREE_SEQUENTIAL_H_
#define	BR_EDU_UNIFEI_TREE_SEQUENTIAL_H_

#include <type_traits>
#include <br/edu/unifei/tree/AveragePerformance.h>
#include <br/edu/unifei/tree/SequentialDescriptor.h>
#include <br/edu/unifei/tree/SequentialNode.h>
#include <br/edu/unifei/tree/Session.h>
#include <br/edu/unifei/tree/Types.h>
#include <br/edu/unifei/tree/Workspace.h>
#include <br/edu/unifei/tree/Metric.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

template <typename K>
class Sequential {

private:
	Workspace * workspace;

    AveragePerformance * averageForAdd;
    AveragePerformance * averageForFind;
public:

    Sequential(Workspace * workspace);

    virtual ~Sequential() {
    }

    Bool add(K * entity);

    K * find(Uuid * uuid);

    Uuid * find(K * entity);

    AveragePerformance * getAverageForAdd() {
		return averageForAdd;
	}//getAverageForAdd

	AveragePerformance * getAverageForFind() {
		return averageForFind;
	}//getAverageForFind

	Workspace * getWorkspace();

    Long getRootPageId();

    Bool remove(K *obj) {
        return false;
    }
};

//----------------------------------------------------------------------

template <typename K>
Sequential<K>::Sequential(Workspace * workspace) {
    static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Metric class");
    averageForAdd = new AveragePerformance();
    averageForFind = new AveragePerformance();
    this->workspace = workspace;
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(SequentialDescriptor::nodeType);
    SequentialDescriptor * descriptor = new SequentialDescriptor(se->load(pageIdDescriptor));
    //clean home
    delete descriptor;
    se->close();
    delete se;
}//Sequential

//----------------------------------------------------------------------

template <typename K>
Bool Sequential<K>::add(K * metric) {
    SequentialNode<K> * newNode, * end, * beginNode;

    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(SequentialDescriptor::nodeType);
    SequentialDescriptor * descriptor = new SequentialDescriptor(se->load(pageIdDescriptor));

    if (descriptor->readBeginPageId() == 0) {
        //create SequentialNode
        newNode = new SequentialNode<K>(se->create());
        // Circularly link
        newNode->writePreviousPageId(newNode->getPageId());
        newNode->writeNextPageId(newNode->getPageId());

        descriptor->writeBeginPageId(newNode->getPageId());
        descriptor->writeEndPageId(newNode->getPageId());
        //clean home
        delete newNode;
    }//endif
    end = new SequentialNode<K>(se->load(descriptor->readEndPageId()));

    //adding object in end node
    if (end->addKey(metric) == false) {
        // node is full, creating new SEQUENTIALNODE
        newNode = new SequentialNode<K>(se->create());
        //adding object
        newNode->addKey(metric);
        // Circularly link
        newNode->writeNextPageId(descriptor->readBeginPageId());
        newNode->writePreviousPageId(end->getPageId());
        end->writeNextPageId(newNode->getPageId());
        beginNode = new SequentialNode<K>(se->load(descriptor->readBeginPageId()));
        beginNode->writePreviousPageId(newNode->getPageId());
        //ajust new end node
        descriptor->writeEndPageId(newNode->getPageId());
        //clean home
        delete newNode;
        delete beginNode;
    }
    //clean home
    delete end;
    delete descriptor;
    se->close();
    delete se;
    return true;
}//add

//----------------------------------------------------------------------

template <typename K>
K* Sequential<K>::find(Uuid * uuid) {
    K* entity = NULL;
    SequentialNode<K> * actualSeqNode;
    Session* se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(SequentialDescriptor::nodeType);
    SequentialDescriptor* descriptor = new SequentialDescriptor(se->load(pageIdDescriptor));
    long actualPageId = descriptor->readBeginPageId();
    long firstPageId = actualPageId;

    if (actualPageId != 0) {
        do {
            actualSeqNode = new SequentialNode<K>(se->load(actualPageId));

            entity = actualSeqNode->findUuid(uuid);
            actualPageId = actualSeqNode->readNextPageId();
            //clean home
            delete actualSeqNode;
        } while ((entity == NULL) && (actualPageId != firstPageId));

    }
    //clean home
    delete descriptor;
    se->close();
    delete se;
    return entity;
}//find

//----------------------------------------------------------------------

template <typename K>
Uuid* Sequential<K>::find(K * entity) {
    SequentialNode<K> * actualSeqNode;
    Uuid * uuid = NULL;
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(SequentialDescriptor::nodeType);
    SequentialDescriptor * descriptor = new SequentialDescriptor(se->load(pageIdDescriptor));
    long actualPageId = descriptor->readBeginPageId();
    long firstPageId = actualPageId;

    if (actualPageId != 0) {
        do {
            actualSeqNode = new SequentialNode<K>(se->load(actualPageId));

            uuid = actualSeqNode->findEntity(entity);
            actualPageId = actualSeqNode->readNextPageId();
            //clean home
            delete actualSeqNode;
        } while ((uuid == NULL) && (actualPageId != firstPageId));

    }
    //clean home
    delete descriptor;
    se->close();
    delete se;
    return uuid;
}//find

//----------------------------------------------------------------------

template <typename K>
Workspace * Sequential<K>::getWorkspace(){
	return workspace;
}

//----------------------------------------------------------------------

template <typename K>
Long Sequential<K>::getRootPageId() {
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(SequentialDescriptor::nodeType);
    SequentialDescriptor * descriptor = new SequentialDescriptor(se->load(pageIdDescriptor));
    long rootPageId = descriptor->readBeginPageId();
    //clean home
    delete descriptor;
    se->close();
    delete se;
    return rootPageId;
}//getRootPageId

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif	/* BR_EDU_UNIFEI_TREE_SEQUENTIAL_H_ */

