#include "API.h"

int API::decode_time(const char* time)//��ȡʱ���ַ�������Ϊʱ����ֵ 
{
	char week[5],ch;
	char month_n[5];
	int year, date, hour, minute, second, month;
	int result = 0;
	sscanf(time, " %c", &ch);
	if (ch >= '0'&&ch <= '9')
	{
		sscanf(time, "%d-%02d-%02d %02d:%02d:%02d", &year, &month, &date, &hour, &minute, &second);
	}
	else
	{
		sscanf(time, " %s%d %s%d %d:%d:%d", week, &date, month_n, &year, &hour, &minute, &second);//��ȡ�ַ�����Ϣ 
		if (!strcmp(month_n, "Jan"))      month = 1;
		else if (!strcmp(month_n, "Feb")) month = 2;
		else if (!strcmp(month_n, "Mar")) month = 3;
		else if (!strcmp(month_n, "Apr")) month = 4;
		else if (!strcmp(month_n, "May")) month = 5;
		else if (!strcmp(month_n, "Jun")) month = 6;
		else if (!strcmp(month_n, "Jul")) month = 7;
		else if (!strcmp(month_n, "Aug")) month = 8;
		else if (!strcmp(month_n, "Sep")) month = 9;
		else if (!strcmp(month_n, "Oct")) month = 10;
		else if (!strcmp(month_n, "Nov")) month = 11;
		else if (!strcmp(month_n, "Dec")) month = 12;
		else month = 0;
	}

	result = (((((year - 2000) * 12 + month - 1) * 31 + date - 1) * 24 + hour) * 60 + minute) * 60 + second;//������ 
	return result;
}
bool API::fetch_file(sMailInfo& mail)
{
	string path;
	cout << "���ڽ������ʼ������Ե�..."<< endl;
	get_root(mail.mailbox_user_name);
	mail.mail_server_name = "pop3.163.com";
	path = rootaddress + "attachment//";
	_mkdir(path.data());
	Pop_receiver temp(mail.mailbox_user_name, mail.mailbox_user_pwd, path.c_str(), mail.mail_server_name);
	if (!temp.create_socket())  //�����ȴ���Э���õ��׽��֣�ʧ�ܿ���������û����
	{
		Error_info = "create socket failed";
		return false;
	}
	if (!temp.login())
	{
		Error_info = "user name or password wrong!";
		return false;
	}
	Sleep(1000);     //�Ե�һ��
	int newest_mail_time = file_info.get_newest_inbox();//��ʾ�ļ��������µ�����ʱ��
	int max_mail = temp.get_mail_sum();  //��ȡ��ǰ�������ϵ��ʼ���Ŀ
	int this_mail_time;
	int count = 5;
	while(max_mail>0&&count>0)
	{
		temp.fetch_mail(max_mail, message);   //����fetch������ö�Ӧid���ʼ�
		//cout <<"�ż�ʱ�䣺"<< message.date << endl;
		this_mail_time = decode_time(message.date.c_str());
		//cout << "�����ʱ�����"<<this_mail_time << endl;
		if (this_mail_time > newest_mail_time)
			file_info.write_inbox(message, this_mail_time);
		else
			break;
		max_mail--;
		count--;
	}
	temp.clear();        ///�������׽���
	return true;
}
void API::get_root(const char* user_name)
{
	char root[MAX_PATH];
	string receive, send;
	rootaddress = string(user_name) + "\\";
	receive = rootaddress + "receive";
	send = rootaddress + "send";
	_mkdir(rootaddress.data());
	_mkdir(receive.data());
	_mkdir(send.data());
	file_info.setRoot(rootaddress.data());
}
API::API()/*���캯��*/
{
	username = "Empty";
}
API::~API()/*��������*/
{
	//��
}
bool API::isOnline(sMailInfo &mail)
{
	mail.mail_server_name = "smtp.163.com";
	mail_info.set(mail);//�û������������������
	username = mail.mailbox_user_name;
	if(!mail_info.CReateSocket())
		return false;
	//get_root(username.data());
	if (mail_info.Logon())
	{
//		file_info.set_info(mail);
		fetch_file(mail);
		return true;
	}
	else
	{
		Error_info = "Logging failed! You are offline!";
		return false;
	}
}
bool API::logon(sMailInfo& mail)
{
	if (isOnline(mail))
	{
		if (file_info.check_info(mail))
		{
			if (!file_info.check_password(mail.mailbox_user_pwd))
			{
				file_info.set_info(mail);
			}
		}
		else
		{
			file_info.set_info(mail);
		}
		return true;
	}
	else
	{
		if (file_info.check_info(mail) && file_info.check_password(mail.mailbox_user_pwd))
		{
			file_info.get_info(mail);
			return true;
		}
		return false;
	}
}
bool API::set(const sMailInfo& mail)
{
	//cout << "����" << endl;
//	cout << mail.mailbox_user_name << endl;
//	get_root(mail.mailbox_user_name);
	if (file_info.set_info(mail))
		return true;
	else
		return false;
}
bool API::sendMessage(sMailInfo mail,vector<string>& file_path)//�����ѽ����õĲ������ú���
{
	mail.mail_server_name = "smtp.163.com";
	if (mail.mailbox_sender_show_name == NULL)
		strcpy(mail.mailbox_sender_show_name, mail.mailbox_user_name);
	mail_info.set(mail);
	for (int i = 0; i < file_path.size(); i++)
	{
		mail_info.AddFilePath((char *)file_path[i].c_str());
	}
	if (!mail_info.BeginSendMail())
	{
		Error_info = "send mail failed";
		return false;
	}
	else
	{
		set(mail);
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		char time[30];
		sprintf(time,"%d-%02d-%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		time_sign = decode_time(time);
		message.receiver = mail.reveiver_name;
		message.Cc_list = mail.Cc_list;
		for (int i = 0; i < file_path.size(); i++)
		{
			message.file_list.push_back(file_path[i]);
		}
		message.date = time;
		message.subject = mail.mail_title;
		message.text_body = mail.mail_body;
		file_info.write_outbox(message, time_sign);
		return true;
	}
}
bool API::getSendMail(message_info&info, int num)
{
	file_info.read_outbox(info, num);
	return true;
}

bool API::getReceiveMail(message_info&info, int num)/*���ص�num���ʼ����ݵ����ã��ϲ����forѭ��ʵ��*/
{
	file_info.read_inbox(info, num);
	return true;
}
int API::getSendNum()/*���ط�������*/
{
	return file_info.get_num_outbox();
}
int API::getReceiveNum()/*�����ռ�����*/
{
	return file_info.get_num_inbox();
}
bool API::deleteSendMail(string time)/*ɾ��ʱ��ΪXX����*/
{
	time_sign = decode_time(time.data());
	file_info.delete_outbox(time_sign);
	return true;
}
bool API::deleteReceiveMail(string time)/*ɾ��ʱ��ΪXX����*/
{
	time_sign = decode_time(time.data());
	file_info.delete_inbox(time_sign);
	return true;
}
bool API::deleteUserInfo()/*ɾ���û���Ϣ*/
{
	string info;
	info = "del "+rootaddress;
	system(info.data());
	return true;
}

