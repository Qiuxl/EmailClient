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
	API();/*���캯��*/
	~API();/*��������*/
	bool fetch_file(sMailInfo& mail);
	bool isOnline(sMailInfo &mail);
	bool logon(sMailInfo& mail);/*��������豣֤�û���������Ϊ�������,�Ƿ�����Ҳ��һ����������������ȷ��API�����ȷ����������*/
								/*���ݰ�������ʱ����ʾ����*/
	bool set(const sMailInfo& mail);/*����Ϣд�룬����sMailInfo��ǰ4����Ա����*/
	bool sendMessage(sMailInfo mail, vector<string>&file_path );/*�����������ʼ�����ϵͳ����Ҫ�����ṩע�ͺ����������Ϣ*/
	bool getSendMail(message_info&info, int num);/*���ص�num���ʼ����ݵ����ã��ϲ����forѭ��ʵ��,numԽСԽ��*/
	bool getReceiveMail(message_info&info, int num);/*���ص�num���ʼ����ݵ����ã��ϲ����forѭ��ʵ��*/
	int getSendNum();/*���ط�������*/
	int getReceiveNum();/*�����ռ�����*/
	bool deleteSendMail(string time);/*ɾ���ڼ���*/
	bool deleteReceiveMail(string time);/*ɾ���ڼ���*/
	bool deleteUserInfo();/*ɾ���û���Ϣ*/
};

#endif// !_API_
