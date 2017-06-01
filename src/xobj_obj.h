#ifndef __XOBJ_OBJ_H__
#define __XOBJ_OBJ_H__

#include <iostream>

#include "xobj.h"

namespace xobj {

struct Object {
    static Object Nil;

protected:
    friend class Value;

    virtual void release() = 0;
    virtual bool operator==(const Value& v) const = 0;
    virtual hash_t hash() const { return (hash_t)this; }
    virtual type_t type() const = 0;
    virtual operator bool() const = 0;

private:
    friend class Value;

    inline void incref() {
        _refs ++;
    }
    inline void decref() {
        if (!--_refs)
            release();
    }

protected:
    union EX {
        struct { bool b1, b2; };
        struct { uint16_t s1, s2; };
        uint32_t u32;
    } _ex;

private:
    uint32_t _refs = 0;      // count of references
};

struct Bool: public Object {
    static Bool True;
    static Bool False;

    inline bool val() const { return _ex.b1; }
    inline void val(bool b) { _ex.b1 = b; }

    type_t type() const override { return TV_BOOL; }
    hash_t hash() const override { return val() ? 1 : 0; }
    void release() override {}
    bool operator==(const Value& v) const override;
    operator bool() const override { return val(); }

    Bool(bool v) { val(v); }
};

}

#endif /* __XOBJ_OBJ_H__ */
