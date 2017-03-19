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
	char *mailbox_receiver;         //�����ߵ������ַ  
	char *mail_title;               //�������  
	char *mail_body;                //�ʼ��ı�����  
	char mail_server_ip_addr[32];   //��������IP  
	char *mail_server_name;         //�����������ƣ�IP�����ƶ�ѡһ������ȡ���ƣ�  
	sMailInfo() { memset(this, 0, sizeof(sMailInfo)); }
};
class CSendMail
{
public:
	CSendMail();
	~CSendMail();

	bool BeginSendMail(sMailInfo &mail_info);
	void AddFilePath(char *file_path);          //��Ӹ����ľ���·���������б���  
	void DeleteFilePath(char *file_path);       //ɾ������·��������еĻ�  
	void DeleteAllPath(void);                   //ɾ��ȫ��������·��
protected:
	void GetFileName(char *file_name, char *file_path); //�Ӹ�����·���л�ȡ�ļ�����  
	void Char2Base64(char *buff_64, char *src_buff, int length);//��char����ת����Base64����  
	bool CReateSocket(SOCKET &sock);            //����socket����  
	bool Logon(SOCKET &sock);                   //��¼���䣬��Ҫ���з��ʼ�ǰ��׼������  
	int GetFileData(char *file_path);           //���ļ�·����ȡ��������  

	bool SendHead(SOCKET &sock);                //�����ʼ�ͷ  
	bool SendTextBody(SOCKET &sock);            //�����ʼ��ı�����  
	bool SendFileBody(SOCKET &sock);            //�����ʼ�����  
	bool SendEnd(SOCKET &sock);                 //�����ʼ���β  

protected:
	vector <char*> file_path_list_;               //��¼����·��  
	char  send_buff_[4096];                     //���ͻ�����  
	char  receive_buff_[1024];
	char *file_buffer_;                         // ָ�򸽼�������  
	sMailInfo mail_info_;

};

#endif // !_SEND_MAIL_H
