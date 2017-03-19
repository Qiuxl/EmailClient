#ifndef _SEND_MAIL_H_
#define _SEND_MAIL_H_
#include"stdafx.h"
#include<winsock.h>
#include<vector>
using namespace std;

struct sMailInfo                    //�ʼ���Ϣ  
{
	char *mailbox_user_name;        //�û���¼���������  
	char *mailbox_user_pwd;         //�û���¼���������  
	char *mailbox_sender_show_name; //�û�����ʱ��ʾ������  
	char *mailbox_sender;           //�����ߵ������ַ  
	//	char *mailbox_receiver;         //�����ߵ������ַ  
	char *mail_title;               //�������  
	char *mail_body;                //�ʼ��ı�����  
	char mail_server_ip_addr[32];   //��������IP  
	char *mail_server_name;         //�����������ƣ�IP�����ƶ�ѡһ������ȡ���ƣ�  
	vector<string> Cc_list;   //֧�ֳ��͹���
	vector<string> Cc_name;  //֧�����͹���
	vector<string> receiver_list;    //֧��Ⱥ��
	vector<string> reveiver_name;   //����ռ������ֵ�����
	void get_receiverlist();       //�����ռ��˵���Ϣ������ռ����Էֺż��
	void get_Cc_list();					//��ȡ�����˵���Ϣ
	string receiver;
	string Cc_addr;
	sMailInfo() { memset(this, 0, sizeof(sMailInfo)); }
};

class CSendMail
{
public:
	CSendMail();
	~CSendMail();
	sMailInfo mail_info_;
	void set(sMailInfo tmp);
	bool BeginSendMail();
	void AddFilePath(char *file_path);          //��Ӹ����ľ���·���������б���  
	void DeleteFilePath(char *file_path);       //ɾ������·��������еĻ�  
	void DeleteAllPath(void);                   //ɾ��ȫ��������·��
	void GetFileName(char *file_name, char *file_path); //�Ӹ�����·���л�ȡ�ļ�����  
	void CSendMail::Encode(char *buff_out, const char* buff_in, int DataByte);//��char����ת����Base64����  
	bool CReateSocket();            //����socket����  
	bool Logon();                   //��¼���䣬��Ҫ���з��ʼ�ǰ��׼������  
	int GetFileData(char *file_path);           //���ļ�·����ȡ��������  

	bool SendHead();                //�����ʼ�ͷ  
	bool SendTextBody();            //�����ʼ��ı�����  
	bool SendFileBody();            //�����ʼ�����  
	bool SendEnd();                 //�����ʼ���β  

protected:
	SOCKET sock;
	vector <char*> file_path_list_;               //��¼����·��  
	char  send_buff_[8192];                     //���ͻ�����  
	char  receive_buff_[1024];
	char *file_buffer_;                         // ָ�򸽼������� 
};

#endif // !_SEND_MAIL_H
