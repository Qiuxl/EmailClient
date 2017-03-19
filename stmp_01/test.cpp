#include"stdafx.h"
#include"Pop3.h"
#include"sendmail.h"
#include"API.h"
#include"Form.h"
void TestSendMail()
{
	CSendMail mail;
	sMailInfo info;

	//  memcpy(info.m_pcIPAddr, "123.125.50.138", strlen("123.125.50.138"));  
	info.mail_server_name = "smtp.163.com"; // 163服务器地址
	info.mailbox_sender = "qzh951120@163.com";   // 发送者
	info.mailbox_user_name = "qzh951120@163.com";    // 用户名
	info.mailbox_user_pwd = "qzh9511200818";   //密码
	info.receiver = "xlq1120@yahoo.com";
	info.Cc_addr = "";
	info.mailbox_sender_show_name = "邱小亮";    //发送显示 的名字
	info.mail_title = "hello,祝你期末考试顺利";
	info.mail_body = "这是一封来自于cpp测试小组的邮件，考试顺利，心情愉快。\n";
	//以上为界面传递给我的信息
	mail.set(info);
	//mail.AddFilePath("C:\\Users\\qzh\\Desktop\\CreateSearchTree.cpp");
	//mail.AddFilePath("C:\\Users\\qzh\\Desktop\\SmtpClient.rar");
	//mail.AddFilePath("C:\\Users\\qzh\\Desktop\\hello.txt");
	//	mail.AddFilePath("C:\\Users\\qzh\\Desktop\\buffer.rar");
	if (!mail.BeginSendMail())
	{
		cout << "send mail failed" << endl;
	}
	else{
		cout << "send successfully!" << endl;
	}
}
void testReceive()
{
	message_info mail_result;
	Pop_receiver temp("cpp2016_group@163.com", "pzcpp2016", "cpp2016_group\\attachment\\", "pop3.163.com");
	if (!temp.create_socket())  //必须先创建协议用的套接字，失败可能是网络没连上
	{
		cout << "create socket failed" << endl;
		return;
	}
	if (!temp.login())
	{
		cout << "user name or password wrong!" << endl;
		return;
	}
	;
	Sleep(1000);     //稍等一会
	int max_mail = temp.get_mail_sum();  //获取当前服务器上的邮件数目
	temp.fetch_mail(max_mail, mail_result);   //调用fetch函数获得对应id的邮件
	temp.clear();        ///最后清除套接字

	//下面这段程序展示结果
	cout << "sender name: " << mail_result.sender_name << endl;
	cout << "sender address: " << mail_result.sender_addr << endl;
	cout << "receiver list: ";
	for (int i = 0; i < mail_result.receiver.size();i++)
	{
		cout << mail_result.receiver[i] << " ";
	}
	cout << endl;
	cout << "date: " << mail_result.date << endl;
	cout << "Cc address: ";
	for (int i = 0; i < mail_result.Cc_list.size();i++)
	{
		cout << mail_result.Cc_list[i] << " ";
	}
	cout << endl;
	cout << "Subject: " << mail_result.subject << endl;
	cout << "main text: " << mail_result.text_body << endl;;
	cout << "attach list: "; 
	for (int i = 0; i < mail_result.file_list.size();i++)
	{
		cout << mail_result.file_list[i] << " ";
	}
	cout << endl;
}

/*
int main()
{
//以下用于发送邮件的测试
TestSendMail();
testReceive();
return 0;
}
*/

int main()
{
	message_info message;
	sMailInfo    info;
	API api;
	vector<string> x;

	info.mail_server_name = "smtp.163.com"; // 163服务器地址 ！！！
	info.mailbox_sender = "qzh951120@163.com";   // 发送者 ！！1
	info.mailbox_user_name = "qzh951120@163.com";    // 用户名 ！！
	info.mailbox_user_pwd = "qzh9511200818";   //密码 ！！1
	info.receiver = "xlq1120@yahoo.com";
	info.Cc_addr = ""; //
	info.mailbox_sender_show_name = "邱小亮";    //发送显示 的名字
	info.mail_title = "hello,祝你期末考试顺利";
	info.mail_body = "这是一封来自于cpp测试小组的邮件，考试顺利，心情愉快。\n";
	api.set(info);
/*	
	if (!api.isOnline(info))
		cout << api.Error_info << endl;
	else
		cout << "OK" << endl;
	//if (!api.sendMessage(info, x))
		//cout << api.Error_info << endl;
	if(!api.fetch_file(info))
		cout << api.Error_info << endl;
	system("pause");
	//TestSendMail();
	testReceive();
*/	
	Form mw;
	mw.showmain();
//	system("pause");
	return 0;
}