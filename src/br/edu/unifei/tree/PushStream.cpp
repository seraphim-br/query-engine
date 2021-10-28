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
#include <br/edu/unifei/tree/PushStream.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

void PushStream::pushBoolean(Bool value) {
    position++;
    array[position - sizeOfBoolean] = value ? (Byte) 1 : (Byte) 0;
}

void PushStream::pushByte(Byte value) {
    position++;
    array[position - sizeOfByte] = value;
}

void PushStream::pushChar(Char value) {
    position++;
    array[position - sizeOfByte] = (Byte) value;
}

// TODO Entity passado como template

/*void PushStream::pushCollection(list<Entity*> collection) {
    pushInt(collection.size());
    for (list<Entity*>::iterator it = collection.begin(); it != collection.end(); it++) {
        pushUuid((*it)->getUuid());
    }
}*/

void PushStream::pushDouble(Double value) {
    Double *vetDouble = (Double *) (array + position);
    position += Stream::sizeOfDouble;
    vetDouble[0] = value;
}

void PushStream::pushFloat(Float value) {
    Float *vetFloat = (Float *) (array + position);
    position += Stream::sizeOfFloat;
    vetFloat[0] = value;
}

void PushStream::pushInt(Int value) {
    array[position + sizeOfInt - 1] = (Byte) (value & mask);
    value >>= bitsPerByte;
    array[position + sizeOfInt - 2] = (Byte) (value & mask);
    value >>= bitsPerByte;
    array[position + sizeOfInt - 3] = (Byte) (value & mask);
    value >>= bitsPerByte;
    array[position + sizeOfInt - 4] = (Byte) (value & mask);
    value >>= bitsPerByte;
    position += sizeOfInt;
}

void PushStream::pushLong(Long value) {
    array[position + sizeOfLong - 1] = (Byte) (value & mask);
    value >>= bitsPerByte;
    array[position + sizeOfLong - 2] = (Byte) (value & mask);
    value >>= bitsPerByte;
    array[position + sizeOfLong - 3] = (Byte) (value & mask);
    value >>= bitsPerByte;
    array[position + sizeOfLong - 4] = (Byte) (value & mask);
    value >>= bitsPerByte;
    array[position + sizeOfLong - 5] = (Byte) (value & mask);
    value >>= bitsPerByte;
    array[position + sizeOfLong - 6] = (Byte) (value & mask);
    value >>= bitsPerByte;
    array[position + sizeOfLong - 7] = (Byte) (value & mask);
    value >>= bitsPerByte;
    array[position + sizeOfLong - 8] = (Byte) (value & mask);
    value >>= bitsPerByte;
    position += sizeOfLong;
}

void PushStream::pushShort(Short value) {
    array[position + sizeOfShort - 1] = (Byte) (value & mask);
    value >>= bitsPerByte;
    array[position + sizeOfShort - 2] = (Byte) (value & mask);
    value >>= bitsPerByte;
    position += sizeOfShort;
}

void PushStream::pushString(string *value) {
    const char * str = value->c_str();
    Int length = value->length();
    pushInt(length);
    for (Int i = 0; i < length; i++) {
        array[position + i] = str[i];
    }
    position += length;
}

void PushStream::pushUuid(Uuid* value) {
    this->pushLong(value->getMostSignificantBits());
    this->pushLong(value->getLeastSignificantBits());
}

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */
