#include<iostream>
#include<fstream>
#include<vector>
#include<iomanip>
#include<string>
#include<stack>
#include<map>
#define rows 6
#define cols 9
using namespace std;
class LL
{
private:
	string input_s;//输入，即待分析字符串
	string stack_s;//栈中字符串
	string ana_table[rows][cols];//预测分析表
	stack<string> stack;//栈
	vector<string> Terminator;//终结符
	vector<string> nTerminator;//非终结符
	map<string, vector<string>> expression;//文法表达式
	map<string, vector<string>> first_set;//First集合
	map<string, vector<string>> follow_set;//Follow集合
	map<string, int> col_nter;//非终结符对应的行号
	map<string, int> row_ter;//终结符对应的列号
public:
	string init_s;//最原始的分析字符串
	void init_set();//各种符号，集合的初始化
	void init_stack();//栈的初始化
	void create_anal_table();//构造、输出分析表
	void LL_analysis();//对输入符号串进行分析
};

int main()
{
	LL L_table;
	L_table.init_set();
	L_table.create_anal_table();

	cout << "请输入待分析串：\n";
	cin >> L_table.init_s;
	L_table.init_s += "$";
	L_table.init_stack();
	L_table.LL_analysis();
	system("pause");
	return 0;
}

void LL::init_set()//各种符号，集合的初始化
{
	/*-----------------存入无左递归文法---------------*/
	expression["E"] = { "TW" };
	expression["W"] = { "+TW","-TW","ε" };
	expression["T"] = { "FX" };
	expression["X"] = { "*FX","/FX","ε" };
	expression["F"] = { "(E)","num" };
	cout << "LL(1)文法:" << endl;
	for (auto ptr = expression.begin(); ptr != expression.end(); ptr++)
	{
		cout << ptr->first << "->";
		for (auto eptr = ptr->second.begin(); eptr < ptr->second.end(); eptr++)
			if (eptr < ptr->second.end() - 1)
				cout << *eptr << "|";
			else
				cout << *eptr << endl;
	}
	/*-------------------终结符--------------------*/
	ana_table[0][1] = "+", ana_table[0][2] = "-", ana_table[0][3] = "*", ana_table[0][4] = "/";
	ana_table[0][5] = "(", ana_table[0][6] = ")", ana_table[0][7] = "num", ana_table[0][8] = "$";
	/*-------------------非终结符------------------*/
	ana_table[1][0] = "E", ana_table[2][0] = "W", ana_table[3][0] = "T",
		ana_table[4][0] = "X", ana_table[5][0] = "F";
	/*-------------------FIRST集合----------------*/
	first_set["E"] = {"(", "num"};
	first_set["W"] = { "+","-","ε" };
	first_set["T"] = { "(","num" };
	first_set["X"] = { "*","/","ε" };
	first_set["F"] = { "(","num" };
	/*------------------FOLLOW集合----------------*/
	follow_set["E"] = { "$",")" };
	follow_set["W"] = { "$",")" };
	follow_set["T"] = { "$",")","+","-" };
	follow_set["X"] = { "$",")","+","-" };
	follow_set["F"] = { "$","+","-","*","/",")" };
	/*---------终结符和非终结符对应的行号和列号-------*/
	for (int i = 1; i < rows; i++)
		row_ter[ana_table[i][0]] = i;
	for (int j = 1; j < cols; j++)
		col_nter[ana_table[0][j]] = j;
}
void LL::init_stack()//栈的初始化
{
	//将起始符号E和$开始
	stack.push("$");
	stack.push(ana_table[1][0]);
	stack_s = "$" + ana_table[1][0];
}
void LL::create_anal_table()//构造、输出分析表
{
	ana_table[0][0] = "";
	for (int i = 1; i < rows; i++)
	{
		string n_ter = ana_table[i][0];//取出非终结符的first集和follow集
		vector<string>first_tmp = first_set[n_ter];
		vector<string>follow_tmp = follow_set[n_ter];
		bool e_empty = false;//exit empty，判断是否含空
		if (*(first_tmp.end() - 1) == "ε")
			e_empty = true;
		for (int j = 1; j < cols; j++)
		{
			string ter = ana_table[0][j];//终结符
			if (find(first_tmp.begin(), first_tmp.end(), ter) != first_tmp.end())//找到对应的终结符
			{
				vector<string> sentence = expression[n_ter];
				if (sentence.size() == 1)//单个生成式
					ana_table[i][j] = n_ter + "->" + sentence[0];
				else//多个生成式
				{
					for (int k = 0; k < sentence.size(); k++)
					{
						string tmp = sentence[k];
						if (ter.size() == 1)
							tmp = tmp.at(0);
						if (tmp == ter)
						{
							ana_table[i][j] = n_ter + "->" + sentence[k];
							break;
						}
					}
				}
			}
			//含空产生式且终结符在FOLLOW集中
			else if (e_empty && find(follow_tmp.begin(), follow_tmp.end(), ter) != follow_tmp.end())
				ana_table[i][j] = n_ter + "->ε";
			else if (find(follow_tmp.begin(), follow_tmp.end(), ter) != follow_tmp.end())
				ana_table[i][j] = "synch";
			else
				ana_table[i][j] = "error";
		}
	}
	cout << "生成的预测分析表如下所示:" << endl;
	for (int i = 0; i <rows; i++) 
	{
		for (int j = 0; j < cols; j++)
			cout << setw(10) << left << ana_table[i][j];
		cout << endl;
	}
}
void LL::LL_analysis()//对输入符号串进行分析
{
	input_s = init_s;
	int c_ptr = 0;//指向正在分析的字符
	string genrate_s = "";//所用生成式
	while (!stack.empty())
	{
		cout << "栈：" << setw(input_s.size() + 10) << stack_s << "输入:" << setw(input_s.size() + 10) << init_s.substr(c_ptr) << "使用的生成式:" << genrate_s << endl;
		string s_flag = "";//字符标记
		genrate_s = "";//产生式
		char ch = input_s.at(c_ptr);//当前分析的字符
		string s_top = stack.top();//栈顶符号
		if (s_top == "$")
		{
			cout << "LL(1)语法分析完成！" << endl;
			return;
		}
		if (ch <= '9' && ch >= '0')
			s_flag = "num";
		else
			s_flag = ch;
		//当前字符符号和栈顶符号相同时
		if (s_flag == s_top)
		{
			if (stack.top() == "num")
				stack_s.erase(stack_s.end() - 3 - stack_s.begin(), 3);//删除 n u m
			else
				stack_s.erase(stack_s.end() - 1);
			stack.pop();//栈顶出栈
			if (s_flag == "num")
			{
				//为了读取多位数字，将指针移动到非数字的第一位
				for (; c_ptr < input_s.size(); c_ptr++)
				{
					char c_ch = input_s.at(c_ptr);//当前字符
					if (c_ch == '+' || c_ch == '-' || c_ch == '*' ||c_ch == '/' || c_ch == '(' || c_ch == ')' || c_ch == '$')
						break;
				}
			}
			else
				c_ptr++;
		}
		else
		{
			//栈顶符号对应的行号
			int row = row_ter[s_top];
			int col = col_nter[s_flag];
			//在分析表中找到对应信息
			genrate_s = ana_table[row][col];
			if (genrate_s == "synch")
			{//从栈顶弹出
				genrate_s = "出错，M[" + ana_table[row][0] + "," + s_flag + "]" + " = synch,弹出" + stack.top();
				if (stack.top() == "num")
					stack_s.erase(stack_s.end() - 3 - stack_s.begin(), 3);
				else
					stack_s.erase(stack_s.end() - 1);
				stack.pop();
			}
			else if (genrate_s == "error")
			{
				genrate_s = "出错,M[" + ana_table[row][0] + "," + s_flag + "]" + " = error ，向前移动向前指针";
				c_ptr++;
			}
			else
			{
				if (stack.top() == "num")
					stack_s.erase(stack_s.end() - 3 - stack_s.begin(), 3);
				else
					stack_s.erase(stack_s.end() - 1);
				stack.pop();
				int pos = genrate_s.find_first_of(">") + 1;
				string tmpstr = genrate_s.substr(pos);
				if (tmpstr == "num")
				{
					stack.push(tmpstr);
					stack_s += tmpstr;
				}
				else if (tmpstr == "ε");
				else
				{
					for (int i = tmpstr.size() - 1; i >= 0; i--)
					{//逆序入栈
						char tmp1 = tmpstr[i];//获取生成式右部
						string tmp = "";
						tmp += tmp1;
						stack.push(tmp);
						stack_s += tmp;
					}
				}
			}
		}
	}
}