#include <stdint.h>

#include "xobj_dict.h"
#include "xobj_str.h"

using namespace xobj;

namespace xobj {

Value Dict::New(size_t size) {
    return new Dict(size);
}

String *Dict::get(char *str, size_t len, hash_t hash) {
    auto node = getnode(hash);
    if (!*node)
        return nullptr;
    auto p = node;
    do {
        auto k = p->key();
        if (k.isstr() && k.s().equals(str, len))
            return &k.s();
        p = p->next();
    } while (p != node);
    return nullptr;
}

Value Dict::get(const Value& k) {
    if (k.isnil())
        return Value::Nil;
    auto node = getnode(k);
    return node ? node->value() : Value::Nil;
}

void Dict::set(const Value& k, const Value& v) {
    if (k.isnil())
        return;
    auto node = getnode(k.hash());
    if (!node)
        return alloc(1), set(k, v);
    Node *exist = nullptr;
    bool needmove = false;
    if (node->empty())              // node is idle
        exist = node,
        exist->key() = k;
    else if (really(node))          // node is already occupied by a key
        exist = node->find(k),      // if exists a key equals k
        needmove = false;
    else
        needmove = true;
    if (!exist) {
        auto idle = popidle();
        if (!idle)                  // there is no idle node in the dict,
            return expand(), set(k, v); // it should expand capacity, and call set again
        if (needmove)               // the node shouldn't be here,
            node->moveto(idle),         // it will be moved to a idle node
            node->init(k, Value::Nil),
            exist = node;
        else
            idle->key() = k,        // idle is the new node
            node->linkin(idle),     // link idle into node
            exist = idle;
    }
    incsize();
    exist->value() = v;
}

bool Dict::remove(const Value& k) {
    auto node = getnode(k);
    if (node) {
        auto n = node->rmself();
        pushidle(n ? n : node);
        decsize();
        return true;
    }
    return false;
}

void Dict::clear() {
    auto o = _items;
    alloc(0);
    delete[] o;
}

void Dict::expand() {
    auto oi = _items;                   // old items
    auto oc = capacity();               // old capacity
    if (oc) {
        alloc(2 * oc);
        transfer(oi, oi + oc);
        delete[] oi;
    } else {
        alloc(1);
    }

}

void Dict::transfer(Node *begin, Node *end) {
    Node *collid = nullptr;                 // List head of colliding nodes
    for (auto p = begin; p < end; p++) {
        if (p->empty() || p->value().isnil())
            continue;
        auto node = getnode(p->key().hash());
        if (node->empty())
            node->set(p->key(), p->value()),
            incsize();
        else if (collid)
            collid->linkin(p);
        else
            p->setnext(p),                  // detatch p from it's linklist
            collid = p;
    }
    // Insert the colliding nodes 
    auto p = collid;
    if (p) do {
        auto idle = popidle(),
             node = getnode(p->key().hash());
        idle->set(p->key(), p->value());
        node->linkin(idle);
        incsize();
    } while (p = p->next(), p != collid);
}

}
