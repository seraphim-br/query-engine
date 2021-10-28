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
#ifndef BR_EDU_UNIFEI_TREE_HEADERNODE_H
#define	BR_EDU_UNIFEI_TREE_HEADERNODE_H

#include <br/edu/unifei/tree/AbstractNode.h>
#include <cstdlib>

using namespace std;

namespace br {
namespace edu {
namespace unifei {
namespace tree {

class HeaderNode : public AbstractNode {
private:

    Int readDescriptorNodeType(Int idx) {
        Int pos = Node::sizeOfHeader() + HeaderNode::sizeOfFeatures() + (idx * HeaderNode::sizeOfEntry());
        return this->readInt(pos);
    }

    static Int sizeOfEntry() {
        return Node::sizeOfInt + Node::sizeOfLong;
    }

    static Int sizeOfFeatures() {
        return Node::sizeOfInt + Node::sizeOfLong + Node::sizeOfLong + Node::sizeOfInt;
    }

    void writeDescriptorNodeType(Int idx, Int nodeType) {
        Int pos = Node::sizeOfHeader() + HeaderNode::sizeOfFeatures() + (idx * HeaderNode::sizeOfEntry());
        this->writeInt(pos, nodeType);
    }

    void writeDescriptorPageId(Int idx, Long pageId) {
        Int pos = Node::sizeOfHeader() + HeaderNode::sizeOfFeatures() + (idx * HeaderNode::sizeOfEntry()) + Node::sizeOfInt;
        this->writeLong(pos, pageId);
    }

protected:

    Int freeSpace() {
        Int num = this->readNumberOfEntries();
        if (num == 0) {
            return this->sizeOfArray() - Node::sizeOfHeader() - HeaderNode::sizeOfFeatures();
        } else {
            return this->sizeOfArray() - Node::sizeOfHeader()
                    - HeaderNode::sizeOfFeatures() - (num * HeaderNode::sizeOfEntry());
        }//endif
    }

    Int getNodeType() {
        return nodeType;
    }

    void incrementNumberOfKeys() {
        Int num = this->readNumberOfEntries() + 1;
        Int pos = Node::sizeOfHeader() + Node::sizeOfInt + Node::sizeOfLong + Node::sizeOfLong;
        this->writeInt(pos, num);
    }

public:
    static const Int nodeType = 1000;

    HeaderNode(Node *node) : AbstractNode(node) {
        this->initialize(node);
    }

    virtual ~HeaderNode() {
    }

    Bool addEntry(Int nodeType, Long pageId) {
        if (HeaderNode::sizeOfEntry() < this->freeSpace()) {
            Int total = this->readNumberOfEntries();
            this->writeDescriptorNodeType(total, nodeType);
            this->writeDescriptorPageId(total, pageId);
            this->incrementNumberOfKeys();
            return true;
        } else {
            return false;
        }
    }

    /**
     * @see AbstractNode#clear()
     */
    void clear() {
        this->writeSizeOfArray(0);
        this->writeLastPageId(0);
        this->writeLastSessionId(0);
    }

    Int indexOfDescriptorNodeType(Int nodeType) {
        Int total = this->readNumberOfEntries();

        for (Int i = 0; i < total; i++) {
            if (nodeType == this->readDescriptorNodeType(i)) {
                return i;
            }
        }
        return -1;
    }

    Long readDescriptorPageId(Int idx) {
        Int pos = Node::sizeOfHeader() + HeaderNode::sizeOfFeatures() + (idx * HeaderNode::sizeOfEntry()) + Node::sizeOfInt;
        return this->readLong(pos);
    }

    Int readNumberOfEntries() {
        Int pos = Node::sizeOfHeader() + Node::sizeOfInt + Node::sizeOfLong + Node::sizeOfLong;
        return this->readInt(pos);
    }

    Long readLastPageId() {
        Int pos = Node::sizeOfHeader() + Node::sizeOfInt;
        return this->readLong(pos);
    }

    Long readLastSessionId() {
        Int pos = Node::sizeOfHeader() + Node::sizeOfInt + Node::sizeOfLong;
        return this->readLong(pos);
    }

    Int readSizeOfArray() {
        Int pos = Node::sizeOfHeader();
        return this->readInt(pos);
    }

    void writeLastPageId(Long last) {
        Int pos = Node::sizeOfHeader() + Node::sizeOfInt;
        this->writeLong(pos, last);
    }

    void writeLastSessionId(Long last) {
        Int pos = Node::sizeOfHeader() + Node::sizeOfInt + Node::sizeOfLong;
        this->writeLong(pos, last);
    }

    void writeSizeOfArray(Int size) {
        Int pos = Node::sizeOfHeader();
        this->writeInt(pos, size);
    }
};

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif	/* BR_EDU_UNIFEI_TREE_HEADERNODE_H */

