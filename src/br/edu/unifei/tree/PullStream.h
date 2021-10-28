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
#ifndef BR_EDU_UNIFEI_TREE_PULLSTREAM_H
#define	BR_EDU_UNIFEI_TREE_PULLSTREAM_H

#include <br/edu/unifei/tree/Stream.h>
#include <br/edu/unifei/tree/Uuid.h>
#include <list>
#include <string>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

class PullStream : public Stream {
public:

    PullStream(Byte array[], Int position) : Stream::Stream(array, position) {
    }

    virtual ~PullStream() {
    }

    Bool pullBoolean();

    Byte pullByte();

    Char pullChar();

    void pullCollection(list<Uuid*> * collection);

    Double pullDouble();

    Float pullFloat();

    Int pullInt();

    Long pullLong();

    Short pullShort();

    void pullString(string * value);

    void pullUuid(Uuid* uuid);
};

} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */

#endif	/* ORG_OBINJECT_META_PULLSTREAM_H */

