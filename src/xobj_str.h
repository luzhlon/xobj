#ifndef __XOBJ_STR_H__
#define __XOBJ_STR_H__

#include <iostream>
#include "xobj_obj.h"

namespace xobj {

uint32_t _gethash(char *data, size_t len);
void _print_str(std::ostream& o, const char *data, size_t len);
void _print_bin(std::ostream& o, const char *data, size_t len);

struct String: public Object {
    static String *New(char *str);
    static String *New(char *str, size_t len);
    static String *New(const char *);
    static void    Delete(String *);

    friend std::ostream& operator<<(std::ostream&, const String&);

    inline size_t  len() const { return _size; }

    virtual bool operator==(Value &v) const;
    virtual uint32_t hash() const { return _hash; }

    bool equals(char *str, size_t len);

    inline const char *c_str() const { return data; }

    String() :Object(TV_STRING), _size(0) {}

private:
    size_t   _size;      // size of bytes without '\0'
    uint32_t _hash;
    char     data[1];
};

}

#endif /* __XOBJ_STR_H__ */
