#include"stdafx.h"
#include"sendmail.h"
using namespace std;

CSendMail::CSendMail()
{
	file_buffer_ = NULL;
	memset(send_buff_, 0, sizeof(send_buff_));
	memset(receive_buff_, 0, sizeof(receive_buff_));
}

CSendMail::~CSendMail()
{
	DeleteAllPath();
}
void CSendMail::Char2Base64(char* buff_64, char* src_buff, int length)
{
	//1   1   1   1   1   1   1   1  
	// �����pBuff64  �� �����pBuff64+1  
	// point���ڵ�λ��  
	static char base_64_encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";//base64��ӳ����ַ���  
	int point;      // ÿһ��Դ�ַ���ֵ�λ�ã���ȡ2,4,6����ʼΪ2  
	point = 2;
	int base_index; // base64�ַ�������  
	char n = 0;     // ��һ��Դ�ַ��Ĳ���ֵ  
	for (int index = 0; index < length; index++)
	{
		if (point == 2)
		{
			base_index = ((*src_buff) >> point) & 0x3f; // ȡ��pSrcBuff�ĸ�pointλ  
		}
		else if (point == 4)
		{
			base_index = ((*src_buff) >> point) & 0xf;    // ȡ��pSrcBuff�ĸ�pointλ  
		}
		else if (point == 6)
		{
			base_index = ((*src_buff) >> point) & 0x3;    // ȡ��pSrcBuff�ĸ�pointλ  
		}
		base_index += n;                                // ��pSrcBuff-1�ĵ�point������Base64������  
		*buff_64++ = base_64_encode[base_index];        // ��������õ�pBuff64  
		n = ((*src_buff) << (6 - point));             // ����Դ�ַ��еĲ���ֵ  
		n = n & 0x3f;                                   //ȷ��n�������λΪ0  
		point += 2;                                     //Դ�ַ��Ĳ��λ������2  
		if (point == 8)      //������λ��Ϊ8˵��pSrcBuff��6λ�������������һ��������Base64�ַ�������ֱ�������һ��  
		{
			base_index = (*src_buff) & 0x3f;            //��ȡ��6λ�������������  
			*buff_64++ = base_64_encode[base_index];
			n = 0;          // ����ֵΪ0  
			point = 2;      // ���λ����Ϊ2  
		}
		src_buff++;

	}
	if (n != 0)
	{
		*buff_64++ = base_64_encode[n];
	}
	if (length % 3 == 2)    // ���Դ�ַ������Ȳ���3�ı���Ҫ��'='��ȫ  
	{
		*buff_64 = '=';
	}
	else if (length % 3 == 1)
	{
		*buff_64++ = '=';
		*buff_64 = '=';
	}
}
