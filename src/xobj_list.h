#ifndef __XOBJ_LIST_H__
#define __XOBJ_LIST_H__

#include <iostream>
#include <vector>
#include <algorithm>
#include "xobj_obj.h"

namespace xobj {

struct List: public Object, public std::vector<Value> {
    List(size_t size = 0) : Object(TV_LIST) { reserve(size); }
    //List(Value *list, size_t size);
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

    inline int len() const { return size(); }
    inline Value get(int i) const { return (*this)[i]; }
    void   set(int i, Value &v) { if (i>=size())resize(i+1); (*this)[i] = v; }

    DECLARE_TOPYOBJ

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

private:
    //std::vector<Value> vec;
};

inline Value _L(List *l) { return Value::V(l); }
template <typename T, typename... Args> inline
Value _L(List *l, T v, Args... rest) { l->push(Value::V(v)); return _L(l, rest...); }

template <typename... Args>
Value L(Args... all) { return _L(new List(sizeof...(all)), all...); }

}

#endif /* __XOBJ_LIST_H__ */
