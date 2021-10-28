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
#ifndef BR_EDU_UNIFEI_TREE_STREAM_H
#define	BR_EDU_UNIFEI_TREE_STREAM_H

#include <br/edu/unifei/tree/Types.h>
#include <string>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

class Stream {
protected:
    Byte *array;
    Int position;
    static const Int bitsPerByte = 8;
    static const Int mask = 0xff;

public:
    static const Int sizeOfBoolean = 1;
    static const Int sizeOfByte = 1;
    static const Int sizeOfDouble = 8;
    static const Int sizeOfFloat = 4;
    static const Int sizeOfInt = 4;
    static const Int sizeOfLong = 8;
    static const Int sizeOfShort = 2;
    static const Int sizeOfUuid = 16;

    Stream(Byte array[], Int position) {
        this->position = position;
        this->array = array;
    }

    virtual ~Stream() {
    }

    static Int sizeOfString(string *str) {
        return sizeOfInt + str->length();
    }
};

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif	/* BR_EDU_UNIFEI_TREE_STREAM_H */

