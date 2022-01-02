#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include"sign.h"
#include"File.h"
#define BUFFSIZE 1024
#define  LINEPLUS  if(cur_ch == '\n' && isForward)\
					{\
						line++;\
					}
					
int get_ch();//利用配对缓冲区，读取缓冲区字符
void read_code(ofstream& dtxt, ofstream& etxt);//读取源程序，并进行词法分析
void out_data(string data, int data_sign, ofstream& txt);//数据类型信息
void out_error(string data, int error_sign, ofstream& txt);//错误信息
void print_inf();//输出语句行数,各类单词、字符个数

int left_buf[BUFFSIZE] = { 0 };//左输入缓冲
int right_buf[BUFFSIZE] = { 0 };//右输入缓冲
int* left_end = &left_buf[1024];//左缓冲区结尾
int* right_end = &right_buf[1024];//右缓冲区结尾
int* cur = right_end;//向前指针

string token = "";//存储字符串
int sign_num[7] = { 0 };//存储运算符、字符等数量
string error_type[2] = { "Error input","未识别结束符"};
string data_type[8] = { "identifier","constant","op","delimiters","keyword","string","character" };

string result = "";//记录当前识别结果
bool isForward = true;
int length = 1;
int line = 1;//记录当前行数
int line_num = 1;
int state = 0;//标识符状态，初始为0
long ch_num = 0;//字符总数

ifstream demo;
int main()
{
	ofstream dtxt(DATA_FILE, ios::trunc);
	ofstream etxt(ERROR_FILE, ios::trunc);
	demo.open(TEST_FILE);

	type_assign();
	read_code(dtxt, etxt);
	print_inf();

	demo.close();
	dtxt.close();
	etxt.close();
	system("pause");
	return 0;
}

void read_code(ofstream& dtxt, ofstream& etxt)//读取源程序，并进行词法分析
{
	int cur_ch = get_ch();//读取字符
	LINEPLUS;
	int ch_type = cha[cur_ch];//判断类型
	int isFlag = 2;
	while (cur_ch != -1)
	{
		if (cur_ch == '#')
		{
			while (cur_ch != '\n')
			{
				cur_ch = get_ch();
				LINEPLUS;
			}
		}
		ch_type = cha[cur_ch];

		isForward = false;
		switch (state)
		{
		case 0://初始状态
		{
			switch (ch_type)
			{
			case 0:case 4:// ' ' ,'\n', '\t', '\r', 界符
			{
				state = 0;
				if ((char)cur_ch == '\"')//双引号
				{
					token += "\"";
					state = 10;
					isFlag = 1;
					break;
				}
				else if ((char)cur_ch == '\'')//单引号
				{
					token += "\'";
					state = 11;
					break;
				}
				else if (ch_type == 4)
				{
					string tmp = "";
					tmp += (char)cur_ch;
					out_data(tmp, 3, dtxt);
				}
				cur_ch = get_ch();
				LINEPLUS;
				break;
			}
			case 1://进入识别标识符、关键字阶段
			{
				token += (char)cur_ch;
				state = 1;
				cur_ch = get_ch();
				LINEPLUS;
				break;
			}
			case 2://识别常数
			{
				token += (char)cur_ch;
				state = 2;
				cur_ch = get_ch();
				LINEPLUS;
				break;
			}
			case 3://运算符
			{
				token += (char)cur_ch;
				state = 8;
				cur_ch = get_ch();
				if (cur_ch == '*')
					state = 9;
				LINEPLUS;
				break;
			}
			default://发生错误，识别到了非字符集的输入，进行忽略
			{
				string tmp = "";
				state = 0;
				tmp += (char)cur_ch;
				out_error(tmp, 0, etxt);
				cur_ch = get_ch();
				LINEPLUS;
				break;
			}
			}
			break;
		}
		case 1://标识符、关键字
		{
			switch (ch_type)
			{
			case 1:case 2://标识符可有任意个 _ 和数字
			{
				token += (char)cur_ch;
				state = 1;
				cur_ch = get_ch();//从输入流读取一个字符
				LINEPLUS;
				break;
			}
			case 0:case 3:case 4://类型：界符，标识符识别结束
			{
				if (keyword.find(token) != keyword.end())//查找成功
				{
					out_data(token, 4, dtxt);
				}
				else//不是关键字，则是标识符
				{
					out_data(token, 0, dtxt);
				}

				token = "";
				state = 0;

				break;
			}
			default://发生错误，识别到非法字符，则将其视为空白符跳过
			{
				state = 1;
				string tmp = "";
				tmp += (char)cur_ch;
				out_error(tmp, 0, etxt);
				cur_ch = get_ch();//从输入流读取一个字符
				LINEPLUS;
				break;
			}

			}
			break;
		}
		case 2://常数
		{
			if ((char)cur_ch == '.')//识别小数状态
			{
				token += (char)cur_ch;

				state = 3;
				cur_ch = get_ch();
				LINEPLUS;
			}
			else if ((char)cur_ch == 'e' || (char)cur_ch == 'E')//识别指数状态
			{
				token += (char)cur_ch;

				state = 5;
				cur_ch = get_ch();//从输入流读取一个字符
				LINEPLUS;
			}
			else if (ch_type == 2)//仍为此状态
			{
				token += (char)cur_ch;

				state = 2;
				cur_ch = get_ch();//从输入流读取一个字符
				LINEPLUS;
			}
			else if (ch_type == 4 || ch_type == 3)//界符或运算符
			{
				out_data(token, 1, dtxt);
				token = "";
				state = 0;
			}
			else//在识别常数时出现了错误字符，如字母，则将其当作界符
			{
				out_data(token, 1, dtxt);
				token = "";
				string tmp = "";
				tmp += (char)cur_ch;
				out_error(tmp, 0, etxt);

				state = 0;
				cur_ch = get_ch();//从输入流读取一个字符
				LINEPLUS;
			}
			break;
		}
		case 3://小数点后第一个数字
		{
			if (ch_type == 2)//识别到数字
			{
				token += (char)cur_ch;
				state = 4;
				cur_ch = get_ch();
				LINEPLUS;
			}
			else//小数点后没有识别到数字，则默认小数以0结尾继续识别
			{
				token += '0';
				state = 4;

				string tmp = "";
				tmp += (char)cur_ch;
				out_error(tmp, 0, etxt);
				cur_ch = get_ch();
				LINEPLUS;
			}
			break;
		}
		case 4://小数点后的其余数字
		{
			if ((char)cur_ch == 'e' || (char)cur_ch == 'E')//转到识别指数状态
			{
				token += (char)cur_ch;
				state = 5;
				cur_ch = get_ch();
				LINEPLUS;
			}
			else if (ch_type == 2)//继续识别数字
			{
				token += (char)cur_ch;
				state = 4;
				cur_ch = get_ch();
				LINEPLUS;
			}
			else if (ch_type == 4 || ch_type == 3)//运算符或界符
			{
				out_data(token, 1, dtxt);
				token = "";
				state = 0;
			}
			else//错误处理
			//视为界符
			{
				out_data(token, 1, dtxt);
				token = "";
				string tmp = "";
				tmp += (char)cur_ch;
				out_error(tmp, 0, etxt);
				state = 0;
				cur_ch = get_ch();//从输入流读取一个字符
				LINEPLUS;
			}
			break;
		}
		case 5://识别指数
		{
			if ((char)cur_ch == '+' || (char)cur_ch == '-')
			{
				token += (char)cur_ch;

				state = 6;
				cur_ch = get_ch();
				LINEPLUS;
			}
			else if (ch_type == 2)
			{
				token += (char)cur_ch;

				state = 7;
				cur_ch = get_ch();
				LINEPLUS;
			}
			else//未识别到有效的正负号和常数,则视为识别到+号
			{
				token += '+';
				string temp = "";
				temp += (char)cur_ch;
				out_error(temp, 0, etxt);

				state = 6;
				cur_ch = get_ch();
				LINEPLUS;
			}
			break;
		}
		case 6://识别指数E之后的符号之后
		{
			if (ch_type == 2)//继续识别数字
			{
				token += (char)cur_ch;

				state = 6;
				cur_ch = get_ch();
				LINEPLUS;
			}
			else if (ch_type == 4 || ch_type == 3)//遇到界符、运算符
			{
				out_data(token, 1, dtxt);
				token = "";
				state = 0;
			}
			else//读取到 非数字 字符，视为界符
			{
				out_data(token, 1, dtxt);
				token = "";
				string temp = "";
				temp += (char)cur_ch;
				out_error(temp, 0, etxt);
				state = 0;
				cur_ch = get_ch();
				LINEPLUS;
			}
			break;
		}
		case 7://识别指数次数
		{
			if (ch_type == 2)//遇到了常数
			{
				token += (char)cur_ch;

				state = 6;
				cur_ch = get_ch();
				LINEPLUS;
			}
			else//未识别到有效数字,则视为识别到0
			{
				token += '0';

				string temp = "";
				temp += (char)cur_ch;
				out_error(temp, 0, etxt);

				state = 6;
				cur_ch = get_ch();//从输入流读取一个字符
				LINEPLUS;
			}
			break;
		}
		case 8://运算符和界符
		{
			length++;
			if (token == "/")//注释开始标志
			{
				if ((char)cur_ch == '*')//识别为注释
				{
					state = 9;
					cur_ch = get_ch();
					LINEPLUS;
					break;
				}
				else if ((char)cur_ch == '/')//第二类注释则去寻找换行符
				{
					cur_ch = get_ch();
					LINEPLUS;
					while ((cur_ch != -1) && ((char)cur_ch != '\n'))
					{
						cur_ch = get_ch();
						LINEPLUS;
					}
					//mark
					token = "";
					state = 0;
					break;
				}
			}
			else if (token == "\"")//准备识别字符串
			{
				state = 10;
				isFlag = 1;
				cur_ch = get_ch();
				LINEPLUS;
				break;
			}
			else if (token == "\'")//准备识别字符
			{
				state = 11;
				cur_ch = get_ch();
				LINEPLUS;
				break;
			}

			if (length > 1 && ch_type != 0)
			{
				ch_type = 2;
			}

			switch (ch_type)
			{
			case 0:
			{
				state = 8;
				cur_ch = get_ch();//从输入流读取一个字符
				LINEPLUS;
				break;
			}
			case 3://类型：运算符，进入运算符识别模式
			{
				token += (char)cur_ch;
				state = 8;
				cur_ch = get_ch();//从输入流读取一个字符
				LINEPLUS;
				break;
			}
			case 1:case 2:case 4://遇到数字、标识符、界符均可认为识别已经结束
			{
				length = 0;
				if (op.find(token) != op.end())
				{
					out_data(token, 2, dtxt);
				}
				else if (delimiter.find(token) != delimiter.end())
				{
					out_data(token, 3, dtxt);
				}
				else
				{
					string tmp1 = "";
					tmp1 += token[0];
					string tmp2 = "";
					tmp2 += token[1];
					if (op.find(tmp1) != op.end() && op.find(tmp2) != op.end())
					{
						out_data(tmp1, 2, dtxt);
						out_data(tmp2, 2, dtxt);
					}
				}
				token = "";
				state = 0;

				break;
			}
			default://遇到非法字符 忽略
			{
				string tmp = "";
				tmp += (char)cur_ch;
				out_error(tmp, 0, etxt);
				state = 8;
				cur_ch = get_ch();
				LINEPLUS;
				break;
			}
			}
			break;
		}
		case 9://注释
		{
			while (1)
			{
				cur_ch = get_ch();
				LINEPLUS;
				if (cur_ch == -1)
					break;
				else if ((char)cur_ch == '*')
				{
					cur_ch = get_ch();
					LINEPLUS;
					if (cur_ch == -1)//文件结束
						break;
					else if ((char)cur_ch == '/')//注释结束
					{
						cur_ch = get_ch();
						LINEPLUS;
						ch_type = cha[cur_ch];
						state = 0;
						token = "";
						break;
					}
					else
						break;
				}
			}
			break;
		}
		case 10://识别字符串
		{
			while (1)
			{
				cur_ch = get_ch();
				LINEPLUS;
				if (cur_ch == -1)
					break;
				else if ((char)cur_ch == '\"')//识别到了另一半引号
				{
					token += (char)cur_ch;
					out_data(token, 5, dtxt);
					token = "";

					state = 0;
					cur_ch = get_ch();
					LINEPLUS;
					break;
				}
				else
				{
					if ((char)cur_ch == '\n'&&isFlag==1)
					{
						isFlag = 2;
						line--;
						out_error(token, 1, etxt);
						state = 0;
						break;
					}
					token += (char)cur_ch;
				}
			}
			break;
		}
		case 11://识别字符
		{
			cur_ch = get_ch();
			LINEPLUS;
			token += cur_ch;
			if (cur_ch == '\\')
			{
				cur_ch = get_ch();
				token += cur_ch;
				LINEPLUS;
			}
			cur_ch = get_ch();
			LINEPLUS;
			if ((char)cur_ch == '\'')
			{
				token += '\'';
				out_data(token, 6, dtxt);
				token = "";
				state = 0;
				cur_ch = get_ch();
				LINEPLUS;
			}
			else//字符识别错误,抛出异常
			{
				out_error(token, 0, etxt);
				token += '\'';//补救措施
				out_data(token, 6, dtxt);
				token = "";
				state = 0;
			}
			break;
		}
		default:
			exit(1);
		}
	}
}
int get_ch()//读取一个字符
{
	isForward = true;
	if (demo.peek() == EOF)
		ch_num++;
	if (cur == left_end)
	{
		cur = right_buf;
		while (!demo.eof() && cur != right_end)
		{
			char ch;
			demo >> noskipws >> ch;//读入所有字符
			if (demo.fail())
				break;
			*cur = ch;
			*cur++;
			if (ch == '\n' && isForward)
			{
				line_num++;
			}
		}
		if (cur != right_end)
			*cur = -1;
		cur = right_buf;
	}
	else if (cur == right_end)
	{
		cur = left_buf;
		while (!demo.eof() && cur != left_end)
		{
			char ch;
			demo >> noskipws >> ch;//读入所有字符
			if (demo.fail())
				break;
			if (ch == '\n'&&isForward)
				line_num++;
			*cur = ch;
			*cur++;
		}
		if (cur != left_end)
			*cur = -1;
		cur = left_buf;
	}
	return (int)(*cur++);
}
void out_data(string data, int data_sign, ofstream& txt)//数据类型信息
{
	txt << "Line " << line << ":   ";
	txt << left << setw(10) << data;
	txt << "	Notation: " << data_type[data_sign] << endl;
	sign_num[data_sign]++;
}
void out_error(string data, int error_sign, ofstream& txt)//错误信息
{
	txt << "Error in Line " << line << ":   ";
	txt << data;
	txt << "	Error type: " << error_type[error_sign] << endl;
}
void print_inf()//输出语句行数,各类单词、字符个数
{
	cout << "详细信息参见输出文件" << endl;
	cout << "字符总数：" << ch_num << endl;
	cout << "行数：" << line_num << endl;
	cout << "标识符数量：" << sign_num[0] << endl;
	cout << "常数数量：" << sign_num[1] << endl;
	cout << "运算符数量：" << sign_num[2] << endl;
	cout << "界符数量：" << sign_num[3] << endl;
	cout << "关键字数量：" << sign_num[4] << endl;
	cout << "字符串数量：" << sign_num[5] << endl;
}