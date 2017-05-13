#ifndef __XOBJ_OBJ_H__
#define __XOBJ_OBJ_H__

#include <iostream>
#include "xobj_val.h"

namespace xobj {

typedef VALUE_TYPE vtype;

struct Object {
    static Object Nil;

    Object(vtype type = TV_NIL) { init(type); }
    virtual ~Object() = default;

    void init(vtype type = TV_NIL)
    { _type = type, _refs = 0; }
    uint16_t type() const { return _type; }

    inline void refInc() { _refs += 1; }
    inline void refDec() { if (_type > TV_BOOL && !(--_refs)) delete this; }

    virtual bool operator==(Value &v) const {
        return type() == v.type() && v._obj == this;
    }
    virtual uint32_t hash() const { return 0; }

    uint32_t _refs;      // count of references
    vtype    _type;      // type-id of object
    union {
        int16_t  i16;
        uint16_t u16;
        bool     b;
    } _ex;
};

struct Bool: public Object {
    static Bool True;
    static Bool False;

    inline bool val() const { return _ex.b; }
    inline void val(bool b) { _ex.b = b; }

    virtual bool operator==(Value &v) const {
        return v.isbool() && val() == v._bool()->val();
    }
    virtual uint32_t hash() const { return val() ? 1 : 0; }

    Bool(bool v): Object(TV_BOOL) { val(v); }
};

}

#endif /* __XOBJ_OBJ_H__ */
