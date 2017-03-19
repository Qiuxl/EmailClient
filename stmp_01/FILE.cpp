
#include"FILE.h"
#include<fstream>
#include<stdlib.h>
#include <algorithm>

bool compare(int a, int b)
{
	return a > b;
}
void File::setRoot(string address) 
{ 
	this->address = address;
}
bool File::check_info(sMailInfo info) 
{ 
	fstream fin(address + "info.txt", ios::in);
	if (!fin.is_open())
		return false;
	else
		return true;
}
bool File::check_password(char* password) 
{ 
	fstream fin((address + "info.txt").c_str(), ios::in);
	string user_name, user_pwd;
	if (!fin.is_open())
		return false;
	getline(fin, user_name);
	getline(fin, user_pwd);
	fin.close();
	if (strcmp(password, user_pwd.c_str()))
		return false;
	return true; 
}
bool File::get_info(sMailInfo& info) 
{ 
	fstream fin(address+"info.txt",ios::in);
	string read_tool;
	if (!fin.is_open())
		return false;
	getline(fin, read_tool);
	strcpy(info.mailbox_user_name, read_tool.c_str());
	getline(fin, read_tool);
	strcpy(info.mailbox_user_pwd, read_tool.c_str());
	getline(fin, read_tool);
	strcpy(info.mailbox_sender_show_name, read_tool.c_str());
	getline(fin, read_tool);
	strcpy(info.mailbox_sender, read_tool.c_str());
	fin.close();
	return true;
}
bool File::set_info(sMailInfo info) 
{ 
	fstream  fout(address+"info.txt", ios::out);
	//cout << "开始写 "<< address << "info.txt" << endl;
	if (!fout.is_open())
		return false;
	fout << string(info.mailbox_user_name)<< endl;
	fout << string(info.mailbox_user_pwd)<< endl;
	fout << string(info.mailbox_sender_show_name) << endl;
	fout << string(info.mailbox_sender) << endl;
	fout.close();
	//cout << "写入成功" << endl;
	return true;
}

int File::get_num_inbox()  //返回存着几封邮件
{
	string filename;
	int count = 0;
	string temp;
	int max = 1000;  //最多1000封
	int array[1000] = { 0 };
	filename = address + "receive\\" + "total_email.log";
	fstream fin(filename, ios::in);
	while (getline(fin, temp))
	{
		count++;
	}
	return count;
}
int File::get_newest_inbox()
{
	string filename;
	int max = 1000;  //最多1000封
	int array[1000] = { 0 };
	filename = address + "receive\\" + "total_email.log";
	fstream fin(filename, ios::in);
	int count = 0;
	int index_1, index_2;
	string tmp_1, tmp_2;
	while (getline(fin, tmp_1))
	{
		stringstream sstr;
		index_1 = tmp_1.find('\n');
		tmp_2.assign(tmp_1, 0, index_1);
		sstr << tmp_2;
		sstr >> array[count++];
	}
	if (count == 0)
	{
		return 0;
	}
	//count最终表示有多少封邮件，
	sort(array, array + count, compare);  //从大到小排序
	return array[0];
}

bool File::read_inbox_with_time(message_info& mail, int time_sign)
{
	stringstream str;
	string filename;
	str << time_sign;
	filename = str.str();
	filename += ".email";
	string tmp_1;
	int index_1, index_2;
	filename = address + "receive\\" + filename;
	fstream fin(filename.c_str(), ios::in);
	int index = 1;
	if (!fin.is_open())
	{
		return false;
	}
	while (getline(fin, tmp_1))
	{
		if (index == 1)
		{   
			index_1 = tmp_1.find(':');
			index_2 = tmp_1.find('\n');
			mail.sender_addr.assign(tmp_1, index_1 + 1, index_2 - index_1 - 1);
		}
		if (index == 2)
		{
			index_1 = tmp_1.find(':');
			index_2 = tmp_1.find('\n');
			mail.sender_name.assign(tmp_1, index_1 + 1, index_2 - index_1 - 1);
		}
		if (index == 3)
		{  
			index_1 = tmp_1.find(':');
			index_2 = tmp_1.find('\n');
			mail.date.assign(tmp_1, index_1 + 1, index_2 - index_1 - 1);
		}
		if (index == 4)
		{  
			mail.receiver.clear();
			string total;
			int index_3;
			index_1 = tmp_1.find(':');
			index_1++;
			index_2 = tmp_1.find('\n');
			total.assign(tmp_1, index_1, index_2 - index_1);
			if (tmp_1.find(';') == std::string::npos)
			{
				mail.receiver.push_back(total);
			}
			else
			{
				while ((index_3 = tmp_1.find(';')) != std::string::npos)
				{
					total.assign(tmp_1, index_1, index_3 - index_1);
					mail.receiver.push_back(total);
					index_1 = index_3;
					tmp_1.erase(0, index_3 + 1); 
					index_1 = 0;
				}
				index_2 = tmp_1.find('\n');
				total.assign(tmp_1, 0, index_2);
				mail.receiver.push_back(total);
			}
		}
		if (index == 5)
		{
			string total;
			mail.Cc_list.clear();
			int index_3;
			index_1 = tmp_1.find(':');
			index_1++;
			index_2 = tmp_1.find('\n');
			total.assign(tmp_1, index_1, index_2 - index_1);
			if (total.length() <= 1)
			{

			}	
			else
			{
				if (tmp_1.find(';') == std::string::npos)
				{
					mail.Cc_list.push_back(total);
				}
				else
				{
					while ((index_3 = tmp_1.find(';')) != std::string::npos)
					{
						total.assign(tmp_1, index_1, index_3 - index_1);
						mail.Cc_list.push_back(total);
						index_1 = index_3;
						tmp_1.erase(0, index_3 + 1); 
						index_1 = 0;
					}
					index_2 = tmp_1.find('\n');
					total.assign(tmp_1, 0, index_2);
					mail.Cc_list.push_back(total);
				}
			}
		}
		if (index == 6)
		{ 
			string total;
			int index_3;
			mail.file_list.clear();
			index_1 = tmp_1.find(':');
			index_1++;
			index_2 = tmp_1.find('\n');
			total.assign(tmp_1, index_1, index_2 - index_1);
			if (total.length() <= 1)
			{

			}	//绌猴紝浠�涔堥兘涓嶅仛
			else
			{
				//涓棿浠ワ紱闅斿紑
				if (tmp_1.find(';') == std::string::npos)
				{
					mail.file_list.push_back(total);
				}
				else
				{
					while ((index_3 = tmp_1.find(';')) != std::string::npos)
					{
						total.assign(tmp_1, index_1, index_3 - index_1);
						mail.file_list.push_back(total);
						index_1 = index_3;
						tmp_1.erase(0, index_3 + 1); //鍒犻櫎锛涘墠闈㈢殑浠ュ強鑷繁
						index_1 = 0;
					}
					index_2 = tmp_1.find('\n');
					total.assign(tmp_1, 0, index_2);
					mail.file_list.push_back(total);
				}
			}
		}
		if (index == 7)
		{
			index_1 = tmp_1.find(':');
			index_2 = tmp_1.find('\n');
			//鍐掑彿涓庢崲琛屼箣闂翠负鍙戜欢浜?
			mail.subject.assign(tmp_1, index_1 + 1, index_2 - index_1 - 1);
		}
		if (index == 8)
		{
			break;  //璺冲嚭鐩存帴璇诲嚭閭欢姝ｆ枃
		}
		index++;
	}
	mail.text_body = "";
	while (getline(fin, tmp_1))
	{
		if (tmp_1 == "\r"||tmp_1 =="\r\n")
		{
			mail.text_body += "\n";
		}
		else
		{
			mail.text_body += tmp_1;
		}
		
	}
	return true;
}
bool File::write_inbox(message_info mail, int time_sign)
{
	string file_name;
	stringstream sstr;
	int i;
	string direcoty = address + "receive\\";
	string for_record;
	sstr << time_sign;
	sstr >> file_name;
	for_record = file_name;
	file_name += ".email";
	file_name = direcoty + file_name;    // 812701654\receive\11282.email
	fstream fout(file_name.c_str(), ios::out);
	fout << "sender_addr:" << mail.sender_addr << endl;  //  1
	fout << "sender_name:" << mail.sender_name << endl;  // 2
	fout << "date:" << mail.date << endl;   // 3
	fout << "receiver:";     //4
	for (i = 0; i < (int)mail.receiver.size() - 1; i++)
	{
		fout << mail.receiver[i] << ";";
	}
	fout << mail.receiver[i] << endl;
	fout << " Cc:";    // 5
	for (i = 0; i < (int)mail.Cc_list.size() - 1; i++)
	{
		fout << mail.Cc_list[i] << ";";
	}
	if (mail.Cc_list.size() > 0)
		fout << mail.Cc_list[i];
	fout << endl;
	fout << "attach:";//6
	for (i = 0; i < (int)mail.file_list.size() - 1; i++)
	{
		fout << mail.file_list[i] << ";";
	}
	if (mail.file_list.size() > 0)
		fout << mail.file_list[i];
	fout << endl;
	fout << "subjec:" << mail.subject << endl;  //7
	fout << "main:" << endl;
	fout << mail.text_body << endl;
	string temp = direcoty + "total_email.log";
	fstream out(temp.c_str(), ios::out | ios::app);
	out << for_record << endl;  
	return true;
}
bool File::read_inbox(message_info& mail, int num)
{
	string filename;
	int max = 1000;  
	int array[1000] = { 0 };
	filename = address + "receive\\" + "total_email.log";
	fstream fin(filename.c_str(), ios::in);
	int count = 0;
	int index_1, index_2;
	string tmp_1, tmp_2;
	while (getline(fin, tmp_1))
	{
		stringstream sstr;  
		index_1 = tmp_1.find('\n');
		tmp_2.assign(tmp_1, 0, index_1);
		sstr << tmp_2;
		sstr >> array[count++];
	}
	if (num > count)  
	{
		return false;
	}
	sort(array, array + count, compare);  
	if (!read_inbox_with_time(mail, array[num - 1]))
		return false;
	return true;
}
bool File::delete_inbox(int time_sign) 
{ 
	string path = address + "receive//";
	char fileName[10];
	sprintf(fileName, "%08x",time_sign);

	return true;
}

int File::get_newest_outbox()//用处不大 
{ 
	string path = address + "send\\";
	return 0; 
}
int File::get_num_outbox() 
{ 
	string path = address + "send\\";
	string temp,aimfile=path+"catalog.txt";
	int time;
	fstream fin(aimfile.c_str(),ios::in);
	if(!fin.is_open())
		return 0;
	getline(fin, temp);
	sscanf(temp.c_str(), "%d", &time);
	fin.close();
	return time;
}
bool File::write_outbox(message_info mail, int time_sign)
{
	string path = address + "send\\";
	string temp, aimfile;
	char time[20];
	fstream fin((path + "catalog.txt").c_str(), ios::in);
	vector<int> name;
	sprintf(time, "%08x.email", time_sign);
	temp = time;
	aimfile = path + temp;
	int mailNum, filename;
	if (fin.is_open())
	{
		bool flag = true;
		getline(fin, temp);
		sscanf(temp.c_str(), " %d", &mailNum);
		for (int i = 0; i < mailNum; i++)
		{
			getline(fin, temp);
			sscanf(temp.c_str(), " %x", &filename);
			if (time_sign >= filename &&flag)
			{
				name.push_back(time_sign);
				flag = false;
				if (time_sign == filename)
					continue;
			}
			name.push_back(filename);
		}
		if (flag)
			name.push_back(time_sign);
		fin.close();
		fstream fout((path + "catalog.txt").c_str(), ios::out);
		sprintf(time, "%d", name.size());
		temp = time;
		fout << time << endl;
		for (int i = 0; i < name.size(); i++)
		{
			sprintf(time, "%08x", name[i]);
			temp = time;
			fout << time << endl;
		}
		fout.close();
	}
	else
	{
		fstream fout((path + "catalog.txt").c_str(), ios::out);
		fout << 1 << endl;
		sprintf(time, "%08x", time_sign);
		temp = time;
		fout << time << endl;
		fout.close();
	}
	fstream fout(aimfile.c_str(), ios::out);
	fout << ":" << mail.sender_addr << endl;
	fout << ":" << mail.sender_name << endl;
	fout << ":" << mail.receiver.size() << endl;
	for (int i = 0; i < mail.receiver.size(); i++)
	{
		fout << ":" << mail.receiver[i] << endl;
	}
	fout << ":" << mail.Cc_list.size() << endl;
	for (int i = 0; i < mail.Cc_list.size(); i++)
	{
		fout << ":" << mail.Cc_list[i] << endl;
	}
	fout << ":" << mail.file_list.size() << endl;
	for (int i = 0; i < mail.file_list.size(); i++)
	{
		fout << ":" << mail.file_list[i] << endl;
	}
	fout << ":" << mail.date << endl;
	fout << ":" << mail.subject << endl;
	fout <<  mail.text_body << endl;
	fout.close();
	return true;
}
bool File::read_outbox(message_info& mail, int N)
{
	string path = address + "send\\";
	string temp, aimfile, temp2;
	int mailNum, fileName, index_1, index_2, num;
	char time[20];
	fstream fin((path + "catalog.txt").c_str(), ios::in);
	if (!fin.is_open()) return false;
	getline(fin, temp);
	sscanf(temp.c_str(), " %d", &mailNum);
	if (N > mailNum) return false;
	for (int i = 0; i < N; i++)
		getline(fin, temp);
	sscanf(temp.c_str(), " %x", &fileName);
	aimfile = path + temp + ".email";
	fin.close();
	fin.open(aimfile.c_str(), ios::in);
	if (!fin.is_open())
		return false;
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	mail.sender_addr.assign(temp, index_1 + 1, index_2 - 1);
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	mail.sender_name.assign(temp, index_1 + 1, index_2 - 1);
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	temp2.assign(temp, index_1 + 1, index_2 - 1);
	sscanf(temp2.c_str(), " %d", &num);
	for (int i = 0; i < num; i++)
	{
		getline(fin, temp);
		index_1 = temp.find(':');
		index_2 = temp.find('\n');
		temp2.assign(temp, index_1 + 1, index_2 - 1);
		mail.receiver.push_back(temp2);
	}
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	temp2.assign(temp, index_1 + 1, index_2 - 1);
	sscanf(temp2.c_str(), " %d", &num);
	for (int i = 0; i < num; i++)
	{
		getline(fin, temp);
		index_1 = temp.find(':');
		index_2 = temp.find('\n');
		temp2.assign(temp, index_1 + 1, index_2 - 1);
		mail.Cc_list.push_back(temp2);
	}
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	temp2.assign(temp, index_1 + 1, index_2 - 1);
	sscanf(temp2.c_str(), " %d", &num);
	for (int i = 0; i < num; i++)
	{
		getline(fin, temp);
		index_1 = temp.find(':');
		index_2 = temp.find('\n');
		temp2.assign(temp, index_1 + 1, index_2 - 1);
		mail.file_list.push_back(temp2);
	}
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	mail.date.assign(temp, index_1 + 1, index_2 - 1);
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	mail.subject.assign(temp, index_1 + 1, index_2 - 1);
	mail.text_body = "";
	while (getline(fin, temp))
		mail.text_body = mail.text_body + temp + "\n";
	fin.close();
	return true;
}
bool File::delete_outbox(int time_sign) 
{ 
	string path = address + "send\\";
	return true;
}
bool File::read_outbox_with_time(message_info& mail, int time_sign) 
{ 
	string path = address + "send\\";
	string aimfile,temp,temp2;
	int index_1, index_2,num;
	char time[20];
	sprintf(time, "%08x", time_sign);
	temp = time;
	aimfile = path + temp + ".email";
	fstream fin(aimfile.c_str(),ios::in);
	if (!fin.is_open())
		return false;
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	mail.sender_addr.assign(temp, index_1 + 1, index_2 - 1);
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	mail.sender_name.assign(temp, index_1 + 1, index_2 - 1);
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	temp2.assign(temp, index_1 + 1, index_2 - 1);
	sscanf(temp2.c_str(), " %d", &num);
	for (int i = 0; i < num; i++)
	{
		getline(fin, temp);
		index_1 = temp.find(':');
		index_2 = temp.find('\n');
		temp2.assign(temp, index_1 + 1, index_2 - 1);
		mail.receiver.push_back(temp2);
	}
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	temp2.assign(temp, index_1 + 1, index_2 - 1);
	sscanf(temp2.c_str(), " %d", &num);
	for (int i = 0; i < num; i++)
	{
		getline(fin, temp);
		index_1 = temp.find(':');
		index_2 = temp.find('\n');
		temp2.assign(temp, index_1 + 1, index_2 - 1);
		mail.Cc_list.push_back(temp2);
	}
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	temp2.assign(temp, index_1 + 1, index_2 - 1);
	sscanf(temp2.c_str(), " %d", &num);
	for (int i = 0; i < num; i++)
	{
		getline(fin, temp);
		index_1 = temp.find(':');
		index_2 = temp.find('\n');
		temp2.assign(temp, index_1 + 1, index_2 - 1);
		mail.file_list.push_back(temp2);
	}
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	mail.date.assign(temp, index_1 + 1, index_2 - 1);
	getline(fin, temp);
	index_1 = temp.find(':');
	index_2 = temp.find('\n');
	mail.subject.assign(temp, index_1 + 1, index_2 - 1);
	mail.text_body = "";
	while (getline(fin, temp))
		mail.text_body = mail.text_body + temp + "\n";
	fin.close();
	return true;
}