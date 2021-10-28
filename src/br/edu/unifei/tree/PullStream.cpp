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
#include <br/edu/unifei/tree/PullStream.h>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

Bool PullStream::pullBoolean() {
    position++;
    return array[position - sizeOfBoolean] == 1;
}

Byte PullStream::pullByte() {
    position++;
    return array[position - sizeOfByte];
}

Char PullStream::pullChar() {
    position++;
    return (Char) array[position - sizeOfByte];
}

void PullStream::pullCollection(list<Uuid*> * collection) {
    Int total = pullInt();
    for (Int i = 0; i < total; i++) {
        Uuid * uuid = new Uuid(0, 0);
        pullUuid(uuid);
        collection->push_back(uuid);
    }
}

Double PullStream::pullDouble() {
    Double* vetDouble = (Double *) (array + position);
    position += Stream::sizeOfDouble;
    return vetDouble[0];
}

Float PullStream::pullFloat() {
    Float* vetFloat = (Float *) (array + position);
    position += Stream::sizeOfFloat;
    return vetFloat[0];
}

Int PullStream::pullInt() {
    Int value = 0;
    value <<= bitsPerByte;
    value += array[position] & mask;
    value <<= bitsPerByte;
    value += array[position + 1] & mask;
    value <<= bitsPerByte;
    value += array[position + 2] & mask;
    value <<= bitsPerByte;
    value += array[position + 3] & mask;
    position += sizeOfInt;
    return value;
}

Long PullStream::pullLong() {
    Long value = 0;
    value <<= bitsPerByte;
    value += array[position] & mask;
    value <<= bitsPerByte;
    value += array[position + 1] & mask;
    value <<= bitsPerByte;
    value += array[position + 2] & mask;
    value <<= bitsPerByte;
    value += array[position + 3] & mask;
    value <<= bitsPerByte;
    value += array[position + 4] & mask;
    value <<= bitsPerByte;
    value += array[position + 5] & mask;
    value <<= bitsPerByte;
    value += array[position + 6] & mask;
    value <<= bitsPerByte;
    value += array[position + 7] & mask;
    position += sizeOfLong;
    return value;
}

Short PullStream::pullShort() {
    Short value = 0;
    value <<= bitsPerByte;
    value += array[position] & mask;
    value <<= bitsPerByte;
    value += array[position + 1] & mask;
    position += sizeOfShort;
    return value;
}

void PullStream::pullString(string * value) {
    Int length = pullInt();
    value->assign(array + position, length);
    position += length;
}

void PullStream::pullUuid(Uuid* uuid) {
    uuid->assign(this->pullLong(), this->pullLong());
}

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */
