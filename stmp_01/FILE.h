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
	string address; //��Ÿ�Ŀ¼���һ����"root/������"����Ϊ���д�ļ��ĸ���ַ 
	
public:
	void setRoot(string address);// �Ѹ�Ŀ¼��address
	//	!info.txt:
	//���ݱꣿ������Ч��Ҫ��
		bool check_info(sMailInfo info);// ����ļ�info�����ڣ��ڸ�Ŀ¼�½����������ļ��кͲ��ýṹ���ݸ�������ֵ������false
		//���򷵻�true��
		bool check_password(char* password);// �ж������Ƿ���ȷ��������Ӧ���
		bool get_info(sMailInfo& info); //�������У����Ĳ�����ȷ��ֵ������
		bool set_info(sMailInfo info); //�������У����Ĳ���д��ȥ
		//!receive:
//	���ݱ꣡������Ч��Ҫ��
		int get_newest_inbox();// �����ļ���ʱ��������Ǹ����ݣ�Ҳ�������µ�
		int get_num_inbox(); //���ش��ż����ʼ�
		bool write_inbox(message_info mail, int time_sign);// ���������Ϣ��Ȼ����time_sign���������ļ�
		bool read_inbox(message_info& mail, int num); //���ص�i���ʼ��������Ϣ��i����ʱ����Ӵ�С����
		bool delete_inbox(int time_sign);// ��ʱ���Ϊtime_sign���ʼ�ɾ��
		bool read_inbox_with_time(message_info& mail, int time_sign);// ����ʱ����ʼ��������Ϣ��i����ʱ����Ӵ�С����
		//!send:
	//���ݱ꣡������Ч��Ҫ��
		int get_newest_outbox();// �����ļ���ʱ��������Ǹ����ݣ�Ҳ�������µ�
		int get_num_outbox(); //���ش��ż����ʼ�
		bool write_outbox(message_info mail, int time_sign);// ���������Ϣ��Ȼ����time_sign���������ļ�
		bool read_outbox(message_info& mail, int N);// ���ص�i���ʼ��������Ϣ��i����ʱ����Ӵ�С����
		bool delete_outbox(int time_sign); //��ʱ���Ϊtime_sign���ʼ�ɾ��
		bool read_outbox_with_time(message_info& mail, int time_sign);// ����ʱ����ʼ��������Ϣ��i����ʱ����Ӵ�С����
};
#endif// !_FILE_

