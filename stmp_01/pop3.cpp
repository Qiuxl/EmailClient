
#include"stdafx.h"
#include"Pop3.h"
#include<Windows.h>
#include<sstream>
//�����׽���
Pop_receiver::Pop_receiver(string user, string pwd, string directory, string Sname, string Sip)
{
	my_directory = directory;
	user_name = user;
	server_name = Sname;
	user_pwd = pwd;
	server_ip = Sname;
}
int Pop_receiver::get_mail_sum()  //�����ʼ�����
{
	get_mailinfo();  //�����ȵ��øú������ܻ�ȡ����Ŀǰ���ʼ���Ŀ������Ĵ�С
	return mail_num;
}
//����׽���
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
	wVersionRequested = MAKEWORD(2, 2);//��2��2�ճ�һ��16λ��word�α�����vc6.0��Ϊwordռ�������ֽ�
	err = WSAStartup(wVersionRequested, &wsaData);  //����socket��
	if (err != 0)
	{
		return false;
	}
	//WSACleanup();  //s�ͷŵķ������������
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
	//���ü�����ַ���ںͶ˿�
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(110);   // ���ʼ�һ�㶼��25�˿ڣ�SSL����465�˿�  
	if (server_name == "") //��ip��ַ
	{
		server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());    // ֱ��ʹ��IP��ַ  
	}
	else
	{
		struct hostent *hp = gethostbyname(server_name.c_str());    // ʹ������  
		server_addr.sin_addr.s_addr = *(int*)(*hp->h_addr_list);
		char* ip = inet_ntoa(server_addr.sin_addr);
		server_ip = string(ip);
	}
	//������������˽������ӣ�
	int ret = connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));  // ��������  
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
//���ڵ�½�����ĳ�ʼ����
// USER �����ַ
// PASS ����
bool Pop_receiver::login()
{
	int rt;
	if (user_name == "" || user_pwd == "")
	{
		cout << "�û����������벻��Ϊ��!" << endl;
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
		cout << "�Ƿ��������֣�" << endl;
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
		cout << "���벻��ȷ" << endl;
		return false;
	}
	return true;
}
//STATNone����������������ع��������ͳ�����ϣ����ʼ����������ֽ���
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
	int index = temp.find(' ');  // ��ʽ����  +ok 156 238237
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
	//һ�����ڻ�ȡ��n���ʼ�����Ϣ
	Sleep(300);
	int rt;
	int message_size = 0; //�ʼ���Ŀ
	stringstream ss;

	ss << mail_id;
	string num_string, send_instruct, temp;//temp���ڹ��췵����Ϣ
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
	char *tmp_receive = new char[message_size + 20]; //���ʼ���Ϣ֮ǰ�� +ok ���ʼ���С����Ϣ
	memset(tmp_receive, 0, message_size + 1);
	//���ͻ�ȡ�ʼ���Ϣ������
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
	//��Ҫ���ν��գ���һ�λ���յ� +OK 2885 octets +ok��ʾ�ɹ�
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
			Sleep(1000);   //ͣ��
		}
		recv(sock, tmp_receive, message_size + 19, 0);
	}

	tmp_receive[message_size + 19] = '\0';
	message_size = 0;
	//cout << mail_message << endl;
	delete []tmp_receive; //�ͷ��ڴ�
	memset(send_buffer, 0, sizeof(send_buffer));
	memset(receive_buffer, 0, sizeof(receive_buffer));
	return true;
}
//base 64���뺯��
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
// �����ʼ�
void Pop_receiver::mail_analysis()
{
	string temp = "", boundary, file_buffer;
	int index_1, index_2, index_3, index_4;
	// �����ʼ�ͷ
	string tmp1;
	boundary_1 = "";
	boundary_2 = "";

	//date ��ʽ Date: Mon, 13 Jun 2016 11:39:03 +0800
	index_1 = mail_message.find("\nDate:");
	index_2 = mail_message.find('\n', index_1 + 1);
	temp.assign(mail_message, index_1 + 1, index_2 - index_1 - 2);
	index_3 = temp.find('+');
	temp.erase(index_3 - 1, temp.length());
 	mail_result.date.assign(temp, 6, temp.length() - 6);
	//	mail_message.erase(0, index_2);
	//cout << mail_message << endl;

	//�ҵ�from����
	index_1 = mail_message.find("\nFrom:");
//	cout << index_1 << endl;
	index_2 = mail_message.find('\n', index_1 + 1);
	temp.assign(mail_message, index_1 + 1, index_2 - index_1 - 2);
//	cout << temp << endl;
	get_sender_info(temp);
	//	mail_message.erase(0, index_2);

	//To ����ʽ�� ��ȡTo��list�����Ÿ���
	//To: qzh951120 <qzh951120@163.com>, 
	//	xlq1120 <xlq1120@yahoo.com>,
	//		615723297 <615723297@qq.com>
	//��ֹ�ж���ռ��ˣ�ֻ��Ҫ�ҵ���һ��ð��ǰ�Ķ���copy����
	index_1 = mail_message.find("\nTo:");
	index_2 = mail_message.find(':', index_1 + 4);
	temp.assign(mail_message, index_1, index_2 - index_1);
	mail_result.receiver.clear();
	 get_recv_Cc_list(temp, mail_result.receiver);

	//Cc:����������ʽ��toһ��
	index_1 = mail_message.find("\nCc:");
	if (index_1 != std::string::npos)
	{
		index_2 = mail_message.find(':', index_1 + 4);
		temp.assign(mail_message, index_1 + 1, index_2 - index_1 - 1);
		mail_result.Cc_list.clear();
		get_recv_Cc_list(temp, mail_result.Cc_list);
	}
	//����subjct
	index_1 = mail_message.find("\nSubject:");
	index_2 = mail_message.find('\n', index_1 + 1);
	temp.assign(mail_message, index_1, index_2 - index_1);
	mail_result.subject = get_subject(temp);

	//��÷ֽ��� boundary��һ��ֻ��һ����boundary=
	index_1 = mail_message.find("boundary=");
	if (index_1 == std::string::npos)
	{
		cout << "mail analysis failed because of no boundary!" << endl;
		return;
	}
	index_2 = mail_message.find('\"', index_1 + 1);
	index_3 = mail_message.find('\"', index_2 + 1);
	boundary_1.assign(mail_message, index_2 + 1, index_3 - index_2 - 1);
	//��һ���ֽ���
	mail_message.erase(0, index_3 + 1);  //����ɾ���ļ�ͷ�Ĳ����ˡ�
	boundary_1 = "--" + boundary_1;   //�ʼ���ʽ��boundaryǰ�����  --
}
//��ȡ�ʼ�������Ϣ��
void Pop_receiver::get_body()
{
	int index_1, index_2, index_3, flag = 1, index_4;
	int len_1, len_2;
	mail_result.file_list.clear();
	string filename;
	string code_type, tmp, decode, file_result;
	len_1 = boundary_1.length();
	if (boundary_1 == "--QXL_INVT")   //�Լ�д���ʼ��ķֽ��ߣ����еĸ�ʽ������������
	{
		index_1 = mail_message.find(boundary_1);  //û�и����Ļ�����֮���ԭ�ľ����ʼ�����
		index_2 = mail_message.find(boundary_1, index_1 + len_1);
		index_3 = mail_message.find("charset=\"gb2312\"", index_1 + len_1);
		mail_result.text_body.assign(mail_message, index_3 + 16, index_2 - index_3 - 16);
		//����Ƿ��и���
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
			fstream fout(filename, ios::out | ios::binary);  //�����Ƶķ�ʽд��ȥ
			fout.write(file_result.c_str(), file_result.length());
			mail_message.erase(0, index_2);
			index_1 = mail_message.find(boundary_1);
			index_2 = mail_message.find(boundary_1, index_1 + len_1);
		}
		return;
	}
	//����֤�Ƿ��еڶ����ֽ���,�������ڵ�һ��֮��
	index_1 = mail_message.find(boundary_1);
	index_1 += len_1;
	index_3 = mail_message.find("boundary=", index_1);

	if (index_3 == std::string::npos)
	{
		flag = 0;  //û�еڶ����ֽ���
	}

//�������ֽ��ߵ�����£��ض��и�������
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
		if (index_4 == std::string::npos || (index_4 > index_5))  //δ�������ܣ�ֱ��ȡ������
		{
			index_2 = mail_message.find('\n', index_3);
			index_3 = mail_message.find(boundary_2, index_2);
			mail_result.text_body.assign(mail_message,index_2 + 1, index_3 - index_2 - 1);
			mail_message.erase(0, index_3);
		}
		else
		{
			//��ȡ�ʼ�ԭ�ļ��ܷ�ʽ
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
	//��ȡ��������,ֱ��Ѱ�ҵ�һ��boundary��ʼ�ĵط�
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
			fstream fout(filename, ios::out | ios::binary);  //�����Ƶķ�ʽд��ȥ
			fout.write(file_result.c_str(), file_result.length());
			mail_message.erase(0, index_2);
			index_1 = mail_message.find(boundary_1);
			index_2 = mail_message.find(boundary_1, index_1 + len_1);
		}
	}
	//ֻ��һ���ֽ���
	else if(flag == 0)
	{
		index_2 = mail_message.find("charset=", index_1 + len_1);
		index_3 = mail_message.find('\"', index_2);
		index_2 = mail_message.find('\"', index_3 + 1);
		code_type.assign(mail_message, index_3 + 1, index_2 - index_3 - 1);

		index_4 = mail_message.find("Content-Transfer-Encoding:", index_1 + 1);
		int index_5 = mail_message.find(boundary_1, index_1 + len_1);
		if (index_4 == std::string::npos || (index_4 > index_5))  //δ�������ܣ�ֱ��ȡ������
		{
			index_2 = mail_message.find('\n', index_3);
			index_3 = mail_message.find(boundary_1, index_2);
			mail_result.text_body.assign(mail_message, index_2 + 1, index_3 - index_2 - 1);
			mail_message.erase(0, index_3);
		}
		else
		{
			//��ȡ�ʼ�ԭ�ļ��ܷ�ʽ
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
//�����ʽ
//Subject: =?utf-8?b?TU9CQeiAgeWPuOacuumAn+aIkOaJi+WGjO+8muOAiuS5seaWl+ilvw==?=
//�����ʽ
//Subject: =?utf-8?b?TU9CQeiAgeWPuOacuumAn+aIkOaJi+WGjO+8muOAiuS5seaWl+ilvw==?=
// ��ʱ���ģ�Ϊ�˽����ļ������õģ�����ֵ���Բ���
string Pop_receiver::get_subject(string& str)
{
	int index_1, index_2, index_3;
	string code_type, pre_code, decode_code;
	index_1 = str.find("=?");
	if (index_1 == std::string::npos)
	{ //û�о�������ı��⣬Ӣ�ı��⣬ֱ��ȡ������
		index_1 = str.find(':');
		index_2 = str.find('\n', index_1 + 1);
		decode_code.assign(str, index_1 + 1, index_2 - index_1 - 1);
		return decode_code;
	}
	//�ҵ����뷽ʽ
	index_3 = str.find('?', index_1 + 2);
	code_type.assign(str, index_1 + 2, index_3 - index_1 - 2);
	index_1 = str.find('?', index_3 + 1);
	index_2 = str.find("?=", index_1 + 1);
	pre_code.assign(str, index_1 + 1, index_2 - index_1 - 1);
	decode_code = base64_decode(pre_code);
	//���Ϊutf-8����
	if (code_type == "utf-8" || code_type == "UTF-8")
	{
		//cout << code_type << endl;
		//�ٽ�utfתΪgbk
		decode_code = utf_to_gb(decode_code.c_str());
	}
	return decode_code;
}

//���ֿ��ܵĸ�ʽ
//From: "812701654@qq.com" <812701654@qq.com>
//"=?UTF-8?B?546L5a6255+l77yISmlhemhpIFdhbmcp?="<wonjiazhi@163.com>
//"=?GBK?B?zfW98rq9?="<wangjinhang@zju.edu.cn>
//=?UTF-8?B?572R5piT6YKu5Lu25Lit5b+D?= <mail@service.netease.com>
void Pop_receiver::get_sender_info(string &str)
{
	//�Ȼ�÷����˵ĵ�ַ
	string decode_code,code_type;
	string pre_code;
	int lr, Rr;
	int index_1, index_2;
	lr = str.find('<');
	Rr = str.find('>');
	mail_result.sender_addr.assign(str, lr + 1, Rr - lr - 1);
	str.erase(lr, Rr);
	//���û�У�=֤��û�о������룬ֱ�ӻ�ȡ�����ڵ����ݼ���
	index_1 = str.find('=?');
	if (index_1 == std::string::npos)
	{
		index_1 = str.find('\"');
		index_2 = str.find('\"', index_1 + 1);  //��ȡ�ڶ������ŵ�λ��
		mail_result.sender_name.assign(str, index_1 + 1, index_2 - index_1 - 1);
		return;
	}
	//����Ĵ���
	index_1 = str.find("=?");
	index_2 = str.find('?', index_1 + 2);
	code_type.assign(str, index_1 + 2, index_2-index_1-2);
	
	
	int index_3 = str.find("?=", index_2);
	pre_code.assign(str, index_2 + 3, index_3 - index_2 - 3);
	decode_code = base64_decode(pre_code);   //ֻ����base64����ı��⣬�Ƚ�base64����
	 
	//���Ϊutf-8����
	//"=?GBK?B?zfW98rq9?="<wangjinhang@zju.edu.cn>
	if (code_type == "utf-8" || code_type == "UTF-8")
	{
	//	cout << code_type << endl;		
		//�ٽ�utfתΪgbk
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
	{//û��<>�м���ռ��ˣ�ֱ�Ӹ�ֵ����
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
//���ⲿ���ý����ʼ�on���ĺ������������ʼ���ǣ��ⲿ����������
void Pop_receiver::fetch_mail(int mail_id, message_info &tmp)
{
	if (mail_id > mail_num)   //�ʼ�id���ڵ�ǰ�ʼ������տ϶�ʧ��
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