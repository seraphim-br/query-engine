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
#ifndef ORG_OBINJECT_BLOCK_MTREEDESCRIPTOR
#define	ORG_OBINJECT_BLOCK_MTREEDESCRIPTOR

#include <br/edu/unifei/tree/AbstractNode.h>
#include <br/edu/unifei/tree/Node.h>

#include <br/edu/unifei/tree/Types.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

class MTreeDescriptor : public AbstractNode{
protected:

    Int getNodeType() {
        return nodeType;
    }//getNodeType

public:
    static const Int nodeType = 1003;

    MTreeDescriptor(Node * node) : AbstractNode(node) {
        initialize(node);
    }//MTreeDescriptor

    virtual ~MTreeDescriptor() {
    }

    void clear() {
        writePreviousPageId(0);
        writeNextPageId(0);
        writeRootPageId(0);
    }//clear

    Long readRootPageId() {
        Int pos = Node::sizeOfHeader();
        return this->readLong(pos);
    }//readRootPageId

    Int readTreeHeight() {
        Int pos = Node::sizeOfHeader() + Node::sizeOfLong;
        return this->readInt(pos);
    }//readTreeHeight

    void writeRootPageId(Long pageId) {
        Int pos = Node::sizeOfHeader();
        this->writeLong(pos, pageId);
    }//writeRootPageId

    void writeTreeHeight(Int height) {
        Int pos = Node::sizeOfHeader() + Node::sizeOfLong;
        this->writeInt(pos, height);
    }//writeTreeHeight

    void incTreeHeight() {
        this->writeTreeHeight(this->readTreeHeight() + 1);
    }

};

//----------------------------------------------------------------------

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif	/*ORG_OBINJECT_BLOCK_MTREEDESCRIPTOR */

