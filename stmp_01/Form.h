#ifndef _Form_h_
#define _Form_h_
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
#include"API.h"

extern struct sMailInfo;
extern class message_info;
extern class CSendMail;
extern class Pop_receiver;
extern class File;

class Form
{
public:
	CSendMail		m_mail;
	message_info	m_message;
	sMailInfo		m_info;
	API				m_api;
	vector<string>	m_x;
public:
	Form();
	~Form();
	void showSendMail();
	void showmain();
	int showlogin();
	void showOnline();
	void showOffline();
	void showReciever();
	void showSender();
};
#endif
