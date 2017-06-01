#include <iostream>
#include <sstream>
#include <string>

#include "xobj_val.h"
#include "xobj_list.h"
#include "xobj_dict.h"
#include "xobj_mp.h"
#include "xobj_str.h"
#include "xobj_num.h"
#include "xobj_out.h"

#include "tstream.h"

using namespace xobj;
using namespace std;

class Test {
public:
    // test msgpack converting
    void msgpack() {
        tstream ts;
        if (!ts.connect("127.0.0.1", 5000)) {
            cout << "Connect failure\n"; return;
        }
        Value data;
        if (!mp_unpack(data, ts)) {
            cout << "Receive data failure\n"; return;
        }
        cout << data << endl;
        if (!mp_pack(data, ts))
            cout << "Send data failure\n";
    }
    // test string internal
    void internal() {
        auto s1 = CS("abc");
        Value s2("abc");
        cout << "Test string internal:\n\t"
            << &s1.o() << ' ' << &s2.o() << ' ' << (&s1.s() == &s2.s()) << endl;
    }
    // test list
    void list() {
        auto list = L("1", "2", "3", 4, 5, 6, true);
        auto& l = list.l();
        l.push(7);
        cout << "Test List: " << l << endl;
        cout << "Test List Iterator: " << endl;
        //for (auto i = l.list()->begin(); i != l.list()->end(); i++)
        for (auto i : l)
            cout << i << endl;
        cout << endl;
    }
    // test dictionary
    void dict() {
        auto dict = D(
            "a", 1,
            "b", 2,
            "c", 3,
            "d", 4);
        auto& d = dict.d();
        cout << "Test Dictionary:\t" << d << endl;
        d.remove("a");
        cout << "After remove 'a':\t" << d << endl;
        d.clear();
        cout << "After clear: \t" << d << endl;
        d.set(1, "1");
        d.set(2, "2");
        d.set("3", 3);
        d.set(true, "true");
        d.set("true", true);
        cout << "Insert new items and Iterator:\n";
        for (auto i : d)
            cout << '\t' << i.key() << "\t: " << i.value() << endl;
        cout << endl;
    }

    void pointer() {
        int n = 1;
        auto p = P(&n);
        auto l = p.p<int>();
        cout << "Test pointer:" << *l << endl;
    }
};

void test1(const Value& v) {
    cout << v << endl;
}

int main(int argc, char **argv) {
    //cout << sizeof(Value) << endl
    //    << sizeof(Object) << endl
    //    << sizeof(Int) << endl
    //    << sizeof(Float) << endl
    //    << sizeof(String) << endl
    //    << sizeof(List) << endl
    //    << sizeof(Dict) << endl;
    Test test;
    test.pointer();
    test.internal();
    test.list();
    test.dict();
    test.msgpack();
    getchar();
    return 0;
}
