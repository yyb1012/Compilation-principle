#include<iostream>
#include<string>
#include<iomanip>
#define sub_str str.substr(c_ptr)
#define flag_check if(!flag) return;
using namespace std;
void E();
void W();
void T();
void X();
void F();
bool flag = true;
int c_ptr = 0;//指向被分析的字符
string str = "";
int main()
{
	cout << "请输入待分析串：" << endl;
    cin >> str;
	str += "$";
    cout << left << setw(20) << "表达式:" << setw(20) << "待分析串:" << endl;
	E();
    if (str[c_ptr] == '$' && flag)
        cout << "Acc,分析成功" << endl;
	else
		cout << "analyse failed! 语句不合法" << endl;
    system("pause");
    return 0;
}
void E()
{
    flag_check;
    cout << left << setw(20) << "E->TW" << setw(20) << sub_str << endl;
    T();
    W();
}
void W()
{
    flag_check;
	if (str[c_ptr] == '+')
    {
        cout << left << setw(20) << "W->+TW" << setw(20) << sub_str << endl;
        c_ptr++;
        T();
        W();
    }
    else if (str[c_ptr] == '-')
    {
        cout << left << setw(20) << "W->-TW" << setw(20) << sub_str << endl;
        c_ptr++;
        T();
        W();
    }
}
void T()
{
    flag_check;
    cout << left << setw(20) << "T->FX" << setw(20) << sub_str << endl;
    F();
    X();
}
void X()
{
    flag_check;
    if (str[c_ptr] == '*')
    {
        c_ptr++;
        F();
        X();
    }
    else if (str[c_ptr] == '/')
    {
        c_ptr++;
        F();
        X();
    }
}
void F()
{
    flag_check;
    if (str[c_ptr] == '(')
    {
        c_ptr++;
        E();
        if (str[c_ptr] == ')')
        {
            cout << left << setw(20) << "F->(E)" << setw(20) << sub_str << endl;
            c_ptr++;
        }
        else
            flag = false;
    }
    else if (isdigit(str[c_ptr]))
    {
        while(isdigit(str[c_ptr]))
            c_ptr++;
        cout << left << setw(20) << "F->num" << setw(20) << sub_str << endl;
    }
    else
        flag = false;
}
