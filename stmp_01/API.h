#ifndef _API_
#define _API_
#include"stdafx.h"
#include"Pop3.h"
#include"sendmail.h"
#include "FILE.h"
#include<string>
#include <windows.h>
#include<stdio.h>
#include<direct.h> 
#include<time.h>
#include<vector>

using namespace std;

extern struct sMailInfo;
extern class message_info;
extern class CSendMail;
extern class Pop_receiver;
extern class File;

class API 
{
private:
	int time_sign;
	string username;
	CSendMail mail_info;
	File file_info;
	message_info message;

	int decode_time(const char* time);
	void get_root(const char* user_name);
	
public:
	string rootaddress;
	string Error_info;
	API();/*构造函数*/
	~API();/*析构函数*/
	bool fetch_file(sMailInfo& mail);
	bool isOnline(sMailInfo &mail);
	bool logon(sMailInfo& mail);/*传入参数需保证用户名和密码为外界输入,是否有网也是一个输入参数，如果正确，API会把正确的内容载入*/
								/*内容包括发送时的显示名称*/
	bool set(const sMailInfo& mail);/*将信息写入，就是sMailInfo的前4个成员变量*/
	bool sendMessage(sMailInfo mail, vector<string>&file_path );/*将参数传给邮件发送系统，需要界面提供注释后带！！！信息*/
	bool getSendMail(message_info&info, int num);/*返回第num封邮件内容到引用，上层可用for循环实现,num越小越新*/
	bool getReceiveMail(message_info&info, int num);/*返回第num封邮件内容到引用，上层可用for循环实现*/
	int getSendNum();/*返回发件数量*/
	int getReceiveNum();/*返回收件数量*/
	bool deleteSendMail(string time);/*删除第几封*/
	bool deleteReceiveMail(string time);/*删除第几封*/
	bool deleteUserInfo();/*删除用户信息*/
};

#endif// !_API_
