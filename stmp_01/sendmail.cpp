// about function gethostbyname 
// test connect to mail server
#include "iostream"
#include <string>
#include"stdafx.h"
#include"sendmail.h"
#include<fstream>
#include"Pop3.h"
#include<atlenc.h>
#include<winsock.h>
using namespace std;

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"msvcrtd.lib")


CSendMail::CSendMail()
{
	file_buffer_ = NULL;
	memset(send_buff_, 0, sizeof(send_buff_));
	memset(receive_buff_, 0, sizeof(receive_buff_));
}

CSendMail::~CSendMail()
{
	//DeleteAllPath();
}
void CSendMail::set(sMailInfo tmp)
{
	mail_info_ = tmp;   //有些成员是指针，因此在程序结束前不能释放参数
}
void CSendMail::GetFileName(char* file_name, char* file_path)
{
	int index;
	if (file_path == NULL || file_name == NULL)
	{
		return;
	}
	// 找到第一个不是  //的地方
	for (index = 0; index < (int)strlen(file_path); index++)
	{
		if (file_path[strlen(file_path) - 1 - index] == '\\')
		{
			memcpy(file_name, &file_path[strlen(file_path) - index], index);
			return;
		}
	}
	if (index == (int)strlen(file_path))
	{
		memcpy(file_name, file_path, strlen(file_path));
	}
}
void  CSendMail::Encode(char *buff_out, const char* buff_in, int DataByte)
{
	//编码表
	const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	string strEncode;
	unsigned char Tmp[4] = { 0 };
	int LineLength = 0;
	for (int i = 0; i<(int)(DataByte / 3); i++)
	{
		Tmp[1] = *buff_in++;
		Tmp[2] = *buff_in++;
		Tmp[3] = *buff_in++;
		strEncode += EncodeTable[Tmp[1] >> 2];
		strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode += EncodeTable[Tmp[3] & 0x3F];
		if (LineLength += 4, LineLength == 76)
		{
			strEncode += "\r\n";
			LineLength = 0;
		}
	}
	//对剩余数据进行编码
	int Mod = DataByte % 3;
	if (Mod == 1)
	{
		Tmp[1] = *buff_in++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode += "==";
	}
	else if (Mod == 2)
	{
		Tmp[1] = *buff_in++;
		Tmp[2] = *buff_in++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode += "=";
	}
	for (int i = 0; i<strEncode.length(); i++)
	{
		*buff_out++ = strEncode[i];
	}
	*buff_out = '\0';
}

void CSendMail::AddFilePath(char *file_path)
{
	if (file_path == NULL)
	{
		return;
	}
	int i;
	char *temp;
	for (i = 0; i < file_path_list_.size(); i++)
	{
		temp = file_path_list_[i];
		if (strcmp(temp, file_path) == 0)
		{
			return;
		}
	}
	file_path_list_.push_back(file_path);
}
void CSendMail::DeleteFilePath(char* file_path) // 删除附件路径  
{
	int i;
	char* temp;
	for (int index = 0; index < file_path_list_.size(); index++)
	{
		temp = file_path_list_[index];
		if (strcmp(file_path, temp) == 0)
		{
			file_path_list_.erase(file_path_list_.begin() + index);
		}
	}
}
void CSendMail::DeleteAllPath(void)  //删除掉所有的现有路径
{
	vector<char *>().swap(file_path_list_);
}
int CSendMail::GetFileData(char *file_path)
{
	if (file_path == NULL)
	{
		return -1;
	}
	fstream file_in(file_path, ios::in | ios::binary);
	if (!file_in.is_open())
		return -1;
	file_in.seekg(0, ios::end);
	int file_length = file_in.tellg();  //返回读指针当前的位置
	file_in.seekg(0, ios::beg);
	file_buffer_ = new char[file_length + 1];
	file_in.read(file_buffer_, file_length);    //按照字节读入
	file_in.close();
	//file_buffer_[file_length] = '\0';
//	cout << file_buffer_ << endl;
	return file_length;
}

bool CSendMail::CReateSocket()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);//将2和2凑成一个16位的word形变量，vc6.0认为word占了两个字节
	err = WSAStartup(wVersionRequested, &wsaData);  //加载socket库
	if (err != 0)
	{
		cout << "SOCKET improt failed\n";
		return false;
	}
	//WSACleanup();  //s释放的方法可以用这个
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		cout << "WSA failed\n";
		WSACleanup();
		return false;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sock == INVALID_SOCKET)
	{
		cout << "SOCK invalid!\n";
		return false;
	}
	//配置监听地址窗口和端口
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(25);   // smtp端口是25  
	if (mail_info_.mail_server_name == "") //绑定ip地址
	{
		server_addr.sin_addr.s_addr = inet_addr(mail_info_.mail_server_ip_addr);    // 直接使用IP地址  
	}
	else
	{
		struct hostent *hp = gethostbyname(mail_info_.mail_server_name);    // 使用名称 
		if (!hp)
		{
			return false;
		}
		server_addr.sin_addr.s_addr = *(int*)(*hp->h_addr_list);
		char* ip = inet_ntoa(server_addr.sin_addr);
		strcpy(mail_info_.mail_server_ip_addr, ip);
	}
	//请求与服务器端进行连接，
	int ret = connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));  // 建立连接  
//	cout << ret << endl;
	if (ret == SOCKET_ERROR)
	{
		cout << "connect failed\n";
		return false;
	}

	return true;
}
bool CSendMail::Logon()
{
	recv(sock, receive_buff_, 1024, 0);
	//接收数据
	memset(send_buff_, 0, sizeof(send_buff_));
	sprintf_s(send_buff_, "HELO %s\r\n", mail_info_.mail_server_ip_addr);
	send(sock, send_buff_, strlen(send_buff_), 0);   // 开始会话  
	memset(receive_buff_, 0, sizeof(receive_buff_));
	recv(sock, receive_buff_, 1024, 0);
	// 250
	if (receive_buff_[0] != '2' || receive_buff_[1] != '5' || receive_buff_[2] != '0')
	{
	//	cout << receive_buff_ << endl;
	//	cout << "error！" << endl;
		return false;
	}

	memset(send_buff_, 0, sizeof(send_buff_));
	sprintf_s(send_buff_, "AUTH LOGIN\r\n");
	send(sock, send_buff_, strlen(send_buff_), 0);    // 请求登录  
	recv(sock, receive_buff_, 1024, 0);
	//334
	if (receive_buff_[0] != '3' || receive_buff_[1] != '3' || receive_buff_[2] != '4')
	{
		cout << receive_buff_ << endl;
	//	cout << "error2" << endl;
		return false;
	}

	memset(send_buff_, 0, sizeof(send_buff_));
	Encode(send_buff_, mail_info_.mailbox_user_name, strlen(mail_info_.mailbox_user_name));
	send_buff_[strlen(send_buff_)] = '\r';
	send_buff_[strlen(send_buff_)] = '\n';
//	cout << "user name after encode! " << send_buff_ << endl;
	send(sock, send_buff_, strlen(send_buff_), 0);   // 发送用户名  
	recv(sock, receive_buff_, 1024, 0);
	//334
	//cout << receive_buff_ << endl;
	if (receive_buff_[0] != '3' || receive_buff_[1] != '3' || receive_buff_[2] != '4')
	{

		cout << receive_buff_ << endl;
		cout << "error3" << endl;
		return false;
	}
	//cXpoOTUxMTIwMDgxOA==
	memset(send_buff_, 0, sizeof(send_buff_));
	Encode(send_buff_, mail_info_.mailbox_user_pwd, strlen(mail_info_.mailbox_user_pwd));
//	cout << "pwd after encode :" << send_buff_ << endl;
	send_buff_[strlen(send_buff_)] = '\r';
	send_buff_[strlen(send_buff_)] = '\n';
	send(sock, send_buff_, strlen(send_buff_), 0);  // 发送用户密码  
	recv(sock, receive_buff_, 1024, 0);
	//cout << receive_buff_ << endl;
	//235
	if (receive_buff_[0] != '2' || receive_buff_[1] != '3' || receive_buff_[2] != '5')
	{
		cout << receive_buff_ << endl;
		return false;
	}
	return true;    // 登录成功  
}
bool CSendMail::SendHead()
{
	int rt;
	string mail_to = "";
	string rcpt_to = "";
	memset(send_buff_, 0, sizeof(send_buff_));
	//指定发送者
	sprintf_s(send_buff_, "MAIL FROM:<%s>\r\n", mail_info_.mailbox_sender);
	//cout << send_buff_ << endl;
	rt = send(sock, send_buff_, strlen(send_buff_), 0);   //发送成功返回成功发送的字节数
	// 第四个参数一般置为0
	if (rt != strlen(send_buff_))
	{
		return false;
	}
	recv(sock, receive_buff_, 1024, 0);
	//cout << receive_buff_ << endl;
	memset(send_buff_, 0, sizeof(send_buff_));
	for (int i = 0; i < mail_info_.reveiver_name.size(); i++)
	{
		rcpt_to += "RCPT TO:<" + mail_info_.receiver_list[i] + ">\r\n";
	}
	for (int i = 0; i < mail_info_.Cc_list.size(); i++)
	{
		rcpt_to += "RCPT TO:<" + mail_info_.Cc_list[i] + ">\r\n";
	}
	rcpt_to.copy(send_buff_, rcpt_to.length());
	//sprintf_s(send_buff_, "RCPT TO:<%s>\r\n", mail_info_.mailbox_receiver);
	//cout << send_buff_ << endl;
	rt = send(sock, send_buff_, strlen(send_buff_), 0);
	if (rt != strlen(send_buff_))
	{
		return false;
	}
	recv(sock, receive_buff_, 1024, 0);
	//cout << receive_buff_ << endl;
	// 1024 是 receive_buff_的大小

	// 应该受到 should recv "250 Mail OK"
	memset(send_buff_, 0, sizeof(send_buff_));
	memcpy(send_buff_, "DATA\r\n", strlen("DATA\r\n"));
	rt = send(sock, send_buff_, strlen(send_buff_), 0);
	if (rt != strlen(send_buff_))
	{
		return false;
	}
	recv(sock, receive_buff_, 1024, 0);
//	cout << receive_buff_ << endl;
	memset(send_buff_, 0, sizeof(send_buff_));
	// 两个不同版本的sprintf_s
	mail_to += "From:\"" + string(mail_info_.mailbox_sender_show_name) + "\"<" + string(mail_info_.mailbox_sender) + ">\r\n";
	//sprintf_s(send_buff_, "From:\"%s\"<%s>\r\n", mail_info_.mailbox_sender_show_name, mail_info_.mailbox_sender);
	mail_to += "To:";
	for (int i = 0; i < mail_info_.reveiver_name.size(); i++)
	{
		mail_to += "\"" + mail_info_.reveiver_name[i] + "\"<";
		mail_to += mail_info_.receiver_list[i] + ">;";
	}
	mail_to += "\r\n";

	//sprintf_s(&send_buff_[strlen(send_buff_)], 150, "To:\"INVT.COM.CN\"<%s>\r\n", mail_info_.mailbox_receiver);
	if (mail_info_.Cc_list.size() > 0)  //加上抄送人
	{
		for (int i = 0; i < mail_info_.Cc_list.size(); i++)
		{
			if (i == 0)
			{
				mail_to += "Cc:";
			}
			mail_to += "\"" + mail_info_.Cc_name[i] + "\"<";
			mail_to += mail_info_.Cc_list[i] + ">;";
		}
	}
	if (mail_info_.Cc_list.size() > 0){
		mail_to += "\r\n";
	}
	mail_to.copy(send_buff_, mail_to.length());
	sprintf_s(&send_buff_[strlen(send_buff_)], 150, "Subject:%s\r\nMime-Version: 1.0\r\nContent-Type: multipart/mixed;\r\n boundary=\"QXL_INVT\"\r\n\r\n", mail_info_.mail_title);
	rt = send(sock, send_buff_, strlen(send_buff_), 0);
	if (rt != strlen(send_buff_))
	{
		return false;
	}
	//cout << send_buff_ << endl;
	return true;
}

bool CSendMail::SendTextBody()
{
	int rt;
	memset(send_buff_, 0, sizeof(send_buff_));
	sprintf_s(send_buff_, "--QXL_INVT\r\nContent-Type: text/plain;\r\n  charset=\"gb2312\"\r\n\r\n%s\r\n\r\n", mail_info_.mail_body);
	rt = send(sock, send_buff_, strlen(send_buff_), 0);
//	cout << rt << endl;
	if (rt != strlen(send_buff_))
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool CSendMail::SendFileBody()
{
	char* file_path;
	int rt;
	int len;
	char file_name[128];
	int pt = 0;
	int dest_length = 0;

	for (int index = 0; index < file_path_list_.size(); index++)
	{
		pt = 0;
		memset(file_name, 0, 128);
		file_path = file_path_list_[index];
		len = GetFileData(file_path);
		GetFileName(file_name, file_path);

		sprintf_s(send_buff_, "--QXL_INVT\r\nContent-Type: application/octet-stream;\r\n  name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment;\r\n  filename=\"%s\"\r\n\r\n", file_name, file_name);
		send(sock, send_buff_, strlen(send_buff_), 0);
		while (pt < len)
		{
			int capacity = sizeof(send_buff_);
			memset(send_buff_, 0, sizeof(send_buff_));

			// 760为1024的最大整数,(760/4)*3=570  
			dest_length = 8192;
			Base64Encode((BYTE*)file_buffer_ + pt, min(len - pt, 570), send_buff_, &dest_length);
			int len1 = strlen(send_buff_);
			//cout << send_buff_ << endl;
			pt += min(len - pt, 570);
			if (pt == len)
			{
				send_buff_[len1] = '\r';
				send_buff_[len1 + 1] = '\n';
				send_buff_[len1 + 2] = '\0';
			}
			else
				send_buff_[len1] = '\0';

			rt = send(sock, send_buff_, strlen(send_buff_), 0);
			if (rt != strlen(send_buff_))
			{
				return false;
			}
		}
		if (len != 0)
		{
			delete[] file_buffer_;
		}
	}

	return true;
}


bool CSendMail::SendEnd()
{
	sprintf_s(send_buff_, "--QXL_INVT--\r\n.\r\n");
	send(sock, send_buff_, strlen(send_buff_), 0);
	recv(sock, receive_buff_, 1024, 0);
	cout << receive_buff_ << endl;
	sprintf_s(send_buff_, "QUIT\r\n");

	send(sock, send_buff_, strlen(send_buff_), 0);
	recv(sock, receive_buff_, 1024, 0);
	cout << receive_buff_ << endl;
	closesocket(sock);
	WSACleanup();
	return true;
}

//sMailInfo &smail_info
bool CSendMail::BeginSendMail()
{
	//memcpy(&mail_info_, &smail_info, sizeof(smail_info));
	CReateSocket();
	Logon();
	mail_info_.get_receiverlist();
	mail_info_.get_Cc_list();
	if (mail_info_.mail_body == NULL
		|| mail_info_.mail_server_ip_addr == NULL
		|| mail_info_.mail_server_name == NULL
		|| mail_info_.receiver_list.size() <= 0
		|| mail_info_.mailbox_sender == NULL
		|| mail_info_.mailbox_sender_show_name == NULL
		|| mail_info_.mail_title == NULL
		|| mail_info_.mailbox_user_name == NULL
		|| mail_info_.mailbox_user_pwd == NULL)
	{
		cout << "information failed " << endl;
		return false;
	}

//	SOCKET sock;
	if (!CReateSocket()) // 建立连接  
	{
		cout << "SOCKET failed\n";
		return false;
	}

	if (!Logon())        // 登录邮箱  
	{
		cout << "login filed!" << endl;
		return false;
	}

	if (!SendHead())     // 发送邮件头  
	{
		cout << "send head failed " << endl;
		return false;
	}

	if (!SendTextBody()) // 发送邮件文本部分  
	{
		cout << "send text failed!" << endl;
		return false;
	}

	if (!SendFileBody()) // 发送附件  
	{
		return false;
	}

	if (!SendEnd())    // 结束邮件，并关闭sock  
	{
		return false;
	}

	return true;
}

// 对smailinfo的初始化
void sMailInfo::get_receiverlist()
{
//	cout << "please input the receiver addrees,if more than one,seperated with ;" << endl;
//	cout << "receiver address:" << endl;
//	string temp;
	//cin >> temp;    //默认@前一部分的名字为收件人名字
	string address;
	int index;
	while (1)
	{
		index = receiver.find_first_of(';');
		if (index != std::string::npos)   //多个收件人
		{
			address.assign(receiver, 0, index);
			cout << address << endl;
			receiver_list.push_back(address);
			int name_end = address.find('@');
			string name;
			name.assign(address, 0, name_end);
			reveiver_name.push_back(name);
			receiver.erase(0, index + 1);
		}
		else{
			break;
		}
	}
	//处理最后一个收件人
	index = receiver.find('@');
	string name;
	name.assign(receiver, 0, index);
	reveiver_name.push_back(name);
	receiver_list.push_back(receiver);
}
void sMailInfo::get_Cc_list()
{
//	cout << "please input the carbon copy addrees,if more than one,seperated with ;" << endl;
//	cout << "carbon copy addrees: " << endl;
	//string temp;
	string address;
	if (Cc_addr.length() == 0)
	{
		return;
	}
	//cin >> temp;    //默认@前一部分的名字为收件人名字
	int index;
	while (1)
	{
		index = Cc_addr.find_first_of(';');
		if (index != std::string::npos)   //多个收件人
		{
			address.assign(Cc_addr, 0, index);
		//	cout << address << endl;
			Cc_list.push_back(address);
			int name_end = address.find('@');
			string name;
			name.assign(address, 0, name_end);
			Cc_name.push_back(name);
			Cc_addr.erase(0, index + 1);
		}
		else{
			break;
		}
	}
	//处理最后一个收件人
	index = Cc_addr.find('@');
	string name;
	name.assign(Cc_addr, 0, index);
	Cc_name.push_back(name);
	Cc_list.push_back(Cc_addr);
}


//cXpoOTUxMTIwQDE2My5jb20 =


//int main(int argc, _TCHAR* argv[])
//{
//	TestSendMail();
//	cout << "send successfully" << endl;
//	system("pause");
//	return 0;
//}


/*
int main()
{
	message_info result;
	Pop_receiver temp("qzh951120@163.com", "qzh9511200818", "qzh951120\\attachment\\","pop3.163.com");
	temp.create_socket();
	temp.login();
	Sleep(1000); 
	int max_mail = temp.get_mail_sum();
	temp.fetch_mail(max_mail, result);
	system("pause");
	return 0;
}
*/