#include "test.h"
#include "global_declare.h"
#include "pandora/util/path_util.h"

void init_log(string program_full_name)
{
    try
    {
        string work_dir = utrade::pandora::get_work_dir_name (program_full_name);
        string program_name = utrade::pandora::get_program_name(program_full_name);

        cout << "program_full_name: " << program_full_name << "\n"
            << "work_dir: " << work_dir << "\n"
            << "program_name: " << program_name << "\n"
            << endl;

        LOG->set_work_dir(work_dir);
        LOG->set_program_name(program_name);
        LOG->start();
 
    }
    catch(const std::exception& e)
    {
        std::cerr << __FILE__ << ":"  << __FUNCTION__ <<"."<< __LINE__ << " " <<  e.what() << '\n';
    }    
}


void impl_main(int argc, char* argv[])
{
    init_log(argv[0]);
}