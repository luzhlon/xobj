#ifndef __XOBJ_OUT_H__
#define __XOBJ_OUT_H__

#include <iostream>

#include "xobj_val.h"

namespace xobj {

using namespace std;

void _print_bin(ostream& o, const char *data, size_t len);
void _print_str(ostream& o, const char *data, size_t len);

ostream& operator<<(ostream&, const Value&);
ostream& operator<<(ostream& o, const String& s);
ostream& operator<<(ostream& o, const List& l);
ostream& operator<<(ostream& o, const Dict& d);

}

#endif /* __XOBJ_OUT_H__ */
