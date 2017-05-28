#ifndef __XOBJ_MP_H__
#define __XOBJ_MP_H__

#include <iostream>
#include <sstream>
#include "xobj_val.h"

namespace xobj {
    using namespace std;
    bool mp_pack(Value& v, ostream& o);
    bool mp_unpack(Value &v, istream& i);
}

#endif /* __XOBJ_MP_H__ */
