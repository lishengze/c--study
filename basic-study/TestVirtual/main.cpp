#include <iostream>
#include <memory.h>
#include <typeinfo>
#include <string>
using namespace std;

// #pragma pack(8)

typedef void (*BasicFuncPtr)();

void PrintFuncAddress(string info, int datasize,  void* obj, int func_count) {
    // cout << "\n----------- Start Test " << info << " ------------ "  << endl;
    printf("\n----------- Start Test %s, this: %p , ends: %p ------------\n", info.c_str(), obj, (void *)(int64_t(obj) + datasize));
    int64_t** vptr = reinterpret_cast<int64_t**>(obj);
    int64_t* vtbl = *vptr;
    printf("vptable address: %p \n", vtbl);

    for (int i = 0; i<func_count; ++i) {
        BasicFuncPtr func = (BasicFuncPtr)*(vtbl+i);
        // cout << "vtbl+" << i << ": "  << *(vtbl+i) << endl;

        printf("vtbl+%d is: %p\n", i, func);
        func();
        cout << endl;
    } 
}

class BaseV0 {
    public:
        BaseV0() {
            // cout << "Base1 Construct this: " << this << endl;

            // memset(this, 0, sizeof(this));

            // print(); // 为什么可以?

            // PrintFuncAddress("BaseV1 Constructor", this, 2);
        }

        void print() {
            cout << "Base0 Print" << endl;
        }

        void func1() {
            cout << "Base0 func1" << endl;
        }

        int a;
};

class BaseV1 {
    public:
        BaseV1() {
            // cout << "Base1 Construct this: " << this << endl;

            // memset(this, 0, sizeof(this));

            // print(); // 为什么可以?

            PrintFuncAddress("BaseV1 Constructor", sizeof(BaseV1), this, 2);
        }

        virtual void print() {
            cout << "Base1 Print" << endl;
        }

        virtual void func1() {
            cout << "Base1 func1" << endl;
        }

};

class BaseV2 {
    public:
        BaseV2() {
            PrintFuncAddress("BaseV2 Constructor", sizeof(BaseV2),this, 2);
        }

        virtual void print() {
            cout << "Base2 Print" << endl;
        }

        virtual void func2() {
            cout << "Base2 func2" << endl;
        }

};

class BaseV3 {
    public:
        BaseV3() {
            PrintFuncAddress("BaseV3 Constructor", sizeof(BaseV3),this, 2);
        }

        virtual void print() {
            cout << "Base3 Print" << endl;
        }

        virtual void func3() {
            cout << "Base3 func3" << endl;
        }

};

class DeV:public  BaseV0, public BaseV1, public BaseV2, public BaseV3{
    public:
        DeV(): BaseV1(), BaseV2(), BaseV3() {
            // cout << "DeV Construct this: " << this << endl;

            // int64_t** vptr = reinterpret_cast<int64_t**>(this);
            // int64_t* vptb = reinterpret_cast<int64_t*>(*vptr);

            // for (int i = 0; i < 3; ++i) {
            //     cout << vptb+i << endl;
            // }

            PrintFuncAddress("DeV Constructor", sizeof(DeV), this, 2);

        }

        virtual void print() {
            cout << "DeV Print" << endl;
        }

        // virtual void funcd1() {
        //     cout << "DeV funcd1" << endl;
        // }    

        // virtual void funcd2() {
        //     cout << "DeV funcd2" << endl;
        // }             `   
};

class DeV2:public DeV{
    public:
        DeV2(){
            // cout << "DeV Construct this: " << this << endl;

            // int64_t** vptr = reinterpret_cast<int64_t**>(this);
            // int64_t* vptb = reinterpret_cast<int64_t*>(*vptr);

            // for (int i = 0; i < 3; ++i) {
            //     cout << vptb+i << endl;
            // }

            PrintFuncAddress("DeV2 Constructor", sizeof(DeV), this, 4);

        }

        virtual void print() {
            cout << "DeV Print" << endl;
        }

        // virtual void funcd1() {
        //     cout << "DeV funcd1" << endl;
        // }    

        // virtual void funcd2() {
        //     cout << "DeV funcd2" << endl;
        // }                
};


int main()
{
    // printf( "Basev1::Print: %p\n", (BasicFuncPtr)&BaseV1::print);
    // printf( "BaseV2::Print: %p\n", (BasicFuncPtr)&BaseV2::print);
    // printf( "BaseV3::Print: %p\n", (BasicFuncPtr)&BaseV3::print);
    // printf( "Basev1::func1: %p\n", (BasicFuncPtr)&BaseV1::func1);
    // printf( "BaseV2::func2: %p\n", (BasicFuncPtr)&BaseV2::func2);
    // printf( "BaseV3::func3: %p\n", (BasicFuncPtr)&BaseV3::func3);    
    // printf( "DeV::Print: %p\n", (BasicFuncPtr)&DeV::print);

    // // cout << "Base1::Print: " << &BaseV1::print << endl;
    // // cout << "Base1::Print: " << &BaseV2::print << endl;
    // // cout << "DeV::Print: " << &DeV::print << endl;
    // // 

    // // BaseV1* base = new BaseV1();
    // // base->print();

    // // BaseV2* base2 = new DeV();

    // // BaseV1* base1 = new DeV();
    // // base1->print();

    // DeV* dev1 = new DeV();

    // BaseV1* b11 = dev1;
    // PrintFuncAddress("Base_dev1_V1", sizeof(BaseV1), b11, 2);

    // BaseV2* b12 = dev1;
    // PrintFuncAddress("Base_dev1_V2", sizeof(BaseV2), b12, 2);

    // BaseV3* b13 = dev1;
    // PrintFuncAddress("Base_dev1_V3",sizeof(BaseV3),  b13, 2);  

    // int64_t* dev1_vptr = *(reinterpret_cast<int64_t**>(dev1));
    // BasicFuncPtr fun1 = BasicFuncPtr(dev1_vptr[0]);
    // fun1();

    // // printf("dev1_vptr address: %p \n", dev1_vptr);

    // int64_t* b11_vptr = *(reinterpret_cast<int64_t**>(b11));
    // BasicFuncPtr fun2 = BasicFuncPtr(b11_vptr[0]);
    // fun2();

    // // printf("b11_vptr address: %p \n", b11_vptr);

    // int64_t* b12_vptr = *(reinterpret_cast<int64_t**>(b12));
    // BasicFuncPtr fun3 = BasicFuncPtr(b12_vptr[0]);
    // fun3();

    // // printf("b12_vptr address: %p \n", b12_vptr);

    // int64_t* b13_vptr = *(reinterpret_cast<int64_t**>(b13));
    // BasicFuncPtr fun4 = BasicFuncPtr(b13_vptr[0]);
    // fun4();

    // delete dev1;

    // printf("b13_vptr address: %p \n", b13_vptr);

    // PrintFuncAddress("Dev1", sizeof(DeV), dev1, 2);
   

    cout << "************ Test Different Derived Vptable Address *************" << endl;
    DeV* dev2 = new DeV();

    BaseV1* b21 = dev2;
    PrintFuncAddress("Base_dev2_V1", sizeof(BaseV1), b21, 2);

    BaseV2* b22 = dev2;
    PrintFuncAddress("Base_dev2_V2", sizeof(BaseV2), b22, 2);

    BaseV3* b23 = dev2;
    PrintFuncAddress("Base_dev2_V3", sizeof(BaseV3), b23, 2); 

    PrintFuncAddress("Dev2", sizeof(DeV), dev2, 2);    


    cout << "\n*********************** Test Third Derived *****************" << endl;
    DeV2* dev3 = new DeV2();
    PrintFuncAddress("Dev3", sizeof(DeV2), dev3, 4); 

    dev3->func3();



    cout << "sizeof (BaseV1): " << sizeof(BaseV1) << "\n"
    << "sizeof (BaseV2): " << sizeof(BaseV2) << "\n"
    << "sizeof (BaseV3): " << sizeof(BaseV3) << "\n"
    << "sizeof (DeV): " << sizeof(DeV) << "\n";

	return 0;
}

