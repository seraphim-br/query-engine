/*
 * Node.h
 *
 *  Created on: 24 de set de 2019
 *      Author: windows
 */

#ifndef INCLUDE_BR_EDU_UNIFEI_TREE_NODE_H_
#define INCLUDE_BR_EDU_UNIFEI_TREE_NODE_H_

#include <br/edu/unifei/tree/Types.h>
#include <br/edu/unifei/tree/Uuid.h>

#include <cstddef>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

class Node {
private:
    Long pageId;
    Byte *array;
    Int size;

    void moveLeft(Int dest, Int source, Int length);

    void moveRight(Int dest, Int source, Int length);

protected:

    Bool readBoolean(Int pos) {
        return (array[pos] == 1);
    }

    Byte readByte(Int pos) {
        return array[pos];
    }

    Double readDouble(Int pos) {
        Double *buff = (Double *) (array + pos);
        return buff[0];
    }

    Float readFloat(Int pos) {
        Float *buff = (Float *) (array + pos);
        return buff[0];
    }

    Int readInt(Int pos) {
        Int *buff = (Int *) (array + pos);
        return buff[0];
    }

    Long readLong(Int pos) {
        Long *buff = (Long *) (array + pos);
        return buff[0];
    }

    Short readShort(Int pos) {
        Short * buff = (Short *) (array + pos);
        return buff[0];
    }

    Uuid *readUuid(Int pos) {
        Long most = this->readLong(pos);
        Long least = this->readLong(pos + sizeOfLong);
        return new Uuid(most, least);
    }

    void writeNodeType(Int nodeType) {
        this->writeInt(0, nodeType);
    }

    void writeBoolean(Int pos, bool value) {
        array[pos] = value ? (Byte) 1 : (Byte) 0;
    }

    void writeByte(Int pos, Byte value) {
        array[pos] = value;
    }

    void writeDouble(Int pos, double value) {
        double * buff = (double *) (array + pos);
        buff[0] = value;
    }

    void writeFloat(Int pos, Float value) {
        Float * buff = (Float *) (array + pos);
        buff[0] = value;
    }

    void writeInt(Int pos, Int value) {
        Int * buff = (Int *) (array + pos);
        buff[0] = value;
    }

    void writeLong(Int pos, Long value) {
        Long * buff = (Long *) (array + pos);
        buff[0] = value;
    }

    void writeShort(Int pos, Int value) {
        Short * buff = (Short *) (array + pos);
        buff[0] = value;
    }

    void writeUuid(Int pos, Uuid *value);

public:
    static const Int bitsPerByte = 8;
    static const Int mask = 0xff;
    static const Int sizeOfByte = 1;
    static const Int sizeOfDouble = 8;
    static const Int sizeOfFloat = 4;
    static const Int sizeOfInt = 4;
    static const Int sizeOfLong = 8;
    static const Int sizeOfShort = 2;
    static const Int sizeOfUuid = 16;

    Node(Long Id, Byte *buff, Int sizeBuff) :
        pageId(Id), array (buff), size(sizeBuff) {
    }

    virtual ~Node() {
    }

    void destroy() {
        if (array != NULL) {
            delete [] array;
        }
    }

    Byte* getArray() {
        return array;
    }

    Int getSize() {
        return size;
    }

    Long readNextPageId() {
        Int pos = sizeOfInt + sizeOfLong;
        return readLong(pos);
    }

    Int readNodeType() {
        return readInt(0);
    }

    Long getPageId() {
        return this->pageId;
    }

    Long readPreviousPageId() {
        Int pos = sizeOfInt;
        return readLong(pos);
    }

    void move(Int dest, Int source, Int length) {
        if (dest > source) {
            this->moveRight(dest, source, length);
        } else {
            this->moveLeft(dest, source, length);
        }
    }

    void writeNextPageId(Long pageId) {
        Int pos = sizeOfInt + sizeOfLong;
        this->writeLong(pos, pageId);
    }

    void writePreviousPageId(Long pageId) {
        Int pos = sizeOfInt;
        this->writeLong(pos, pageId);
    }

    Int sizeOfArray() {
        return this->size;
    }

    static Int sizeOfHeader() {
        // node type + previous page Id + next page Id
        return sizeOfInt + sizeOfLong + sizeOfLong;
    }

};

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif /* INCLUDE_BR_EDU_UNIFEI_TREE_NODE_H_ */
