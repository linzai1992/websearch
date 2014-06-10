/*
 * easou_extractor_stdata_bk.cpp
 *
 *  Created on: 2013-09-27
 *
 *  Author: round
 */

#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include "easou_extractor_stdata_bk.h"
#include "easou_extractor_stdata.h"

#include "easou_html_tree.h"
#include "easou_vhtml_tree.h"
#include "easou_vhtml_basic.h"
#include "easou_html_extractor.h"
#include "easou_html_attr.h"
#include "html_text_utils.h"
#include "./thrift/StructData_types.h"

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

using namespace std;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using boost::shared_ptr;

#define MAX_NUM 300

namespace {
std::map<std::string, int> local_map_ks;
const char * local_str="12 ������ 12 �������� 13 ������ 13 �������� 14 ���� 15 ���� 16 ���� 17 ������ 18 �������� 19 ���� 20 ְҵ 21 ��� 22 ���� 23 ��Ф 24 Ѫ�� 25 ��ҵԺУ 26 ��Ҫ�ɾ� 27 ������Ʒ 28 ���͹�˾ 29 Уѵ 30 ����ʱ�� 31 ѧУ���� 32 ���ܲ��� 33 ����У�� 34 ��ҪԺϵ 35 ֪��У�� 36 ˶ʿ�� 37 ��ʿ�� 38 ��� 39 �����ص�ѧ�� 40 ��Ҫ���� 40 �������� 41 ѧУ��ַ 42 ռ����� 43 ��չĿ�� 44 ��� 45 �������� 46 ѧ�� 47 ��ʦ 48 ���� 49 Ժ�� 50 ѧУ���� 51 ���� 52 ��Ʒʱ�� 53 ��Ʒ��˾ 54 ��Ƭ���� 54 ���� 55 Ƭ�� 55 ����Ƭʱ�� 56 ��ӳʱ�� 57 ���� 58 ���� 59 ���� 60 ��� 61 ��Ƭ�� 62 �������� 63 ���� 64 ���� 65 ���� 65 ԭ���� 66 ����ʱ�� 67 �»� 68 �绰���� 69 �������� 70 ��� 71 �˿� 72 �������� 73 ��������� 74 ��Ͻ���� 75 ����λ�� 76 �������� 77 �������� 78 ���ƴ��� 79 ���� 80 ��վ 81 ���� 82 �л� 83 ���� 83 �������л� 84 GDP 85 �г� 86 ��ί��� 87 ������У 87 ��Ҫ��У 87 �ߵ�ѧ�� 88 ����ʱ�� 89 �ز� 90 ������� 91 ���� 92 ������������ 93 ���� 94 ҳ�� 95 ������ 96 ��Ҫ���� 97 ������ 98 ԭ�� 99 ����";
}
const static char *g_seps[] ={ "��", ":", 0};//�ָ���

struct STDATA_KS
{
	map<string, int> map_ks;
	StructureData *stdata;
	int flag[MAX_NUM];
};

//ȥ���ַ�����β�Ŀո�
static void clean_string(char *str)
{
	if(str == NULL || str[0] == '\0')
		return;
	char *start = str - 1;
	char *end = str;
	char *p = str;
	while(*p)
	{
		switch(*p)
		{
			case ' ':
			case '\r':
			case '\n':
			case '\t':
			case '_':
			case '-':
				{
					if(start + 1==p)
					{
						start = p;
					}
				}
				break;
			default:
				break;
		}
		++p;
	}
	--p;
	++start;
	if(*start == 0)
	{
		*str = 0 ;
		return;
	}
	end = p + 1;
	while(p > start)
	{
		switch(*p)
		{
			case ' ':
			case '\r':
			case '\n':
			case '\t':
			case '_':
			case '-':
				{
					if(end - 1 == p)
						end = p;
				}
				break;
			default:
				break;
		}
		--p;
	}
	memmove(str,start,end-start);
	*(str + (end - start)) = 0;
}



char *find_next_text(html_vnode_t *vnode)
{
	static char content[4096] = {0};
	int len = 0;
	//���ֵܽ��Ϊ��Ϊ�� �ж�
	if(vnode->nextNode != NULL)
	{
		html_vnode_t *next = vnode->nextNode;
		while(next != NULL)
		{
			char text[1024] = {0};
			int len_text = html_node_extract_content(next->hpNode, text, 1023);
			if(len + len_text > 4095)
				return content;
			len += sprintf(content+len, "%s", text);
			next = next->nextNode;
		}
		return content;
	}
	else if(vnode->upperNode != NULL && vnode->upperNode->nextNode != NULL)
	{
		html_vnode_t *vnode_next = vnode->upperNode->nextNode;
		len = html_node_extract_content(vnode_next->hpNode, content, 4095);
		content[len] = '\0';
		return content;
	}
	return NULL;
}

static int start_for_ks_value(html_vnode_t *vnode, void *data)
{
	STDATA_KS *std_map = (STDATA_KS *)data;
	if (vnode->hpNode->html_tag.tag_type == TAG_PURETEXT)
	{
		char *text = vnode->hpNode->html_tag.text;
		if(text == NULL || (text != NULL && strlen(text) > 99))
			return VISIT_NORMAL;
		int len_text = strlen(text);
		char text_name[100] = {0};
		sprintf(text_name, "%s", text);

		//Ŀǰ������һ�ַָ���,��ֻ�����Էָ�����β�����
		char *str = strstr(text_name, g_seps[0]);
		if(str == NULL || (str != NULL && strlen(str) != strlen(g_seps[0])))
			return VISIT_SKIP_CHILD;

		int len_str = strlen(str);
		text_name[len_text - len_str] = '\0';

		string key = string(text_name);
		map<string, int>::iterator it = std_map->map_ks.find(key);
		if(it == std_map->map_ks.end())
			return VISIT_SKIP_CHILD;
		//printf("key=%s ", key.c_str());
		//printf("stdata_key=%d\n", std_map->map_ks[key]);

		char *value = find_next_text(vnode);

		if(value != NULL && strlen(value) > 0)
		{
			clean_string(value);
			StructureKV temp_kv;
			temp_kv.key = std_map->map_ks[key];
			temp_kv.value = string(value, strlen(value));
			if(std_map->flag[temp_kv.key] == 1)
				return VISIT_SKIP_CHILD;
			std_map->stdata->all.push_back(temp_kv);
			std_map->flag[temp_kv.key] = 1;
		}

		return VISIT_SKIP_CHILD;
	}
	return VISIT_NORMAL;
}

//�ٿ�KS��
int html_tree_extract_bk_stdata(const char *url, int url_len, unsigned int pagetype, html_tree_t *html_tree, html_vtree_t *html_vtree, area_tree_t *atree, StructureData *stdata)
{
	stdata->type = 0;
	stdata->version = 0;
	stdata->all.clear();

	stdata->type = pagetype;

	int ret = html_tree_extract_stdata(url, url_len, pagetype, html_tree, stdata);
	if(ret != 0)
		return -1;

	if(strncmp(url, "http://baike.baidu.com/view/", 28) != 0 && strncmp(url, "http://baike.baidu.com/subview/", 31) != 0)
		return 0;
	
	//hash_map<char *,int> map_ks;
	//map<string, int> local_map_ks;
	if ( local_map_ks.empty() ){
		// load
		std::stringstream tmp;
		int value;
		std::string key;
		tmp << local_str;	
		while (tmp.good()){
			tmp >> value >> key;
			local_map_ks[key] = value;
		}  
//		for(map<string, int>::iterator it = local_map_ks.begin(); it != local_map_ks.end() ; ++it) {
//			cout << it->first << "--> " << it->second << endl;
//		}
        }

	
	//��ȡ�����ļ�
//	FILE *fp = fopen("./Key_baike.conf", "r");
//	assert(fp);
//
//	while(2 == fscanf(fp,"%d,%s\n", &key, name))
//	{
//		string value = string(name);
//		map_ks[value] = key;
//	}
	

	STDATA_KS std_map;
	memset(&std_map, 0, sizeof(std_map));
	std_map.stdata = stdata;
	std_map.map_ks = local_map_ks;

	//for(map<string, int>::iterator itb = std_map.map_ks.begin(); itb != std_map.map_ks.end(); itb++)
	//	printf("name:%s key:%d\n", itb->first.c_str(), itb->second);

	//key_value�ṹ��ȡ
	html_vtree_visit(html_vtree, start_for_ks_value, NULL, &std_map);

	//fclose(fp);
	return 0;
}


