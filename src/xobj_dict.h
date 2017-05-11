#ifndef __XOBJ_DICT_H__
#define __XOBJ_DICT_H__

#include <iostream>
#include <vector>
#include <algorithm>
#include "xobj_obj.h"

namespace xobj {

struct Dict: public Object {
    struct Node {
        Node() = default;
        ~Node() = default;

        inline Value& key() { return _key; }
        inline Value& value() { return _val; }
        inline bool empty() { return key().isnil(); }

    private:
        friend struct Dict;

        inline void init() {
            key().init(), value().init();
            prev = 0, next = 0;
        }
        inline void init(Value &k, Value &v = Value::Nil) {
            set(k, v);
            prev = next = 0;
        }
        inline void set(Value &k, Value &v = Value::Nil) { key() = k; value() = v; }
        inline Node& operator=(Node &n) {
            key() = n.key(); value() = n.value();
            return *this;
        }
        inline Node *Next() { return this+next; }
        inline Node *Prev() { return this+prev; }
        inline void  Next(Node *n) { next = n - this; }
        inline void  Prev(Node *n) { prev = n - this; }

        Value _key;     // key
        Value _val;     // value
        int   prev = 0;
        int   next = 0;
    };

    struct Iterator {
        Iterator(Node *P, Node *E) :p(P), e(E) { increase(); }

        void  operator++() { ++p; increase(); }
        bool  operator!=(Iterator& it) { return p != it.e; }
        Node& operator*() { return *p; }

        inline void increase() { while (p < e && p->empty()) p++; }
    private:
        Node *p;
        Node *e;
    };

    Dict(size_t size = 0) : Object(TV_DICT) { alloc(size); }
    ~Dict() = default;

    inline int len() const { return _size; }
    void set(Value &k, Value &v);
    void set(const Value &k, const Value &v) { set((Value &)k, (Value &)v); }
    Value get(Value &k) {
        if (k.isnil()) return Value::Nil;
        auto *node = NodeByKey(k);
        return node ? node->value() : Value::Nil;
    }
    Value get(const Value &k) { return get((Value &)k); }
    /* Tuple *keys(); */
    /* Tuple *values(); */
    /* Tuple *items(); */

    void clear() { auto o = _items; alloc(0); delete[] o; }

    Iterator begin() const { return *(new Iterator(_items, _items+_capacity)); }
    Iterator end() const { return *(new Iterator(_items, _items+_capacity)); }

    friend std::ostream& operator<<(std::ostream&, const Dict&);
    DECLARE_TOPYOBJ

private:
    friend struct String;
    String *get(char *str, size_t len);

private:
    void alloc(size_t size) {
        _capacity = size * 2;
        _free = _capacity;
        _size = 0;
        _items = _capacity ? new Node[_capacity + 1]: nullptr;
    }
    void checkitems() { if (!_capacity) alloc(1); }
    void rehash();
    // 
    inline Node *HashNode(uint32_t hash) {
        return _capacity ? &_items[hash % _capacity]: nullptr;
    }
    Node *HashNode(Value &k) { return HashNode(k._hash()); }
    // Link new node n to o placed
    void Link(Node *o, Node *n);
    // Move the node
    void MoveNode(Node *src, Node *dst);
    // Get a idle node, it's needed extend if return NULL
    Node *GetIdleNode();
    // Find node in links k placed
    Node *FindNode(Node *n, Value &k);
    // Node k occupied, there is no thus in dict if return NULL
    Node *NodeByKey(Value &k);
    // A new key
    Node *NewKeyNode(Value &k);

    Node    *_items;
    uint16_t _size;
    uint16_t _free;
    uint16_t _capacity;
};

inline Value _D(Dict *d) { return Value::V(d); }
template <typename T, typename T2, typename... Args> inline
Value _D(Dict *d, T k, T2 v, Args... rest) { d->set(k, v); return _D(d, rest...); }

template <typename... Args>
Value D(Args... all) {
    auto d = new Dict((sizeof...(all))/2);
    return _D(d, all...);
}

}

#endif /* __XOBJ_DICT_H__ */
