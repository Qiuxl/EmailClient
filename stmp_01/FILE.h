#ifndef _FILE_
#define _FILE_
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include"sendmail.h"
#include"Pop3.h"
using namespace std;
extern struct sMailInfo;
extern class message_info;

class File
{
private:
	string address; //存放根目录，我会给你"root/邮箱名"，作为你读写文件的根地址 
	
public:
	void setRoot(string address);// 把根目录成address
	//	!info.txt:
	//内容标？？的有效，要存
		bool check_info(sMailInfo info);// 如果文件info不存在，在根目录下建另外两个文件夹和并用结构内容给他赋初值，返回false
		//否则返回true；
		bool check_password(char* password);// 判断密码是否正确，返回相应结果
		bool get_info(sMailInfo& info); //把引用中？？的部分正确赋值给引用
		bool set_info(sMailInfo info); //把引用中？？的部分写回去
		//!receive:
//	内容标！！的有效，要存
		int get_newest_inbox();// 返回文件中时间戳最大的那个数据，也就是最新的
		int get_num_inbox(); //返回存着几封邮件
		bool write_inbox(message_info mail, int time_sign);// 给你相关信息，然后以time_sign命名建立文件
		bool read_inbox(message_info& mail, int num); //返回第i封邮件的相关信息，i根据时间戳从大到小排列
		bool delete_inbox(int time_sign);// 把时间戳为time_sign的邮件删掉
		bool read_inbox_with_time(message_info& mail, int time_sign);// 返回时间戳邮件的相关信息，i根据时间戳从大到小排列
		//!send:
	//内容标！！的有效，要存
		int get_newest_outbox();// 返回文件中时间戳最大的那个数据，也就是最新的
		int get_num_outbox(); //返回存着几封邮件
		bool write_outbox(message_info mail, int time_sign);// 给你相关信息，然后以time_sign命名建立文件
		bool read_outbox(message_info& mail, int N);// 返回第i封邮件的相关信息，i根据时间戳从大到小排列
		bool delete_outbox(int time_sign); //把时间戳为time_sign的邮件删掉
		bool read_outbox_with_time(message_info& mail, int time_sign);// 返回时间戳邮件的相关信息，i根据时间戳从大到小排列
};
#endif// !_FILE_

