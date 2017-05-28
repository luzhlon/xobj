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

    Value() { _obj = nullptr; }
    Value(const Value &v) { _obj = v._obj; incref(); }
    ~Value() { decref(); }
    Value& operator=(Value &v) {
        decref();   // original reference --
        _obj = v._obj;
        incref();   // new reference ++
        return *this;
    }

    Value(bool b);
    Value(int64_t i);
    Value(double n);
    Value(char *str, size_t len);
    Value(Object *obj) { setobj(obj); }
    Value(int32_t i) : Value((int64_t)i) {}
    Value(float n) : Value((double)n) {}
    Value(char *str) : Value(str, strlen(str)) {}
    Value(const char *str) : Value((char *)str) {}
    Value(const char *str, size_t len) : Value((char *)str, len) {}

    inline type_t type() const { return _obj ? _obj->type() : TV_NIL; }

    inline bool isnil() const   { return _obj == nullptr; }
    inline bool isbool() const  { return type() == TV_BOOL; }
    inline bool isstr() const   { return type() == TV_STRING; }
    inline bool islist() const  { return type() == TV_LIST; }
    inline bool isdict() const  { return type() == TV_DICT; }
    inline bool isint() const   { return type() == TV_INT; }
    inline bool isfloat() const { return type() == TV_FLOAT; }

    int len();
    Value& set(Value &k, Value &v);
    Value  get(Value &k) const;
    void clear();

    Value join(Value &v);
    Value join(const Value &v) { return join((Value &)v); }

    int index(Value &v);
    int count(Value &v);

    bool operator==(Value& v) const { return isnil() ? v.isnil() : o() == v; }
    bool operator!=(Value& v) const { return !(*this == v); }
    Value operator[](Value& v) const { return get(v); }
    Value& operator+=(Value& v);
    Value& operator-=(Value& v);
    // unsafe
    operator bool() { return isnil() ? false : (bool)o(); }
    //operator int();
    //operator float();
    //operator char*();

    Value  get(const Value &k) const { return get((Value &)k); }
    Value& set(const Value &k, const Value &v) { return set((Value &)k, (Value &)v); }
    int index(const Value &v) { return index((Value &)v); }
    int count(const Value &v) { return count((Value &)v); }
    Value& operator=(const Value &v) { return operator=((Value &)v); }
    Value operator[](const Value &v) const { return get(v); }
    Value& operator+=(const Value &v) { return operator+=((Value &)v); }
    Value& operator-=(const Value &v) { return operator-=((Value &)v); }
    bool operator==(const Value &v) const { return operator==((Value &)v); }

    void setobj(Object *o) { decref(); _obj = o; incref(); }
    inline Object&  o() const { return *_obj; }
    inline List&    l() const { return *(List *)_obj; }
    inline String&  s() const { return *(String *)_obj; }
    inline Dict&    d() const { return *(Dict *)_obj; }
    inline Int&     i() const { return *(Int *)_obj; }
    inline Float&   f() const { return *(Float *)_obj; }
    inline Bool&    b() const { return *(Bool *)_obj; }
    inline Tuple&   t() const { return *(Tuple *)_obj; }
    // get pointer
    void *p() const;

    hash_t gethash() { return isnil() ? _obj->hash(): 0; }

private:
    void incref() { if (!isnil()) _obj->incref(); }          // reference ++
    void decref() { if (!isnil()) _obj->decref(); }          // reference --

private:
    Object  *_obj = nullptr;
};

typedef Value V;

// Value operator""_s(const char *str, size_t len);
//template <typename T> inline Value V(T v) { return Value::V(v); }
// Return a integer stores a pointer
Value P(void *p);

std::ostream& operator<<(std::ostream&, const Value&);

}

#endif
