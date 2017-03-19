#ifndef _SEND_MAIL_H_
#define _SEND_MAIL_H_
#include"stdafx.h"
#include<winsock.h>
#include<vector>
using namespace std;

struct sMailInfo                    //邮件信息  
{
	char *mailbox_user_name;        //用户登录邮箱的名称  
	char *mailbox_user_pwd;         //用户登录邮箱的密码  
	char *mailbox_sender_show_name; //用户发送时显示的名称  
	char *mailbox_sender;           //发送者的邮箱地址  
	//	char *mailbox_receiver;         //接收者的邮箱地址  
	char *mail_title;               //邮箱标题  
	char *mail_body;                //邮件文本正文  
	char mail_server_ip_addr[32];   //服务器的IP  
	char *mail_server_name;         //服务器的名称（IP与名称二选一，优先取名称）  
	vector<string> Cc_list;   //支持抄送功能
	vector<string> Cc_name;  //支持密送功能
	vector<string> receiver_list;    //支持群发
	vector<string> reveiver_name;   //存放收件人名字的容器
	void get_receiverlist();       //输入收件人的信息，多个收件人以分号间隔
	void get_Cc_list();					//获取抄送人的信息
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
	void AddFilePath(char *file_path);          //添加附件的决定路径到附件列表中  
	void DeleteFilePath(char *file_path);       //删除附件路径，如果有的话  
	void DeleteAllPath(void);                   //删除全部附件的路径
	void GetFileName(char *file_name, char *file_path); //从附件的路径中获取文件名称  
	void CSendMail::Encode(char *buff_out, const char* buff_in, int DataByte);//把char类型转换成Base64类型  
	bool CReateSocket();            //建立socket连接  
	bool Logon();                   //登录邮箱，主要进行发邮件前的准备工作  
	int GetFileData(char *file_path);           //由文件路径获取附件内容  

	bool SendHead();                //发送邮件头  
	bool SendTextBody();            //发送邮件文本正文  
	bool SendFileBody();            //发送邮件附件  
	bool SendEnd();                 //发送邮件结尾  

protected:
	SOCKET sock;
	vector <char*> file_path_list_;               //记录附件路径  
	char  send_buff_[8192];                     //发送缓冲区  
	char  receive_buff_[1024];
	char *file_buffer_;                         // 指向附件的内容 
};

#endif // !_SEND_MAIL_H
