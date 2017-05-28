#ifndef __XOBJ_LIST_H__
#define __XOBJ_LIST_H__

#include <iostream>
#include <vector>
#include <algorithm>

#include "xobj_obj.h"
#include "xobj_val.h"

namespace xobj {

struct List: public Object, public std::vector<Value> {
    List(size_t size = 0) { reserve(size); }
    ~List() = default;

    int index(Value &v) const { return std::find(begin(), end(), v)-begin(); }
    int index(const Value &v) const { return index((Value &)v); }
    int count(Value &v) const { return std::count(begin(), end(), v); }
    int count(const Value &v) const { return count((Value &)v); }
    void append(Value &val) { push_back(val); }
    void append(const Value &val) { return append((Value &)val); }
    void del(int i) { erase(begin()+i); }
    //void remove(Value &val) { erase(val); }
    inline void push(Value &val) { append(val); }
    inline void push(const Value &val) { append(val); }
    inline Value pop() { auto t=back(); pop_back(); return t; }

    inline size_t len() const { return size(); }
    inline Value get(int i) const { return (*this)[i]; }
    void   set(int i, Value &v) { if (i>=size())resize(i+1); (*this)[i] = v; }

    type_t type() const override { return TV_LIST; }
    void release() override { delete this; }
    operator bool() const override { return len() > 0; }
    bool operator==(Value& v) const override {
        return v.islist() && this == &v.l();
    }

    friend std::ostream& operator<<(std::ostream& o, const List& l) {
        o << '[';
        auto len = l.len();
        auto p = l.valist();
        if (len) {
            int i = 0;
            o << p[i++];
            while (i < len)
                o << ',' << p[i++];
        }
        o << ']';
        return o;
    }

    const Value *valist() const { return &front(); }
};

struct Tuple : public Object {
    static Tuple *New(uint16_t n);
    static void Init(Value *l, size_t len);
    static void UnRef(Value *l, size_t len);

    size_t len() const { return _ex.s1; }
    inline Value get(int i) const { return i < len() ? data[i]: Value::Nil; }
    inline void  set(int i, Value &v) { if (i < len()) data[i] = v; }

    type_t type() const override { return TV_TUPLE; }
    operator bool() const override { return len() > 0; }
    virtual void release() override {
        UnRef(data, len()); delete (char *)this;
    }

private:
    Tuple(uint16_t n) { len(n); Init(data, len()); }
    void len(uint16_t n) { _ex.s1 = n; }

    Value data[0];
};

inline Value _L(List *l) { return Value::V(l); }
inline Value _T(Tuple *t) { return Value::V(t); }

template <typename T, typename... Args> inline
Value _L(List *l, T v, Args... rest) { l->push(Value::V(v)); return _L(l, rest...); }

template <typename T, typename... Args> inline
Value _T(Tuple *t, int i, T v, Args... rest) { t->set(i, Value::V(v)); return _T(t, i+1, rest...); }

template <typename... Args>
Value L(Args... all) { return _L(new List(sizeof...(all)), all...); }

template <typename... Args>
Value T(Args... all) { return _T(new Tuple(sizeof...(all)), 0, all...); }

}

#endif /* __XOBJ_LIST_H__ */
