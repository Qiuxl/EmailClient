#ifndef _POP_3
#define _POP_3
#include<atlenc.h>
#include<winsock.h>
#include<string>
#include<vector>
using namespace std;
class message_info
{
public:
	string sender_addr;
	string sender_name;
	vector<string>receiver, Cc_list, file_list;//存放收件人的邮箱地址以及抄送人的地址，没有则为空,file_list 存放附件名称
	string date;
	string subject;
	string text_body; //存放正文
};
class Pop_receiver
{
public:
	//内联构造函数
	Pop_receiver(string user, string pwd, string directory, string Sname, string Sip = "");
//	~Pop_receiver();系统自动析构，这里省略自己写的
	string base64_decode(const string &encoded_string);
	inline bool isbase64_code(char c){
		return (isalnum(c) || c == '+' || c== '/');  //判断是否为字母或者数字的函数 isalnum
	}
	void copy(message_info& tmp);
	void fetch_mail(int mail_id, message_info &tmp);
	string utf_to_gb(const char* utf8);
	bool create_socket();
	bool login();
	bool get_mailinfo();
	bool get_mailmessage(int mail_id);
	void get_sender_info(string& str); //获取收件人的名字和地址，收件人中可能有编码方式的存在。
	void get_recv_Cc_list(string& str, vector<string>&v);  //获取收件人以及抄送人的地址，这里不解析收件人的名字
	void mail_analysis();   //解析邮件信息，存放在下面那个类中
	string get_subject(string& str);
	void get_body();   //获取正文信息以及附件信息
	string pass_r_n(string &str);  //正文或者文件中间进行翻译的时候去除掉前后的\r\n
	void clear();
	int get_mail_sum();
	//void show()
	//{
	//	cout << "sender name: " << mail_result.sender_name << endl;
	//	cout << "sender address: " << mail_result.sender_addr << endl;
	//	cout << "receiver list: ";
	//	for (int i = 0; i < mail_result.receiver.size();i++)
	//	{
	//		cout << mail_result.receiver[i] << " ";
	//	}
	//	cout << endl;
	//	cout << "date: " << mail_result.date << endl;
	//	cout << "Cc address: ";
	//	for (int i = 0; i < mail_result.Cc_list.size();i++)
	//	{
	//		cout << mail_result.Cc_list[i] << " ";
	//	}
	//	cout << endl;
	//	cout << "Subject: " << mail_result.subject << endl;
	//	cout << "main text: " << mail_result.text_body << endl;;
	//	cout << "attach list: "; 
	//	for (int i = 0; i < mail_result.file_list.size();i++)
	//	{
	//		cout << mail_result.file_list[i] << " ";
	//	}
	//	cout << endl;
	//}
protected:
	message_info mail_result;
private:
	SOCKET sock;
	string my_directory;     //给自己附件的文件目录路
	string user_name;  //邮箱地址
	string user_pwd;   //密码
	string server_name;
	string server_ip;
	string mail_message;  //用于存放接受到的邮件信息，此时未经过解码
	vector<string> attach_list;  //存放附件名字的容器
	string boundary_1, boundary_2;  //可能有两个分界线，若有则第二个为附件的分界线，自己写的邮件则没有两个分界线。
	int mail_size;
	int mail_num;     
	char receive_buffer[1024];
	char send_buffer[8192];
};

#endif // !_POP_3
