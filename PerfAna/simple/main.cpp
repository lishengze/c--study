#include <string.h>

#define MAX_STRING_LEN 1024

class BaseClass {
    
private:
    int a_;
    double b_;
    char d1_[MAX_STRING_LEN];
    char d2_[MAX_STRING_LEN];
    char d3_[MAX_STRING_LEN];
    char d4_[MAX_STRING_LEN];
    char d5_[MAX_STRING_LEN];
    char d6_[MAX_STRING_LEN];
    char d7_[MAX_STRING_LEN];
    char d8_[MAX_STRING_LEN];

public:
    void SetA(int a) { a_ = a;}
    void SetB(double b) { b_ = b;}
    void SetD(char* src, int len) { memcpy(d1_, src, len >MAX_STRING_LEN?MAX_STRING_LEN:len);}

    int GetA() {return a_;}
};

class MemberClass{
public:
    BaseClass   base_obj_;
};

class InheritClass: public BaseClass{

};

int main() {
    MemberClass member_obj;
    InheritClass inherit_obj;

    member_obj.base_obj_.SetA(100);
    inherit_obj.SetA(100);

  	MemberClass* p_member_obj = new MemberClass();
    InheritClass* p_base_obj = new InheritClass();
  
  	if (p_member_obj) {
        p_member_obj->base_obj_.SetA(100);
        delete p_member_obj;
    }

    if (p_base_obj) {
        p_base_obj->SetA(100);
        delete p_base_obj;
    }

    return 1;
}