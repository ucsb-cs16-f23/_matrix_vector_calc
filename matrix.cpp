#include <iostream>
#include"mymatrix.h"
#include"my_Vars.h"
#include"command_a.h"
#include<string>
using namespace std;

int main()
{
    char* _m_iobuf = new char[2048];
    FunctionClassINIT(4096, 128);
    cmNode::table_init();
    implFuncInit();
    while (true)
    {
        cin.getline(_m_iobuf, 2048, '\n');
        if (strcmp(_m_iobuf, "exit")==0)break;
        try {
            procCommand_1(_m_iobuf);
        }
        catch (const char str_error[]) {
            cerr << "Error!\n"<< str_error << endl;
        }
        catch (int) {
            cerr << "int type err" << endl;
        }
    }
    return 0;
}

