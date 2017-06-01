#ifndef __XOBJ_LIST_H__
#define __XOBJ_LIST_H__

#include <iostream>
#include <vector>
#include <algorithm>

#include "xobj_obj.h"
#include "xobj_val.h"

namespace xobj {

using namespace std;

struct List: public Object, public vector<Value> {
    
    static Value New(size_t size = 0);

    int index(const Value& v) const {
        return std::find(begin(), end(), v)-begin();
    }
    int count(const Value& v) const {
        return std::count(begin(), end(), v);
    }
    //void remove(Value& val) { erase(val); }
    inline void push(const Value& val) { push_back(val); }
    inline Value pop() { auto t=back(); pop_back(); return t; }
    inline void del(int i) { erase(begin()+i); }

    inline size_t len() const { return size(); }
    inline Value get(int i) const { return (*this)[i]; }
    void   set(int i, Value& v) { if (i>=size())resize(i+1); (*this)[i] = v; }

    type_t type() const override { return TV_LIST; }
    void release() override { delete this; }
    operator bool() const override { return len() > 0; }
    bool operator==(const Value& v) const override {
        return v.islist() && this == &v.l();
    }

    const Value *valist() const { return &front(); }

private:
    List(size_t size) { reserve(size); }
};

struct Tuple : public Object {
    static Value New(uint16_t n);
    static void Init(Value *l, size_t len);
    static void UnRef(Value *l, size_t len);

    size_t len() const { return _ex.s1; }
    inline Value get(int i) const { return i < len() ? data[i]: Value::Nil; }
    inline void  set(int i, Value& v) { if (i < len()) data[i] = v; }

    type_t type() const override { return TV_TUPLE; }
    bool operator==(const Value& v) const override {
        return v.istuple() && this == &v.t();
    }
    operator bool() const override { return len() > 0; }
    virtual void release() override {
        UnRef(data, len()); delete (char *)this;
    }

private:
    Tuple(uint16_t n) { len(n); Init(data, len()); }
    void len(uint16_t n) { _ex.s1 = n; }

    Value data[0];
};

inline Value _L(List& l) { return Value::V(&l); }
inline Value _T(Tuple& t) { return Value::V(&t); }

template <typename T, typename... Args> inline
Value _L(List& l, T v, Args... rest) {
    l.push(Value::V(v));
    return _L(l, rest...);
}

template <typename T, typename... Args> inline
Value _T(Tuple& t, int i, T v, Args... rest) {
    t.set(i, Value::V(v));
    return _T(t, i+1, rest...);
}

template <typename... Args>
Value L(Args... all) {
    return _L(List::New(sizeof...(all)).l(), all...);
}

template <typename... Args>
Value T(Args... all) {
    return _T(Tuple::New(sizeof...(all)), 0, all...);
}

}

#endif /* __XOBJ_LIST_H__ */
