#include "online_test.h"
#include <iostream>
using namespace std;

void func1() {
    int arr[50],i,n;
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    for (i = 3; i < n; ++i) {
        arr[i] = arr[i-1] + arr[i-2] + arr[i-3];
    }
    cout << arr[n-1] << endl;

}

void OnlineTest() {
    cout << "Online Test" << endl;

    func1();


}