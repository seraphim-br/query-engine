/*
 * Workspace.h
 *
 *  Created on: 3 de out de 2019
 *      Author: windows
 */

#ifndef INCLUDE_BR_EDU_UNIFEI_TREE_WORKSPACE_H_
#define INCLUDE_BR_EDU_UNIFEI_TREE_WORKSPACE_H_

#include <br/edu/unifei/tree/HeaderNode.h>
#include <br/edu/unifei/tree/Types.h>
#include <string>


namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

class Session;

class Workspace {
private:

    static const Long pageIdDescriptor = 0;

    string name;

    Long newPageId(HeaderNode *header);

protected:

    Int sizeArray;

    Workspace(string name);

    void initialize();

    void createHeaderNode();

    virtual Bool existWorkspace() = 0;

    virtual void loadWorkspace() = 0;

    virtual void createWorkspace() = 0;

public:

    Workspace(string name, Int sizeArray);

    virtual ~Workspace(){
    }

    Long findUniqueDescriptor(Int nodeType);

    const string getName();

    Long incrementPageId();

    Long incrementSessionId();

    Int sizeOfArray();

    Session *openSession();

    virtual Bool deletePage(Long id) = 0;

    virtual Bool flushPage(Node *node) = 0;

    virtual Node *loadPage(Long id) = 0;

};

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* INCLUDE_BR_EDU_UNIFEI_TREE_WORKSPACE_H_ */
