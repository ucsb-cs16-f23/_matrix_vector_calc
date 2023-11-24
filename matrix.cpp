#define _CRTDBG_MAP_ALLOC
#include <iostream>
#include"mymatrix.h"
#include"my_Vars.h"
#include"command_a.h"
#include<string>
#include<Windows.h>
using namespace std;

ostream& operator << (ostream& os, const wchar_t* wstr)
{
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), wstr, wcslen(wstr), NULL, NULL);
    return os;
}

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
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
        catch (int code) {
            switch (code)
            {
            case -1:
                cout << L"杂鱼🤍，你抽卡出货率就跟你的除数一样！" << endl;
                break;
            case 8:
                cout << L"矩阵的那一列有东西吗？笨蛋！" << endl;
                break;
            case 10:
                cout << L"没人没告诉你向量相加要维数一致吗？果真是杂🤍鱼🤍捏！" << endl;
                break;
            case 11:
                cout << L"杂鱼🤍，你是怎么把两个维数不一样的向量点乘的？" << endl;
                break;
            case 19:
                cout << L"为什么你的下标里面不放数字？还真是杂🤍鱼🤍捏！(...[<Not A Number>])" << endl;
                break;
            case 100:
                cout << L"果真是笨蛋啊，居然想对一个显然不可逆的矩阵（行和列数量不一样）找逆矩阵！" << endl;
                break;
            case 10000:
                cout << L"杂鱼🤍，你是怎么把两个类型不一致的变量加在一起的？" << endl;
                break;
            case 227:
                cout << L"住手！在向量的某个元素塞入向量？太大了塞不下啊！" << endl;
                break;
            case 226:
                cout << L"杂鱼🤍，我才不会告诉你等号左边的表达式不可以赋值呢！" << endl;
                break;
            case 114:
                cout << L"笨蛋，你在干什么？栈爆掉啦！" << endl;
            default:
                break;
            }
        }
    }
    delete[] _m_iobuf;
    _clean_data_before_quit();
    return 0;
}

