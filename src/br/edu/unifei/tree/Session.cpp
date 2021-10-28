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
#include <br/edu/unifei/tree/Session.h>

#include <br/edu/unifei/tree/Workspace.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

//----------------------------------------------------------------------

Session::Session(Workspace *workspace) {
    blockAccess = 0;
    this->workspace = workspace;
    sessionId = workspace->incrementSessionId();
}

//----------------------------------------------------------------------

Session::~Session() {
    close();
}

//----------------------------------------------------------------------

void Session::close() {
    map<Long, Node*>::iterator it = nodesCache.begin();
    Node *node;
    while (it != nodesCache.end()) {
        node = it->second;
        workspace->flushPage(node);
        it++;
    }
    nodesCache.clear();
}

//----------------------------------------------------------------------

Node *Session::create() {
    Long pageId = workspace->incrementPageId();
    return this->load(pageId);
}

//----------------------------------------------------------------------

Long Session::findPageIdDescriptor(Int nodeType) {
    return workspace->findUniqueDescriptor(nodeType);
}

//----------------------------------------------------------------------

Long Session::getSessionId() {
    return sessionId;
}

//----------------------------------------------------------------------

Node *Session::load(Long id) {
    map<Long, Node*>::iterator itFind = nodesCache.find(id);
    Node * nodeFind;
    //increment BlockAccess
    blockAccess++;
    if (itFind == nodesCache.end()) {
        nodeFind = workspace->loadPage(id);
        nodesCache.insert(std::pair<Long, Node*>(id, nodeFind));
    } else {
        nodeFind = itFind->second;
    }
    return nodeFind;
}

//----------------------------------------------------------------------

Long Session::getBlockAccess() {
    return blockAccess;
}

//----------------------------------------------------------------------

void Session::resetBlockAccess() {
    blockAccess = 0;
}

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */
