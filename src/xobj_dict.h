#ifndef __XOBJ_DICT_H__
#define __XOBJ_DICT_H__

#include <iostream>
#include <algorithm>
#include "xobj_obj.h"
#include "xobj_val.h"

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

        inline void init(Value &k, Value &v = Value::Nil) {
            set(k, v); prev = next = 0;
        }
        inline void set(Value &k, Value &v = Value::Nil) {
            key() = k; value() = v;
        }
        inline Node& operator=(Node &n) {
            key() = n.key(); value() = n.value();
            return *this;
        }
        inline Node *Next() { return this+next; }
        inline Node *Prev() { return this+prev; }
        inline void  Next(Node *n) { next = n - this; }
        inline void  Prev(Node *n) { prev = n - this; }

    private:
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

    Dict(size_t size = 0) { alloc(size); }
    ~Dict() { delete[] _items; }

    inline int len() const { return _size; }
    void set(Value &k, Value &v);
    void set(const Value &k, const Value &v) { set((Value &)k, (Value &)v); }
    Value get(Value &k) {
        if (k.isnil()) return Value::Nil;
        auto node = NodeByKey(k);
        return node ? node->value() : Value::Nil;
    }
    Value get(const Value &k) { return get((Value &)k); }
    /* Tuple *keys(); */
    /* Tuple *values(); */
    /* Tuple *items(); */

    void clear() { auto o = _items; alloc(0); delete[] o; }

    type_t type() const override { return TV_DICT; }
    void release() override { delete this; }
    operator bool() const override { return len() > 0; }
    bool operator==(Value& v) const override {
        return v.isdict() && this == &v.d();
    }

    Iterator begin() const { return *(new Iterator(_items, _items+_capacity)); }
    Iterator end() const { return *(new Iterator(_items, _items+_capacity)); }

    friend std::ostream& operator<<(std::ostream&, const Dict&);

private:
    friend struct String;
    String *get(char *str, size_t len, hash_t hash);

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
    Node *HashNode(Value &k) { return HashNode(k.gethash()); }
    // Link new node n to o placed
    void Link(Node *o, Node *n);
    // Move the node
    void MoveNode(Node *src, Node *dst);
    // Get a idle node, it's needed extend if return NULL
    Node *GetIdleNode();
    // Find node in links which k placed in
    Node *FindNode(Node *n, Value &k);
    // Node which k occupied, return NULL if NONE
    Node *NodeByKey(Value &k);
    // A new key
    Node *NewKeyNode(Value &k);
    // Check a node if should be in it's position
    inline bool Really(Node *n) { return HashNode(n->key()) == n; }

private:
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
