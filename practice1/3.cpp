#include <iostream>
using namespace std;

int main()
{
    int a;
    int *p;
    cout << "&a = " << &a << endl;
    cout << "&p = " << &p << endl;
    p = &a;
    cout << "&a = " << &a << endl;
    cout << "&p = " << &p << endl;
    cout << "a = " << a << endl;
    cout << "p = " << p << endl;

}