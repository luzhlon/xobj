#ifndef __XOBJ_NUM_H__
#define __XOBJ_NUM_H__

#include "xobj_obj.h"

namespace xobj {

struct Int: public Object {
    Int(int64_t i) { this->i = i; }

    inline int64_t val() const { return i; }

    hash_t hash() const override { return i; }
    type_t type() const override { return TV_INT; }
    operator bool() const override { return val() != 0; }
    void release() override {
        delete this;
    }
    bool operator==(const Value &v) const override {
        return v.isint() && val() == v.i().val();
    }

private:
    int64_t i;
};

struct Float : public Object {
    Float(double f) { this->f = f; }

    inline double val() const { return f; }

    hash_t hash() const override { return *(hash_t *)&f; }
    type_t type() const override { return TV_FLOAT; }
    operator bool() const override { return val() != 0.0; }
    void release() override { delete this; }
    bool operator==(const Value &v) const override {
        return v.isfloat() && val() == v.f().val();
    }

private:
    double f;
};

}

#endif /* __XOBJ_NUM_H__ */
