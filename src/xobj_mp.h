#ifndef __XOBJ_MP_H__
#define __XOBJ_MP_H__

#include <iostream>
#include "xobj_val.h"

namespace xobj {
    void toMsgPack(std::ostream& o, Value& v);
    bool fromMsgPack(std::istream& i, Value &v);
}

#endif /* __XOBJ_MP_H__ */
