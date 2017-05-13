#include <iostream>
#include <sstream>
#include <string>

#include "xobj_val.h"
#include "xobj_list.h"
#include "xobj_dict.h"
#include "xobj_mp.h"
#include "xobj_str.h"

using namespace xobj;
using namespace std;
// test msgpack converting
void test_msgpack() {
    auto l = L(Value::Nil, D("float", 1.23, "negative", -12333333333333332));
    //auto l = V(1.23);
    cout << l << endl;
    stringstream s;
    xobj::toMsgPack(s, l);
    auto& str = s.str();
    s.seekg(0, s.beg);
    _print_bin(cout, str.c_str(), str.size());
    cout << endl;
    Value v;
    if (fromMsgPack(s, v))
        cout << v << endl;
}
// test string internal
void test_internal() {
    Value s1("abc");
    Value s2("abc");
    cout << "Test string internal:\n"
         << s1._obj << endl
         << s2._obj << endl << endl;
}
// test list
void test_list() {
    auto l = L("1","2","3", 4, 5, 6, true);
    l += 7;
    cout << "Test List: " << l << endl;
    cout << "Test List Iterator: " << endl;
    //for (auto i = l.list()->begin(); i != l.list()->end(); i++)
    for (auto i : *l.list())
        cout << i << endl;
    cout << endl;
}
// test dictionary
void test_dict() {
    auto d = D("a", 1,
               "b", 2,
               "c", 3,
               "d", 4);
    cout << "Test Dictionary: " << d << endl;
    cout << "Dictionary Iterator:" << endl;
    for (auto i : *d.dict())
        cout << i.key() << ':' << i.value() << endl;
    cout << endl;
    d.clear();
    d.set("e\x01", 5);
    d.set(1, "1");
    cout << "Clear Dictionary: " << d << endl;
    cout << "d.ea = " << d.get("ea") << endl;
    cout << "d[1] = " << d[1] << endl;
    cout << "d[2] = " << d[2] << endl;
}

int main(int argc, char **argv) {
    using namespace std;

    //test_internal();
    //test_list();
    //test_dict();
    test_msgpack();

    getchar();
    return 0;
}
