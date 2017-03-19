
#include"stdafx.h"
#include"Pop3.h"
#include<Windows.h>
#include<sstream>
//创建套接字
Pop_receiver::Pop_receiver(string user, string pwd, string directory, string Sname, string Sip)
{
	my_directory = directory;
	user_name = user;
	server_name = Sname;
	user_pwd = pwd;
	server_ip = Sname;
}
int Pop_receiver::get_mail_sum()  //返回邮件总数
{
	get_mailinfo();  //必须先调用该函数才能获取邮箱目前的邮件数目和邮箱的大小
	return mail_num;
}
//清除套接字
void Pop_receiver::clear()
{
	closesocket(sock);
	WSACleanup();
}
bool Pop_receiver::create_socket()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	memset(send_buffer, 0,sizeof(send_buffer));
	memset(receive_buffer, 0, sizeof(receive_buffer));
	wVersionRequested = MAKEWORD(2, 2);//将2和2凑成一个16位的word形变量，vc6.0认为word占了两个字节
	err = WSAStartup(wVersionRequested, &wsaData);  //加载socket库
	if (err != 0)
	{
		return false;
	}
	//WSACleanup();  //s释放的方法可以用这个
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return false;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sock == INVALID_SOCKET)
	{
		return false;
	}
	//配置监听地址窗口和端口
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(110);   // 发邮件一般都是25端口，SSL的是465端口  
	if (server_name == "") //绑定ip地址
	{
		server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());    // 直接使用IP地址  
	}
	else
	{
		struct hostent *hp = gethostbyname(server_name.c_str());    // 使用名称  
		server_addr.sin_addr.s_addr = *(int*)(*hp->h_addr_list);
		char* ip = inet_ntoa(server_addr.sin_addr);
		server_ip = string(ip);
	}
	//请求与服务器端进行连接，
	int ret = connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));  // 建立连接  
	//cout << ret << endl;
	recv(sock, receive_buffer, 1024, 0);
	if (ret == SOCKET_ERROR)
	{
		cout << "connect error !" << endl;
		closesocket(sock);
		WSACleanup();
		return false;
	}
//	cout << receive_buffer << endl;
	return true;
}
//对于登陆函数的初始化，
// USER 邮箱地址
// PASS 密码
bool Pop_receiver::login()
{
	int rt;
	if (user_name == "" || user_pwd == "")
	{
		cout << "用户名或者密码不能为空!" << endl;
		return false;
	}
	string send_usr = "USER " + user_name + "\r\n";
	send_usr.copy(send_buffer, send_usr.length());
	send_buffer[send_usr.length()] = '\0';
	//cout << send_buffer << endl;
	send(sock, send_buffer, strlen(send_buffer), 0);
	memset(receive_buffer, 0, sizeof(receive_buffer));
	recv(sock, receive_buffer, 1024, 0);
	if (receive_buffer[0] != '+' || receive_buffer[1] != 'O' || receive_buffer[2] != 'K')
	{
		cout << "非法邮箱名字！" << endl;
		return false;
	}
	memset(send_buffer, 0, sizeof(send_buffer));
	string send_pwd = "PASS " + user_pwd + "\r\n";
	send_pwd.copy(send_buffer, send_pwd.length());
	send_buffer[send_pwd.length()] = '\0';
	rt= send(sock, send_buffer, strlen(send_buffer), 0);
	if (rt != strlen(send_buffer)){
		return false;
	}
//	cout << send_buffer << endl;
	memset(receive_buffer, 0, sizeof(receive_buffer));  
	recv(sock, receive_buffer, 1024, 0);
//	cout << receive_buffer << endl;
	if (receive_buffer[0] != '+' || receive_buffer[1] != 'O' || receive_buffer[2] != 'K')
	{
		cout << "密码不正确" << endl;
		return false;
	}
	return true;
}
//STATNone处理请求服务器发回关于邮箱的统计资料，如邮件总数和总字节数
bool Pop_receiver::get_mailinfo()
{
	int rt;
	mail_num = 0;
	mail_size = 0;
	string mail_instruct;
	memset(send_buffer, 0, sizeof(send_buffer));
	memset(receive_buffer, 0, sizeof(receive_buffer));
	mail_instruct = "STATNone\r\n";
	mail_instruct.copy(send_buffer, mail_instruct.length());
	rt=send(sock, send_buffer, strlen(send_buffer), 0);
	if (rt != strlen(send_buffer))
	{
		return false;
	}
	recv(sock, receive_buffer, 1024, 0);
//cout << receive_buffer << endl;
	string temp(receive_buffer);
	int index = temp.find(' ');  // 格式如下  +ok 156 238237
	temp.erase(0, index + 1);
	index = temp.find(' ');
	for (int i = 0; i < index; i++)
	{
		mail_num = mail_num * 10 + temp[i] - '0';
	}
	temp.erase(0, index + 1);  // 238237
	for (int i = 0; i < temp.length(); i++)
	{
		
		if (temp[i]>='0'&&temp[i] <= '9')
			mail_size = mail_size * 10 + temp[i] - '0';
		else
			break;
	}
	//cout << mail_num << " " << mail_size << endl;
	return true;
}
bool Pop_receiver::get_mailmessage(int mail_id)
{
	memset(send_buffer, 0, sizeof(send_buffer));
	memset(receive_buffer, 0, sizeof(receive_buffer));
	//一下用于获取第n封邮件的信息
	Sleep(300);
	int rt;
	int message_size = 0; //邮件数目
	stringstream ss;

	ss << mail_id;
	string num_string, send_instruct, temp;//temp用于构造返回消息
	num_string = ss.str();
	send_instruct = "LIST " + num_string + "\r\n";
	send_instruct.copy(send_buffer, send_instruct.length());
	rt = send(sock, send_buffer, strlen(send_buffer), 0);
	if (rt != strlen(send_buffer))
	{
		return false;
	}
	recv(sock, receive_buffer, 1024, 0);
	string for_judge;
	for_judge = string(receive_buffer);
	if (for_judge.find("+OK")==std::string::npos)
	{	
		return false;
	}
	temp = string(receive_buffer);
	int index = temp.find(' ');
	temp.erase(0, index + 1);
	index = temp.find(' ');
	temp.erase(0, index + 1);
	for (int i = 0; i < temp.length(); i++)
	{

		if (temp[i] >= '0'&&temp[i] <= '9')
			message_size = message_size * 10 + temp[i] - '0';
		else
			break;
	}
	char *tmp_receive = new char[message_size + 20]; //收邮件信息之前有 +ok 和邮件大小的信息
	memset(tmp_receive, 0, message_size + 1);
	//发送获取邮件信息的命令
	send_instruct = "RETR " + num_string + "\r\n";
	memset(send_buffer, 0, sizeof(send_buffer));
	send_instruct.copy(send_buffer, send_instruct.length());
	rt = send(sock, send_buffer, strlen(send_buffer), 0);
	if (rt != strlen(send_buffer))
	{
		return false;
	}
	Sleep(1000);
	recv(sock, tmp_receive, message_size + 19, 0);
	//cout << tmp_receive << endl;
	//需要两次接收，第一次会接收到 +OK 2885 octets +ok表示成功
	if (tmp_receive[0] != '+' || tmp_receive[1] != 'O' || tmp_receive[2] != 'K')
	{
		return false;
	}
	mail_message.clear();
	while (mail_message.length() < message_size)
	{
		mail_message += string(tmp_receive);
		if (mail_message.length() >= message_size)
		{
			break;
		}
		memset(tmp_receive, 0, sizeof(tmp_receive));
		if (mail_size > 100000)
		{
			Sleep(1000);   //停顿
		}
		recv(sock, tmp_receive, message_size + 19, 0);
	}

	tmp_receive[message_size + 19] = '\0';
	message_size = 0;
	//cout << mail_message << endl;
	delete []tmp_receive; //释放内存
	memset(send_buffer, 0, sizeof(send_buffer));
	memset(receive_buffer, 0, sizeof(receive_buffer));
	return true;
}
//base 64解码函数
//reference online
string Pop_receiver::base64_decode( const string &encoded_string) {
	const string base64_alphbat= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;
	while (in_len-- && (encoded_string[in_] != '=') && isbase64_code(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_alphbat.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64_alphbat.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}
// 解析邮件
void Pop_receiver::mail_analysis()
{
	string temp = "", boundary, file_buffer;
	int index_1, index_2, index_3, index_4;
	// 解析邮件头
	string tmp1;
	boundary_1 = "";
	boundary_2 = "";

	//date 格式 Date: Mon, 13 Jun 2016 11:39:03 +0800
	index_1 = mail_message.find("\nDate:");
	index_2 = mail_message.find('\n', index_1 + 1);
	temp.assign(mail_message, index_1 + 1, index_2 - index_1 - 2);
	index_3 = temp.find('+');
	temp.erase(index_3 - 1, temp.length());
 	mail_result.date.assign(temp, 6, temp.length() - 6);
	//	mail_message.erase(0, index_2);
	//cout << mail_message << endl;

	//找到from句子
	index_1 = mail_message.find("\nFrom:");
//	cout << index_1 << endl;
	index_2 = mail_message.find('\n', index_1 + 1);
	temp.assign(mail_message, index_1 + 1, index_2 - index_1 - 2);
//	cout << temp << endl;
	get_sender_info(temp);
	//	mail_message.erase(0, index_2);

	//To 语句格式： 获取To：list，逗号隔开
	//To: qzh951120 <qzh951120@163.com>, 
	//	xlq1120 <xlq1120@yahoo.com>,
	//		615723297 <615723297@qq.com>
	//防止有多个收件人，只需要找到下一个冒号前的东西copy即可
	index_1 = mail_message.find("\nTo:");
	index_2 = mail_message.find(':', index_1 + 4);
	temp.assign(mail_message, index_1, index_2 - index_1);
	mail_result.receiver.clear();
	 get_recv_Cc_list(temp, mail_result.receiver);

	//Cc:抄送人语句格式跟to一样
	index_1 = mail_message.find("\nCc:");
	if (index_1 != std::string::npos)
	{
		index_2 = mail_message.find(':', index_1 + 4);
		temp.assign(mail_message, index_1 + 1, index_2 - index_1 - 1);
		mail_result.Cc_list.clear();
		get_recv_Cc_list(temp, mail_result.Cc_list);
	}
	//解析subjct
	index_1 = mail_message.find("\nSubject:");
	index_2 = mail_message.find('\n', index_1 + 1);
	temp.assign(mail_message, index_1, index_2 - index_1);
	mail_result.subject = get_subject(temp);

	//获得分界线 boundary，一般只有一个。boundary=
	index_1 = mail_message.find("boundary=");
	if (index_1 == std::string::npos)
	{
		cout << "mail analysis failed because of no boundary!" << endl;
		return;
	}
	index_2 = mail_message.find('\"', index_1 + 1);
	index_3 = mail_message.find('\"', index_2 + 1);
	boundary_1.assign(mail_message, index_2 + 1, index_3 - index_2 - 1);
	//第一个分界线
	mail_message.erase(0, index_3 + 1);  //可以删除文件头的部分了。
	boundary_1 = "--" + boundary_1;   //邮件格式是boundary前面加上  --
}
//获取邮件正文信息。
void Pop_receiver::get_body()
{
	int index_1, index_2, index_3, flag = 1, index_4;
	int len_1, len_2;
	mail_result.file_list.clear();
	string filename;
	string code_type, tmp, decode, file_result;
	len_1 = boundary_1.length();
	if (boundary_1 == "--QXL_INVT")   //自己写的邮件的分界线，独有的格式，单独分析。
	{
		index_1 = mail_message.find(boundary_1);  //没有附件的话两者之间的原文就是邮件正文
		index_2 = mail_message.find(boundary_1, index_1 + len_1);
		index_3 = mail_message.find("charset=\"gb2312\"", index_1 + len_1);
		mail_result.text_body.assign(mail_message, index_3 + 16, index_2 - index_3 - 16);
		//检测是否有附件
		mail_message.erase(0, index_2);
		index_1 = mail_message.find(boundary_1);
		index_2 = mail_message.find(boundary_1, index_1 + len_1);
		while (index_2 != std::string::npos)
		{
			int a, b;
			a = mail_message.find("filename=");
			b = mail_message.find('\"', a);
			index_3 = mail_message.find('\"', b + 1);
			filename.assign(mail_message, b + 1, index_3 - b - 1);
			filename = get_subject(filename);
			mail_result.file_list.push_back(filename);
			
			decode.assign(mail_message, index_3 + 1, index_2 - index_3 - 1);
			decode = pass_r_n(decode);
			file_result = base64_decode(decode);
			filename = my_directory + filename;
			fstream fout(filename, ios::out | ios::binary);  //二进制的方式写进去
			fout.write(file_result.c_str(), file_result.length());
			mail_message.erase(0, index_2);
			index_1 = mail_message.find(boundary_1);
			index_2 = mail_message.find(boundary_1, index_1 + len_1);
		}
		return;
	}
	//先验证是否有第二个分界线,若有则在第一个之后
	index_1 = mail_message.find(boundary_1);
	index_1 += len_1;
	index_3 = mail_message.find("boundary=", index_1);

	if (index_3 == std::string::npos)
	{
		flag = 0;  //没有第二个分界线
	}

//有两个分界线的情况下，必定有附件编码
	if (flag == 1)
	{
		index_1 = mail_message.find('\"', index_3);
		index_2 = mail_message.find('\"', index_1 + 1);
		boundary_2.assign(mail_message, index_1+1, index_2 - index_1 - 1);
		boundary_2 = "--" + boundary_2;
		mail_message.erase(0, index_2 + 1);
		len_2 = boundary_2.length();
		index_1 = mail_message.find(boundary_2);
		index_2 = mail_message.find("charset=", index_1 + len_2);
		index_3 = mail_message.find('\"', index_2);
		index_2 = mail_message.find('\"', index_3 + 1);
		code_type.assign(mail_message, index_3 + 1, index_2 - index_3 - 1);

		index_4 = mail_message.find("Content-Transfer-Encoding:",index_1+1);
		int index_5 = mail_message.find(boundary_2, index_1 + len_1);
		if (index_4 == std::string::npos || (index_4 > index_5))  //未经过加密，直接取出来。
		{
			index_2 = mail_message.find('\n', index_3);
			index_3 = mail_message.find(boundary_2, index_2);
			mail_result.text_body.assign(mail_message,index_2 + 1, index_3 - index_2 - 1);
			mail_message.erase(0, index_3);
		}
		else
		{
			//获取邮件原文加密方式
			index_2 = mail_message.find(' ', index_4);
			index_3 = mail_message.find('\n', index_2);
			string encrypt_mode;
			encrypt_mode.assign(mail_message,index_2 + 1, index_3 - index_2 - 1);
			if (encrypt_mode.find("base64")==std::string::npos)
			{
				cout << "mail analysis failed because of no boundary!" << endl;
				return;
			}
			else
			{
				index_1 = mail_message.find(boundary_2, index_3);
				tmp.assign(mail_message, index_3 + 1, index_1 - index_3 - 1);
				tmp = pass_r_n(tmp);
				decode = base64_decode(tmp);
				mail_result.text_body = decode;
				if (code_type == "utf-8" || code_type == "UTF-8")
				{
					mail_result.text_body = utf_to_gb(decode.c_str());
				}
			}
			mail_message.erase(0, index_1);
		}
	//获取附件内容,直接寻找第一个boundary开始的地方
		index_1 = mail_message.find(boundary_1);
		index_2 = mail_message.find(boundary_1, index_1 + len_1);
		while (index_2 != std::string::npos)
		{
			int a, b;
			a = mail_message.find("filename=");
			b = mail_message.find('\"', a);
			index_3 = mail_message.find('\"', b + 1);
			filename.assign(mail_message,b + 1, index_3 - b- 1);
			filename = get_subject(filename);
			mail_result.file_list.push_back(filename);
			decode.assign(mail_message, index_3 + 1, index_2 - index_3 - 1);
			decode = pass_r_n(decode);
			file_result = base64_decode(decode);
			filename = my_directory + filename;
			fstream fout(filename, ios::out | ios::binary);  //二进制的方式写进去
			fout.write(file_result.c_str(), file_result.length());
			mail_message.erase(0, index_2);
			index_1 = mail_message.find(boundary_1);
			index_2 = mail_message.find(boundary_1, index_1 + len_1);
		}
	}
	//只有一个分界线
	else if(flag == 0)
	{
		index_2 = mail_message.find("charset=", index_1 + len_1);
		index_3 = mail_message.find('\"', index_2);
		index_2 = mail_message.find('\"', index_3 + 1);
		code_type.assign(mail_message, index_3 + 1, index_2 - index_3 - 1);

		index_4 = mail_message.find("Content-Transfer-Encoding:", index_1 + 1);
		int index_5 = mail_message.find(boundary_1, index_1 + len_1);
		if (index_4 == std::string::npos || (index_4 > index_5))  //未经过加密，直接取出来。
		{
			index_2 = mail_message.find('\n', index_3);
			index_3 = mail_message.find(boundary_1, index_2);
			mail_result.text_body.assign(mail_message, index_2 + 1, index_3 - index_2 - 1);
			mail_message.erase(0, index_3);
		}
		else
		{
			//获取邮件原文加密方式
			index_2 = mail_message.find(' ', index_4);
			index_3 = mail_message.find('\n', index_2);
			string encrypt_mode;
			encrypt_mode.assign(mail_message, index_2 + 1, index_3 - index_2 - 1);
			if (encrypt_mode.find("base64") == std::string::npos)
			{
				cout << "mail analysis failed because of no boundary!" << endl;
				return;
			}
			else
			{
				index_1 = mail_message.find(boundary_1, index_3);
				tmp.assign(mail_message, index_3 + 1, index_1 - index_3 - 1);
				tmp = pass_r_n(tmp);
				decode = base64_decode(tmp);
				mail_result.text_body = decode;
				if (code_type == "utf-8" || code_type == "UTF-8")
				{
					mail_result.text_body = utf_to_gb(decode.c_str());
				}
			}
			mail_message.erase(0, index_1);
		}
	}
}
//标题格式
//Subject: =?utf-8?b?TU9CQeiAgeWPuOacuumAn+aIkOaJi+WGjO+8muOAiuS5seaWl+ilvw==?=
//标题格式
//Subject: =?utf-8?b?TU9CQeiAgeWPuOacuumAn+aIkOaJi+WGjO+8muOAiuS5seaWl+ilvw==?=
// 临时更改，为了解码文件名字用的，返回值可以不用
string Pop_receiver::get_subject(string& str)
{
	int index_1, index_2, index_3;
	string code_type, pre_code, decode_code;
	index_1 = str.find("=?");
	if (index_1 == std::string::npos)
	{ //没有经过编码的标题，英文标题，直接取出即可
		index_1 = str.find(':');
		index_2 = str.find('\n', index_1 + 1);
		decode_code.assign(str, index_1 + 1, index_2 - index_1 - 1);
		return decode_code;
	}
	//找到编码方式
	index_3 = str.find('?', index_1 + 2);
	code_type.assign(str, index_1 + 2, index_3 - index_1 - 2);
	index_1 = str.find('?', index_3 + 1);
	index_2 = str.find("?=", index_1 + 1);
	pre_code.assign(str, index_1 + 1, index_2 - index_1 - 1);
	decode_code = base64_decode(pre_code);
	//如果为utf-8编码
	if (code_type == "utf-8" || code_type == "UTF-8")
	{
		//cout << code_type << endl;
		//再将utf转为gbk
		decode_code = utf_to_gb(decode_code.c_str());
	}
	return decode_code;
}

//三种可能的格式
//From: "812701654@qq.com" <812701654@qq.com>
//"=?UTF-8?B?546L5a6255+l77yISmlhemhpIFdhbmcp?="<wonjiazhi@163.com>
//"=?GBK?B?zfW98rq9?="<wangjinhang@zju.edu.cn>
//=?UTF-8?B?572R5piT6YKu5Lu25Lit5b+D?= <mail@service.netease.com>
void Pop_receiver::get_sender_info(string &str)
{
	//先获得发件人的地址
	string decode_code,code_type;
	string pre_code;
	int lr, Rr;
	int index_1, index_2;
	lr = str.find('<');
	Rr = str.find('>');
	mail_result.sender_addr.assign(str, lr + 1, Rr - lr - 1);
	str.erase(lr, Rr);
	//如果没有？=证明没有经过编码，直接获取引号内的内容即可
	index_1 = str.find('=?');
	if (index_1 == std::string::npos)
	{
		index_1 = str.find('\"');
		index_2 = str.find('\"', index_1 + 1);  //获取第二个引号的位置
		mail_result.sender_name.assign(str, index_1 + 1, index_2 - index_1 - 1);
		return;
	}
	//编码的存在
	index_1 = str.find("=?");
	index_2 = str.find('?', index_1 + 2);
	code_type.assign(str, index_1 + 2, index_2-index_1-2);
	
	
	int index_3 = str.find("?=", index_2);
	pre_code.assign(str, index_2 + 3, index_3 - index_2 - 3);
	decode_code = base64_decode(pre_code);   //只解析base64编码的标题，先将base64解码
	 
	//如果为utf-8编码
	//"=?GBK?B?zfW98rq9?="<wangjinhang@zju.edu.cn>
	if (code_type == "utf-8" || code_type == "UTF-8")
	{
	//	cout << code_type << endl;		
		//再将utf转为gbk
		mail_result.sender_name = utf_to_gb(decode_code.c_str());
	}
	else{
		mail_result.sender_name = decode_code;
	}
}

void Pop_receiver::get_recv_Cc_list(string& str ,vector<string>&v)
{
	int lr, Rr; // < >
	int index_1, index_2;
	string tmp;

	lr = str.find('<');
	if (lr == std::string::npos)
	{//没有<>中间的收件人，直接赋值即可
		index_1 = str.find(':');
		index_2 = str.find('\n', index_1 + 1);		
		tmp.assign(str, index_1 + 1, index_2 - index_1 - 1);
		v.push_back(tmp);
		return;
	}
	Rr = str.find('>');
	while (Rr != std::string::npos)
	{
		tmp.assign(str, lr+1, Rr - lr - 1);
		v.push_back(tmp);
		str.erase(0, Rr + 1);
		lr = str.find('<');
		Rr = str.find('>', lr + 1);
	}
}
string Pop_receiver::utf_to_gb(const char *utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
//	cout << str << endl;
	return string(str);
}
string Pop_receiver::pass_r_n(string &str)
{
	int i, count=0;
	string temp="";
	for (i = 0; i < str.length(); i++)
	{
		if (str[i] == '\r' || str[i] == '\n')
			continue;
		temp += str[i];
	}
	return temp;
}
//给外部调用接收邮件on个的函数，参数是邮件标记，外部传进来的类
void Pop_receiver::fetch_mail(int mail_id, message_info &tmp)
{
	if (mail_id > mail_num)   //邮件id大于当前邮件，接收肯定失败
	{
		return;
	}
//	get_mailinfo();
	while(!get_mailmessage(mail_id))
	{
		get_mailmessage(mail_id);
	}
	Sleep(1000);
	mail_analysis();
	get_body();
	tmp = mail_result;
	//
}