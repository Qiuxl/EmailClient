#include "Form.h"
#include<cstring>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
Form::Form()
{
	m_info.mail_server_name = "smtp.163.com"; // 163服务器地址 ！！！
}

Form::~Form()
{}

void Form::showmain()
{
	while (true)
	{
		cout << "============ SMTP ============" << endl;
		cout << endl;
		cout << " [1] 登陆.\n";
		cout << " [2] 退出.\n";
		cout << endl;
		int choice;
		cin >> choice;
		switch (choice)
		{
			case 1: showlogin(); break;
			case 2: cout << endl << "ByeBye! "; system("pause"); return;
			default:
				cout << "请输入正确的选项！" << endl;
				continue;
		}

	}
}

int Form::showlogin()
{
	char scanName[50];
	char scanPwd[20];
	cout << "============ SMTP ============" << endl;
	printf(" 请输入用户名：");
	cin >> scanName;
	cout << endl;
	printf(" 请输入密码  ：");
	cin >> scanPwd;
	cout << endl;

	m_info.mailbox_user_name = scanName;
	m_info.mailbox_sender = scanName;
	m_info.mailbox_user_pwd = scanPwd;
	if (m_api.isOnline(m_info))
	{
		cout << "Login Sucess!" << endl;
		cout << "Welcome " << m_info.mailbox_user_name << " !" << endl;
		system("pause");
		showOnline();
	}
	else
	{
		cout << "offline\n";
		system("pause");
	}

		/*
	if( isonline == online )
	showOnline();
	else if( isonline == offline )
	showOffline();
	else
	showWrong();
	*/
	
	return 0;
}

void Form::showOnline()
{
	int idlist[1];
	idlist[0] = 1;
	system("cls");
	while (true)
	{
		int choice;
		system("cls");
		cout << "Welcome " << m_info.mailbox_user_name << " to use."<<endl;
		cout << endl;
		cout << "============ SMTP ============" << endl;
		cout << endl;
		cout << "[1] 收件箱.\n";
		cout << endl;
		cout << "[2] 发送邮件.\n";
		cout << endl;
		cout << "[3] 发件箱.\n";
		cout << endl;
		cout << "[4] 退出.\n";
		cout << endl;
		cin >> choice;
		switch (choice)
		{
		case 1:
			showReciever();
			break;
		case 2:
			showSender();
			break;
		case 3:
			showSendMail();
			break;
		case 4:
			return;
		default:
			printf("Wrong input\n");
			getchar();
		}
	}
}

void Form::showSender()
{
	char receiver_address[50];
	char receiver_show_name[20];
	char mail_title[128];
	char mail_body[4096];
	string title, line;
	string body="", part;
	string attach;
	system("cls");

	cout << "======================= 发邮件 =======================" << endl;
	printf("收件人地址:");
	cin >> receiver_address;
	cout << endl;

	m_info.Cc_addr = ""; //

	printf("发件人署名:");
	cin >> receiver_show_name;
	cout << endl;
	getchar();
	printf("标题：");
	memset(mail_title, 0, sizeof(mail_title));
	cin.getline(mail_title, 200, '\n');
	printf("正文,以====（四个等号结尾）：\n");
	memset(mail_body, 0, sizeof(mail_body));
	while (1)
	{
		cin.getline(mail_body, 200, '\n');
		line = mail_body;
		int judge = line.find("====");
		if (judge == std::string::npos)
		{
			string temp;
			temp.assign(line, 0, judge);
			body += "\n" +line;
			//break;
		}
		else
		{
			string temp;
			temp.assign(line, 0, judge);
			body += "\n" + temp;
			break;
		}
		memset(mail_body, 0, sizeof(mail_body));
	}
	cin.clear();
	cout << endl;
	memset(mail_body, 0, sizeof(mail_body));
	body.copy(mail_body, body.length());
	cout << endl;
	cout << "添加附件？如果多个以分号隔开即可,没有附件就输入N:  ";
	cin >> attach;
	cout << endl;
	m_info.receiver = receiver_address;
	m_info.mailbox_sender_show_name = receiver_show_name;    //发送显示 的名字
	m_info.mail_title = mail_title;;
	m_info.mail_body = mail_body;
	int index_1, index_2;
	m_x.clear();
	if (attach.length() <= 2)
	{

	}
	else
	{
		if ((index_1 = attach.find(';')) == std::string::npos)
		{
			m_x.push_back(attach);
		}
		else{
			while (index_1 != std::string::npos)
			{
				string temp;
				temp.assign(attach, 0, index_1);
				m_x.push_back(temp);
				attach.erase(0, index_1 + 1);
				index_1 = attach.find(';');
			}
			m_x.push_back(attach);
		}
	}

	if( !m_api.sendMessage(m_info, m_x) )
	{
		cout << "send mail failed" << endl;
	}
	else{
		cout << "send successfully!" << endl;
	}
	system("pause");
}

void Form::showReciever()
{
	message_info info;
	int mail_num,select_num;
	bool flag = true;
	mail_num = m_api.getReceiveNum();
	while (flag)
	{
		system("cls");
		printf("---------------------------- receiver ----------------------------|\n");
		cout << "收件夹中共存放 "<<mail_num<<" 封邮件"<< endl;
		if (mail_num < 5)
			cout << "前" << mail_num << "封邮件的主题如下"<<endl;
		else
			cout << "前5封邮件的主题如下" << endl;
		cout.clear();
		for (int i = 1; i <= 5 && i <= mail_num; i++)
		{
			m_api.getReceiveMail(info, i);
			cout << " "<< i << " " << "主题：" << info.subject << "  发件人：" << info.sender_name << endl;
		}
		cout << "\n请选择要查看的邮件,输入0则退出" << endl;
		cin >> select_num;
		if (select_num == 0)
		{
			cout << "返回上一层"<<endl;
			flag = false;
		}
		else if (select_num > mail_num||select_num<0)
		{
			cout << "数值越界，重新输入"<< endl;
		}
		else
		{
			m_api.getReceiveMail(info, select_num);
			cout << "sender name: " << info.sender_name << endl;
			cout << "sender address: " << info.sender_addr << endl;
			cout << "receiver list: ";
			for (int i = 0; i < info.receiver.size(); i++)
			{
				cout << info.receiver[i] << " ";
			}
			cout << endl;
			cout << "date: " << info.date << endl;
			cout << "Cc address: ";
			for (int i = 0; i < info.Cc_list.size(); i++)
			{
				cout << info.Cc_list[i] << " ";
			}
			cout << endl;
			cout << "subject: " << info.subject << endl;
			cout << "main text:" << endl;
			cout << info.text_body << endl;

		}
		cout << "---------------------------- receiver ----------------------------|\n";
		system("pause");
	}
}


void Form::showSendMail()
{
	message_info info;
	int mail_num, select_num;
	bool flag = true;
	mail_num = m_api.getSendNum();
	while (flag)
	{
		system("cls");
		printf("---------------------------- sender ----------------------------|\n");
		cout << "收件夹中共存放 " << mail_num << " 封邮件" << endl;
		if (mail_num < 5)
			cout << "前" << mail_num << "封邮件的主题如下" << endl;
		else
			cout << "前5封邮件的主题如下" << endl;
		cout.clear();
		for (int i = 1; i <= 5 && i <= mail_num; i++)
		{
			m_api.getSendMail(info, i);
			cout << " " << i << " " << "主题：" << info.subject << "  发件人：" << info.sender_name << endl;
		}
		cout << "\n请选择要查看的邮件,输入0则退出" << endl;
		cin >> select_num;
		if (select_num == 0)
		{
			cout << "返回上一层" << endl;
			flag = false;
		}
		else if (select_num > mail_num || select_num<0)
		{
			cout << "数值越界，重新输入" << endl;
		}
		else
		{
			m_api.getSendMail(info, select_num);
			cout << "sender name: " << info.sender_name << endl;
			cout << "sender address: " << info.sender_addr << endl;
			cout << "receiver list: ";
			for (int i = 0; i < info.receiver.size(); i++)
			{
				cout << info.receiver[i] << " ";
			}
			cout << endl;
			cout << "date: " << info.date << endl;
			cout << "Cc address: ";
			for (int i = 0; i < info.Cc_list.size(); i++)
			{
				cout << info.Cc_list[i] << " ";
			}
			cout << endl;
			cout << "subject: " << info.subject << endl;
			cout << "main text:" << endl;
			cout << info.text_body << endl;

		}
		cout << "---------------------------- sender ----------------------------|\n";
		system("pause");
	}
}