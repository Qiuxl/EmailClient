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
	info.mail_server_name = "smtp.163.com"; // 163��������ַ
	info.mailbox_sender = "qzh951120@163.com";   // ������
	info.mailbox_user_name = "qzh951120@163.com";    // �û���
	info.mailbox_user_pwd = "qzh9511200818";   //����
	info.receiver = "xlq1120@yahoo.com";
	info.Cc_addr = "";
	info.mailbox_sender_show_name = "��С��";    //������ʾ ������
	info.mail_title = "hello,ף����ĩ����˳��";
	info.mail_body = "����һ��������cpp����С����ʼ�������˳����������졣\n";
	//����Ϊ���洫�ݸ��ҵ���Ϣ
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
	if (!temp.create_socket())  //�����ȴ���Э���õ��׽��֣�ʧ�ܿ���������û����
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
	Sleep(1000);     //�Ե�һ��
	int max_mail = temp.get_mail_sum();  //��ȡ��ǰ�������ϵ��ʼ���Ŀ
	temp.fetch_mail(max_mail, mail_result);   //����fetch������ö�Ӧid���ʼ�
	temp.clear();        ///�������׽���

	//������γ���չʾ���
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
//�������ڷ����ʼ��Ĳ���
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

	info.mail_server_name = "smtp.163.com"; // 163��������ַ ������
	info.mailbox_sender = "qzh951120@163.com";   // ������ ����1
	info.mailbox_user_name = "qzh951120@163.com";    // �û��� ����
	info.mailbox_user_pwd = "qzh9511200818";   //���� ����1
	info.receiver = "xlq1120@yahoo.com";
	info.Cc_addr = ""; //
	info.mailbox_sender_show_name = "��С��";    //������ʾ ������
	info.mail_title = "hello,ף����ĩ����˳��";
	info.mail_body = "����һ��������cpp����С����ʼ�������˳����������졣\n";
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