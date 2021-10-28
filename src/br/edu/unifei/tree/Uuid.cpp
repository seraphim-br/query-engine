/*
 * Uuid.cpp
 *
 *  Created on: 24 de set de 2019
 *      Author: windows
 */

#include <br/edu/unifei/tree/Types.h>
#include <br/edu/unifei/tree/Uuid.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace br {
namespace edu {
namespace unifei {
namespace tree {

using namespace std;

static Bool UUID_GENERATOR_RANDOMIZE = true;

//----------------------------------------------------------------------
Uuid::Uuid() {
    if (UUID_GENERATOR_RANDOMIZE == false) {
        UUID_GENERATOR_RANDOMIZE = true;
        srand(time(NULL));
    }
    mostSigBits = ((ULong) rand()) << 32;
    mostSigBits |= ((ULong) rand());
    leastSigBits = ((ULong) rand()) << 32;
    leastSigBits += ((ULong) rand());
}

//----------------------------------------------------------------------
string Uuid::digits(ULong val, Int digits) {
    stringstream ss;
    ULong hi = ((ULong) 1) << (digits * 4);
    ss << setfill('0') << setw(digits + 1) << hex << (hi | (val & (hi - 1)));
    return ss.str().substr(1, digits);
}

//----------------------------------------------------------------------
Int Uuid::compareTo(Uuid* uuid) {
    if (this->mostSigBits == uuid->mostSigBits) {
        if (this->leastSigBits > uuid->leastSigBits)
            return 1;
        else if (this->leastSigBits < uuid->leastSigBits)
            return -1;
        else return 0;
    } else if (this->mostSigBits > uuid->mostSigBits) return 1;
    else return -1;
}

//----------------------------------------------------------------------
void Uuid::fromString(string value) {
	ULong most;
	ULong least;
    UInt val_comp[6];
    //split the components of the UUID
    sscanf(value.c_str(), "%8x-%4x-%4x-%4x-%8x%4x",
            &val_comp[0], &val_comp[1], &val_comp[2], &val_comp[3], &val_comp[4], &val_comp[5]);
    //Most Significant Bits
    most = val_comp[0];
    most <<= 16;
    most |= val_comp[1];
    most <<= 16;
    most |= val_comp[2];
    this->mostSigBits = most;
    //Least Significant Bits
    least = val_comp[3];
    least <<= 32;
    least |= val_comp[4];
    least <<= 16;
    least |= val_comp[5];
    this->leastSigBits = least;
}

//----------------------------------------------------------------------
string Uuid::toString() {
	ULong most = (ULong)this->mostSigBits;
	ULong least = (ULong)this->leastSigBits;
    return ( digits((most >> 32), 8) + "-"
            + digits((most >> 16), 4) + "-"
            + digits((most), 4) + "-"
            + digits((least >> 48), 4) + "-"
            + digits((least >> 16), 8)
            + digits((least), 4));
}


} /* namespace tree */
} /* namespace unifei */
} /* namespace edu */
} /* namespace br */
