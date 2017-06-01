#ifndef __XOBJ_VAL_H__
#define __XOBJ_VAL_H__

#include <iostream>

#include "xobj.h"
#include "xobj_obj.h"

namespace xobj {

struct Value {
    static Value Nil;
    static Value True;
    static Value False;

    template<typename T> inline static
        Value V(T v) { Value val(v); return val; }

    Value() = default;
    ~Value() { decref(); }

    Value(const Value &v) {
        refer(v._obj);
    }
    Value& operator=(const Value &v) {
        refer(v._obj); return *this;
    }

    Value(bool b);
    Value(int64_t i);
    Value(double n);
    Value(char *str, size_t len);
    Value(Object *obj) { refer(obj); }
    Value(int32_t i) : Value((int64_t)i) {}
    Value(float n) : Value((double)n) {}
    Value(char *str) : Value(str, strlen(str)) {}
    Value(const char *str) : Value((char *)str) {}
    Value(const char *str, size_t len) : Value((char *)str, len) {}
    // reference a object
    void refer(Object *o) { decref(); _obj = o; incref(); }
    // Move this object to another value
    void moveto(Value &o) {
        o.decref();
        o._obj = _obj;
        _obj = nullptr;
    }

    inline type_t type() const { return _obj ? _obj->type() : TV_NIL; }

    inline bool isnil() const   { return _obj == nullptr; }
    inline bool isbool() const  { return type() == TV_BOOL; }
    inline bool isstr() const   { return type() == TV_STRING; }
    inline bool islist() const  { return type() == TV_LIST; }
    inline bool istuple() const { return type() == TV_TUPLE; }
    inline bool isdict() const  { return type() == TV_DICT; }
    inline bool isint() const   { return type() == TV_INT; }
    inline bool isfloat() const { return type() == TV_FLOAT; }

    bool operator==(const Value& v) const {
        return isnil() ? v.isnil() : o() == v;
    }
    bool operator!=(const Value& v) const {
        return !(*this == v);
    }

    operator bool() { return isnil() ? false : (bool)o(); }

    inline Object&  o() const { return *_obj; }
    inline List&    l() const { return *(List *)_obj; }
    inline String&  s() const { return *(String *)_obj; }
    inline Dict&    d() const { return *(Dict *)_obj; }
    inline Int&     i() const { return *(Int *)_obj; }
    inline Float&   f() const { return *(Float *)_obj; }
    inline Bool&    b() const { return *(Bool *)_obj; }
    inline Tuple&   t() const { return *(Tuple *)_obj; }
    // get pointer
    template<typename T>
    inline T *p() const {
        return isint() ? (T *)i().val() : nullptr;
    }

    hash_t hash() const {
        return isnil() ? 0 : _obj->hash();
    }

private:
    void incref() { if (!isnil()) _obj->incref(); }          // reference ++
    void decref() { if (!isnil()) _obj->decref(); }          // reference --

private:
    Object  *_obj = nullptr;
};

typedef Value V;

// Return a integer stores a pointer
Value P(void *p);

}

#endif
