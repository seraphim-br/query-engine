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
#ifndef BR_EDU_UNIFEI_TREE_SEQUENTIALDESCRIPTOR_H_
#define	BR_EDU_UNIFEI_TREE_SEQUENTIALDESCRIPTOR_H_

#include <br/edu/unifei/tree/Types.h>
#include <br/edu/unifei/tree/AbstractNode.h>

using namespace std;

namespace br {
namespace edu {
namespace unifei {
namespace tree {

class SequentialDescriptor : public AbstractNode {
protected:

    Int getNodeType() {
        return nodeType;
    }//getNodeType

public:
    static const Int nodeType = 1002;

    SequentialDescriptor(Node * node) : AbstractNode(node) {
        initialize(node);
    }//SequencialDescriptor

    virtual ~SequentialDescriptor() {
    }

    void clear() {
        writePreviousPageId(0);
        writeNextPageId(0);
        writeBeginPageId(0);
        writeEndPageId(0);
    }//clear

    Long readBeginPageId() {
        int pos = AbstractNode::sizeOfHeader();
        return this->readLong(pos);
    }//getBeginNodeID

    Long readEndPageId() {
        int pos = AbstractNode::sizeOfHeader() + AbstractNode::sizeOfLong;
        return this->readLong(pos);
    }//getEndNodeID

    void writeBeginPageId(Long pageId) {
        int pos = AbstractNode::sizeOfHeader();
        this->writeLong(pos, pageId);
    }//setBeginNodeID

    void writeEndPageId(Long pageId) {
        int pos = AbstractNode::sizeOfHeader() + AbstractNode::sizeOfLong;
        this->writeLong(pos, pageId);
    }//setEndNodeID

};

//----------------------------------------------------------------------

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif	/*BR_EDU_UNIFEI_TREE_SEQUENTIALDESCRIPTOR_H_ */

