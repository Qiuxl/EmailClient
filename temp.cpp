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
	// 分配给pBuff64  ↑ 分配给pBuff64+1  
	// point所在的位置  
	static char base_64_encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";//base64所映射的字符表  
	int point;      // 每一个源字符拆分的位置，可取2,4,6；初始为2  
	point = 2;
	int base_index; // base64字符的索引  
	char n = 0;     // 上一个源字符的残留值  
	for (int index = 0; index < length; index++)
	{
		if (point == 2)
		{
			base_index = ((*src_buff) >> point) & 0x3f; // 取得pSrcBuff的高point位  
		}
		else if (point == 4)
		{
			base_index = ((*src_buff) >> point) & 0xf;    // 取得pSrcBuff的高point位  
		}
		else if (point == 6)
		{
			base_index = ((*src_buff) >> point) & 0x3;    // 取得pSrcBuff的高point位  
		}
		base_index += n;                                // 与pSrcBuff-1的低point结合组成Base64的索引  
		*buff_64++ = base_64_encode[base_index];        // 由索引表得到pBuff64  
		n = ((*src_buff) << (6 - point));             // 计算源字符中的残留值  
		n = n & 0x3f;                                   //确保n的最高两位为0  
		point += 2;                                     //源字符的拆分位置上升2  
		if (point == 8)      //如果拆分位置为8说明pSrcBuff有6位残留，可以组成一个完整的Base64字符，所以直接再组合一次  
		{
			base_index = (*src_buff) & 0x3f;            //提取低6位，这就是索引了  
			*buff_64++ = base_64_encode[base_index];
			n = 0;          // 残留值为0  
			point = 2;      // 拆分位置设为2  
		}
		src_buff++;

	}
	if (n != 0)
	{
		*buff_64++ = base_64_encode[n];
	}
	if (length % 3 == 2)    // 如果源字符串长度不是3的倍数要用'='补全  
	{
		*buff_64 = '=';
	}
	else if (length % 3 == 1)
	{
		*buff_64++ = '=';
		*buff_64 = '=';
	}
}
