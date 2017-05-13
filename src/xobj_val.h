#ifndef __XOBJ_VAL_H__
#define __XOBJ_VAL_H__

#include <stdint.h>
#include <string.h>
#include <sstream>
#include <iostream>

#ifdef XOBJ_USE_PY

#include <Python.h>

#define DECLARE_TOPYOBJ \
    PyObject *toPyObj();
#else
#define DECLARE_TOPYOBJ
#endif

namespace xobj {

struct Object;
struct Number;
struct Bool;
struct String;
struct List;
struct Dict;

enum VALUE_TYPE {
    TV_NIL = 0, TV_BOOL,
    TV_NUMBER, TV_STRING,
    TV_LIST, TV_DICT
};

struct Value {
    static Value Nil;
    static Value True;
    static Value False;

    template<typename T> inline static
        Value V(T v) { Value val(v); return val; }

    Value() = default;
    Value(const Value &v) { _obj = v._obj; _inc_ref(); }
    ~Value() { _dec_ref(); }
    Value& operator=(Value &v) {
        _dec_ref();   // original reference --
        _obj = v._obj;
        _inc_ref();   // new reference ++
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

    void init() { _obj = nullptr; }

    inline VALUE_TYPE type() const;

    inline bool isnil() const   { return type() == TV_NIL; }
    inline bool isbool() const  { return type() == TV_BOOL; }
    inline bool isstr() const   { return type() == TV_STRING; }
    inline bool islist() const  { return type() == TV_LIST; }
    inline bool isdict() const  { return type() == TV_DICT; }
    inline bool isnum() const   { return type() == TV_NUMBER; }
    inline bool isint() const;
    inline bool isfloat() const;

    int len();
    Value& set(Value &k, Value &v);
    Value  get(Value &k) const;
    void clear();

    Value join(Value &v);
    Value join(const Value &v) { return join((Value &)v); }

    int index(Value &v);
    int count(Value &v);

    bool operator==(Value& v) const;
    bool operator!=(Value& v) const { return !(*this == v); }
    Value operator[](Value& v) const { return get(v); }
    Value& operator+=(Value& v);
    Value& operator-=(Value& v);
    // unsafe
    operator bool();
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

    DECLARE_TOPYOBJ;

    void setobj(Object *o) { _dec_ref(); _obj = o; _inc_ref(); }
    inline List    *list() const { return (List *)_obj; }
    inline String  *str() const { return (String *)_obj; }
    inline Dict    *dict() const { return (Dict *)_obj; }
    inline Number  *num() const { return (Number *)_obj; }
    inline Bool    *_bool() const { return (Bool *)_obj; }

    uint32_t _hash() const;

private:
    void _inc_ref();          // reference ++
    void _dec_ref();          // reference --

public:
    Object  *_obj = nullptr;
};

// Value operator""_s(const char *str, size_t len);
template <typename T> inline Value V(T v) { return Value::V(v); }

std::ostream& operator<<(std::ostream&, const Value&);

}

#endif
