#ifndef __XOBJ_NUM_H__
#define __XOBJ_NUM_H__

#include <iostream>
#include "xobj_obj.h"

namespace xobj {

struct Number: public Object {
    Number(int64_t i): Object(TV_NUMBER) { isint(true); this->i = i; }
    Number(double f): Object(TV_NUMBER) { isint(false); this->f = f; }
    virtual ~Number() = default;


    virtual bool operator==(Value &v) const {
        return v.isint() ?
            (isint() && i == v.num()->i) : (!isint() && f == v.num()->f);
    }
    virtual uint32_t _hash() const { return i; }

    inline bool isint() const { return _ex.b; }
    inline void isint(bool b) { _ex.b = b; }

    friend std::ostream& operator<<(std::ostream& o, const Number& n) {
        if (n.isint())
            o << n.i;
        else
            o << n.f;
        return o;
    }

    union {
        int64_t i;
        double  f;
    };
};

}

#endif /* __XOBJ_NUM_H__ */
