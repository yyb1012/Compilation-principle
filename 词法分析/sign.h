#pragma once
#include<set>
#include<string>
using namespace std;

int cha[1024] = { 0 };
set<string> op = {
	"+","-","*","/","%","++","--",//算数运算
	">","<","==",">=","<=","!=",//关系运算
	"&&","||","!",//逻辑运算
	"&","|","~","^","<<",">>",//位操作
	"=","+=","-=","*=","/=","%=","&=","|=","^=",">>=","<<=",//赋值运算
	".",":","\\"
};//运算符

set<string> delimiter{
	"(",")","[","]","{","}",";",",","\"","\'",
};//界符

set<string> keyword{
	"auto","short","int","long","float","double","char",
	"struct","union","enum","typedef","const","unsigned",
	"signed","extern","register","static","void","if","else",
	"switch","case","for","do","while","goto",
	"continue","break","default","sizeof","return",
};//关键字

//标记字符类型
void type_assign()
{
	int i = 0;
	for (i = 0; i < 1024; i++)
	{
		char ch = (char)i;
		string tmp = "";
		tmp += ch;//转换为string便于后续操作
		while (ch == '=')
			break;
		if (ch == '\t' || ch == '\r' || ch == '\n' || ch == ' ')
			cha[i] = 0;//跳过
		else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
			cha[i] = 1;//字母
		else if (ch >= '0' && ch <= '9')
			cha[i] = 2;//数字
		else if (op.find(tmp) != op.end())
			cha[i] = 3;//运算符
		else if (delimiter.find(tmp) != delimiter.end())
			cha[i] = 4;//界符
		else if (ch == '#')
			cha[i] = 5;//预处理
		else
			cha[i] = -1;
	}
}
