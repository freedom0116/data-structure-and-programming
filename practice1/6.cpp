#include <iostream>
using namespace std;

void f()
{
    int a, b, c;
    cout << "a = " << &a << endl;
    cout << "b = " << &b << endl;
    cout << "c = " << &c << endl;
}
int main()
{
    f();
    f();
    int *p = new int;
    int *q = new int;
    cout << "p = " << &p << endl;
    cout << "q = " << &q << endl;    
    f();
}