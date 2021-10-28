/*
 * Workspace.cpp
 *
 *  Created on: 3 de out de 2019
 *      Author: windows
 */

#include <br/edu/unifei/tree/Session.h>
#include <br/edu/unifei/tree/Workspace.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

//----------------------------------------------------------------------

Workspace::Workspace(string name) {
    this->sizeArray = 4096;
    this->name = name;
}//Workspace

//----------------------------------------------------------------------

Workspace::Workspace(string name, Int sizeArray) {
    this->sizeArray = sizeArray;
    this->name = name;
}//Workspace

//----------------------------------------------------------------------

void Workspace::createHeaderNode() {
    Byte *array = new Byte[sizeArray];
    for (Int i = 0; i<this->sizeArray; i++) {
        array[i] = 0;
    }
    Node * node = new Node(0, array, sizeArray);
    HeaderNode *headerNode = new HeaderNode(node);
    headerNode->writeSizeOfArray(sizeArray);
    headerNode->writeLastPageId(0);
    headerNode->writeLastSessionId(0);
    headerNode->writePreviousPageId(0);
    headerNode->writeNextPageId(0);
    //storing and clean home
    this->flushPage(headerNode);
    delete node;
}//createHeaderNode

//----------------------------------------------------------------------

void Workspace::initialize() {
    if (existWorkspace()) {
        loadWorkspace();
    } else {
        createWorkspace();
        createHeaderNode();
    }
}//initialize

//----------------------------------------------------------------------

Long Workspace::findUniqueDescriptor(Int nodeType) {
    Long pageId;
    Node * node = this->loadPage(pageIdDescriptor);
    HeaderNode * headerNode = new HeaderNode(node);
    Int idx = headerNode->indexOfDescriptorNodeType(nodeType);
    if (idx == -1) {
        pageId = newPageId(headerNode);
        headerNode->addEntry(nodeType, pageId);
    } else {
        pageId = headerNode->readDescriptorPageId(idx);
    }
    //clean home
    this->flushPage(headerNode);
    delete node;
    return pageId;
}//findUniqueDescriptor

//----------------------------------------------------------------------

const string Workspace::getName() {
    return name;
}//getName

//----------------------------------------------------------------------

Long Workspace::incrementPageId() {
    //adjusting LastPageId
    Node * node = this->loadPage(pageIdDescriptor);
    HeaderNode * headerNode = new HeaderNode(node);
    long pageId = newPageId(headerNode);
    this->flushPage(headerNode);
    delete node;
    return pageId;
}//incrementPageId

//----------------------------------------------------------------------

Long Workspace::incrementSessionId() {
    Node * node = this->loadPage(pageIdDescriptor);
    HeaderNode *headerNode = new HeaderNode(node);
    long inc = headerNode->readLastSessionId() + 1;
    headerNode->writeLastSessionId(inc);
    this->flushPage(headerNode);
    delete node;
    return inc;
}//incrementSessionId

//----------------------------------------------------------------------

Long Workspace::newPageId(HeaderNode *header) {
    Long pageId = header->readLastPageId() + 1;
    header->writeLastPageId(pageId);
    //creating node
    Byte *array = new Byte[this->sizeOfArray()];
    for (int i = 0; i<this->sizeArray; i++) {
        array[i] = 0;
    }
    Node *node = new Node(pageId, array, this->sizeOfArray());
    this->flushPage(node);
    return pageId;
}//newPageId

//----------------------------------------------------------------------

Session * Workspace::openSession() {
    return new Session(this);
}//openSession

//----------------------------------------------------------------------

Int Workspace::sizeOfArray() {
    return sizeArray;
}//sizeOfArray

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */
