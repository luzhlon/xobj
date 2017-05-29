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
            key() = k;
            value() = v;
            _prev = _next = 0;
        }
        // Move this node to n
        void moveto(Node *n) {
            n->init(key(), value());
            auto nx = next(), pr = prev();
            if (nx != this)     // next is valid
                n->setnext(nx), nx->setprev(n);
            if (pr != this)     // previous is valid
                n->setprev(pr), pr->setnext(n);
        }
        // Link node n into this linklist
        void linkin(Node *n) {
            n->setprev(this);
            n->setnext(next());
            next()->setprev(n);
            setnext(n);
        }
        // Find k in this linklist
        Node *find(Value& k) {
            auto p = this;
            do {
                if (p->key() == k)
                    return p;
            } while ((p = p->next(), p != this));
            return nullptr;
        }
        inline Node *next() { return this + _next; }
        inline Node *prev() { return this + _prev; }
        inline void  setnext(Node *n) { _next = n - this; }
        inline void  setprev(Node *n) { _prev = n - this; }

    private:
        Value _key;     // key
        Value _val;     // value
        int   _prev = 0;
        int   _next = 0;
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
        auto node = getnode(k);
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
    void rehash();
    // Get a idle node, it's needed extend if return NULL
    Node *idlenode();
    // Get node by hash
    Node *getnode(hash_t hash, bool safe = false) {
        if (_capacity)
            return &_items[hash % _capacity];
        return safe ? (alloc(1), getnode(hash)): nullptr;
    }
    // Get node by key
    Node *getnode(Value &key) {
        auto node = getnode(key.gethash());
        return node && really(node) ?
            node->find(key) : nullptr;
    }
    // A new key
    Node *newkey(Value &k);
    // Check a node if should be in it's position
    inline bool really(Node *n) {
        return getnode(n->key().gethash()) == n;
    }

private:
    Node    *_items;
    uint16_t _size;
    uint16_t _free;
    uint16_t _capacity;
};

inline Value _D(Dict *d) { return Value::V(d); }

template <typename T, typename T2, typename... Args>
inline Value _D(Dict *d, T k, T2 v, Args... rest) {
    d->set(k, v);
    return _D(d, rest...);
}

template <typename... Args>
Value D(Args... all) {
    auto d = new Dict((sizeof...(all))/2);
    return _D(d, all...);
}

}

#endif /* __XOBJ_DICT_H__ */
