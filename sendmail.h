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
	char *mailbox_receiver;         //接收者的邮箱地址  
	char *mail_title;               //邮箱标题  
	char *mail_body;                //邮件文本正文  
	char mail_server_ip_addr[32];   //服务器的IP  
	char *mail_server_name;         //服务器的名称（IP与名称二选一，优先取名称）  
	sMailInfo() { memset(this, 0, sizeof(sMailInfo)); }
};
class CSendMail
{
public:
	CSendMail();
	~CSendMail();

	bool BeginSendMail(sMailInfo &mail_info);
	void AddFilePath(char *file_path);          //添加附件的决定路径到附件列表中  
	void DeleteFilePath(char *file_path);       //删除附件路径，如果有的话  
	void DeleteAllPath(void);                   //删除全部附件的路径
protected:
	void GetFileName(char *file_name, char *file_path); //从附件的路径中获取文件名称  
	void Char2Base64(char *buff_64, char *src_buff, int length);//把char类型转换成Base64类型  
	bool CReateSocket(SOCKET &sock);            //建立socket连接  
	bool Logon(SOCKET &sock);                   //登录邮箱，主要进行发邮件前的准备工作  
	int GetFileData(char *file_path);           //由文件路径获取附件内容  

	bool SendHead(SOCKET &sock);                //发送邮件头  
	bool SendTextBody(SOCKET &sock);            //发送邮件文本正文  
	bool SendFileBody(SOCKET &sock);            //发送邮件附件  
	bool SendEnd(SOCKET &sock);                 //发送邮件结尾  

protected:
	vector <char*> file_path_list_;               //记录附件路径  
	char  send_buff_[4096];                     //发送缓冲区  
	char  receive_buff_[1024];
	char *file_buffer_;                         // 指向附件的内容  
	sMailInfo mail_info_;

};

#endif // !_SEND_MAIL_H
