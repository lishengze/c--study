#include "test_template_add.h"

void TestTemplateAdd()
{
    try
    {
        User user;

        DataBase* user_a = new DataUseA();

        user.init_base(user_a);

        TestDataA data_a;
        data_a.id = 999;

        user.add_data(&data_a);
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] TestTemplateAdd " << e.what() << '\n';
    }
    
}