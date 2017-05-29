#ifndef __XOBJ_STR_H__
#define __XOBJ_STR_H__

#include "xobj_obj.h"

namespace xobj {

struct _StrAttr {
    uint32_t hash = 0;
    bool     iskw = true;
};

_StrAttr _getstrattr(char *data, size_t len);
uint32_t _gethash(char *data, size_t len);

struct String: public Object {
    static String *New(char *str, size_t len);
    static String *New(std::istream& in, size_t len);

    inline size_t len() const { return _ex.u32; }
    bool equals(char *str, size_t len);

    bool operator==(Value &v) const override;
    type_t type() const override { return TV_STRING; }
    operator bool() const override { return len() > 0; }

    virtual bool isbin() const { return true; }
    virtual void isbin(bool b) {}
    virtual bool iskeyword() const { return true; }
    virtual const char *c_str() const { return nullptr; }

protected:
    inline void len(uint32_t n) { _ex.u32 = n; }
};

class VarStr : public String {
public:
    bool isbin() const override { return _isbin; }
    void isbin(bool b) override { _isbin = b; }
    bool iskeyword() const override { return _attr.iskw; }
    hash_t hash() const override { return _attr.hash; }
    const char *c_str() const override { return data; }

    void release() override { delete (char*)this; }

private:
    friend class String;

    VarStr() { isbin(false); }
    VarStr(_StrAttr& attr) : VarStr() {
        _attr = attr;
        isbin(attr.iskw);
    }
    VarStr(char *str, size_t n, _StrAttr& attr) : VarStr(attr) {
        memcpy(data, str, n);
        String::len(n); data[n] = 0;
    }

    _StrAttr _attr;
    bool     _isbin;
    char   data[1];
};

class ConstStr : public String {
public:
    bool isbin() const override { return true; }
    void isbin(bool b) override {}
    bool iskeyword() const override { return _attr.iskw; }
    hash_t hash() const override { return _attr.hash; }
    const char *c_str() const override { return _str; }

    void release() override { delete this; }

private:
    friend Value CS(const char *);

    ConstStr(const char *);

    const char *_str;
    _StrAttr _attr;
};

class TempStr : public String {
public:
    bool isbin() const override { return true; }
    void isbin(bool b) override {}
    bool iskeyword() const override { return false; }
    hash_t hash() const override {
        return _hs ? _hs : (_hs = _getstrattr(_str, len()).hash);
    }
    const char *c_str() const override { return _str; }

    void release() override { delete this; }

private:
    friend Value TS(char *);
    friend Value TS(char *, size_t);

    TempStr(char *str, size_t n) : _str(str) {
        String::len(n);
    }

    char *_str;
    mutable hash_t _hs = 0;
};

Value TS(char *str, size_t n);
Value TS(char *str);
Value CS(const char *str);

}

#endif /* __XOBJ_STR_H__ */
