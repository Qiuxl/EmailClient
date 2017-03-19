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
	vector<string>receiver, Cc_list, file_list;//����ռ��˵������ַ�Լ������˵ĵ�ַ��û����Ϊ��,file_list ��Ÿ�������
	string date;
	string subject;
	string text_body; //�������
};
class Pop_receiver
{
public:
	//�������캯��
	Pop_receiver(string user, string pwd, string directory, string Sname, string Sip = "");
//	~Pop_receiver();ϵͳ�Զ�����������ʡ���Լ�д��
	string base64_decode(const string &encoded_string);
	inline bool isbase64_code(char c){
		return (isalnum(c) || c == '+' || c== '/');  //�ж��Ƿ�Ϊ��ĸ�������ֵĺ��� isalnum
	}
	void copy(message_info& tmp);
	void fetch_mail(int mail_id, message_info &tmp);
	string utf_to_gb(const char* utf8);
	bool create_socket();
	bool login();
	bool get_mailinfo();
	bool get_mailmessage(int mail_id);
	void get_sender_info(string& str); //��ȡ�ռ��˵����ֺ͵�ַ���ռ����п����б��뷽ʽ�Ĵ��ڡ�
	void get_recv_Cc_list(string& str, vector<string>&v);  //��ȡ�ռ����Լ������˵ĵ�ַ�����ﲻ�����ռ��˵�����
	void mail_analysis();   //�����ʼ���Ϣ������������Ǹ�����
	string get_subject(string& str);
	void get_body();   //��ȡ������Ϣ�Լ�������Ϣ
	string pass_r_n(string &str);  //���Ļ����ļ��м���з����ʱ��ȥ����ǰ���\r\n
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
	string my_directory;     //���Լ��������ļ�Ŀ¼·
	string user_name;  //�����ַ
	string user_pwd;   //����
	string server_name;
	string server_ip;
	string mail_message;  //���ڴ�Ž��ܵ����ʼ���Ϣ����ʱδ��������
	vector<string> attach_list;  //��Ÿ������ֵ�����
	string boundary_1, boundary_2;  //�����������ֽ��ߣ�������ڶ���Ϊ�����ķֽ��ߣ��Լ�д���ʼ���û�������ֽ��ߡ�
	int mail_size;
	int mail_num;     
	char receive_buffer[1024];
	char send_buffer[8192];
};

#endif // !_POP_3
