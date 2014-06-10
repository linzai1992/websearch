/*
 * easou_extractor_stdata.cpp
 *
 *  Created on: 2013-07-19
 *
 *  modify : 2013-08-15
 *  
 *  Author: round
 */

#include <string>
#include "easou_extractor_stdata.h"

#include "easou_html_tree.h"
#include "easou_vhtml_tree.h"
#include "easou_vhtml_basic.h"
#include "easou_html_extractor.h"
#include "easou_html_attr.h"
#include "html_text_utils.h"
#include "./thrift/StructData_types.h"

#include "easou_mark_markinfo.h"
#include "easou_mark_func.h"

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <stack>


using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using boost::shared_ptr;


struct visit_for_bk_t
{
	const char* url;
	int url_len;
	StructureData* stdata;
	int dir_num;

	char* buf;
	int avail;
	int size;
	int count;
	int count1;//����ͼƬ������
	
	//������
	char *title[10];
	char *desp[10];
	int number_title;
	int number_desp;
};

struct visit_for_jy_t
{
	StructureData *stdata;

	const char *url;
	int url_len;

	char *buf_img;
	char *buf_jy;
	char *buf_jq;
	int avail_img;
	int avail_jy;
	int avail_jq;
	int size;
	int count;
};

struct visit_for_dp_t
{
	StructureData *stdata;
	char *buf;
	int avail;
	int size;
};

//ȥ���ַ�����β�Ŀո�
void clean_string(char *str)
{
	if(str == NULL)
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
	//�����������ַ�����β�� ������ǰ
	--p;
	++start;
	if(*start == 0)
	{
		//�Ѿ����ַ�����ĩβ��
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

//ȥ���ַ����ڵĿհ��ַ�
void remove_str_blank(const char *src, char *dst)
{
	char c;

	while (c = *src++)
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
			*dst++ = c;
	*dst = 0;
}

static html_node_t *get_sub_a_node(html_node_t *node_t)
{
	std::stack<html_node_t *> stack_node; //����ջ

	if (node_t == NULL || node_t->child == NULL)
		return NULL;
	if (node_t->html_tag.tag_type == TAG_A)
		return node_t;

	html_node_t *node = node_t->child;

	while (node != NULL || !stack_node.empty())
	{
		while (node != NULL)
		{
			if (node->html_tag.tag_type == TAG_A)
			{
				return node;
			}
			else
			{
				stack_node.push(node);
				node = node->child;
			}
		}
		if (!stack_node.empty())
		{
			node = stack_node.top();
			stack_node.pop();
			node = node->next;
		}
	}
	return NULL;
}


int start_visit_bk(html_tag_t* html_tag, void* result, int flag)
{
	visit_for_bk_t* data = (visit_for_bk_t*) result;

	//����
	if (html_tag->tag_type == TAG_H1)
	{
		char *attr = get_attribute_value(html_tag, ATTR_CLASS);
		if(attr != NULL && strcmp(attr, "title") == 0)
		{

			char content[1024] = {0};
			int len = html_node_extract_content(html_tag->html_node, content, 1023);
			StructureKV temp_kv;
			temp_kv.key = 0;
			temp_kv.value = string(content, len);
			data->stdata->all.push_back(temp_kv);

		}
	} 
	else if (html_tag->tag_type == TAG_DIV)
	{
		//����
		char *attr = get_attribute_value(html_tag, ATTR_CLASS);
		if(attr != NULL && strcmp(attr, "card-summary-content") == 0 && data->count == 1)
		{

			char content[10240] = {0};
			int len = html_node_extract_content(html_tag->html_node, content, 10239);
			if(len > 0)
			{
				StructureKV temp_kv;
				temp_kv.key = 2;
				temp_kv.value = string(content, len);
				data->stdata->all.push_back(temp_kv);
				data->count = 0;
			}
		}
		char *attr_id = get_attribute_value(html_tag, ATTR_ID);
		if(attr_id != NULL && strstr(attr_id, "card-show-tab") != NULL)
		{
			char content[10240] = {0};
			int len = html_node_extract_content(html_tag->html_node->child, content, 10239);
			content[len] ='\0';

			char *attr_a = get_attribute_value(&html_tag->html_node->child->next->html_tag, ATTR_HREF);

			if(len + data->url_len + strlen(attr_a) > 40959)
				return VISIT_ERROR;
			sprintf(data->desp[data->number_desp], "%s %s%s", content, data->url, attr_a);

			data->number_desp++;
		}
	}
	else if (html_tag->tag_type == TAG_DD)
	{
		//ǰ5��Ŀ¼������
		char *attr = get_attribute_value(html_tag, ATTR_CLASS);
		//baike�İ� ����http://baike.baidu.com/view/ http://baike.baidu.com/link?url=
		//modify ��ȡĿ¼���� 2013-09-27
		if(data->dir_num < 6 && attr != NULL && strcmp(attr, "catalog-item") == 0 && html_tag->html_node->child != NULL)
		{
			//��DD�ڵ��µ�A��ǩ
			html_node_t *node = get_sub_a_node(html_tag->html_node);
			if(node == NULL)
				return VISIT_SKIP_CHILD;
			char *attr_a = get_attribute_value(&node->html_tag, ATTR_HREF);
			if (attr_a)
			{
				int attr_len = strlen(attr_a);

				char content[1024] = {0};
				int len = html_node_extract_content(node, content, 1023);
				content[len] = '\0';

				if (data->avail + len + data->url_len + attr_len + 1 >= data->size || attr_len > 2)
					return VISIT_ERROR;

				char content_black[1024] = {0};
				remove_str_blank(content, content_black); ////ƴ���ַ���ʱ Ӧ��ȥ���ո�
				data->avail += sprintf(data->buf + data->avail, "%s %s%s ", content_black, data->url, attr_a);
				data->dir_num++;
			}
		}
		char *attr_tag = get_attribute_value(html_tag, "classtag");
		if(attr_tag != NULL && strstr(attr_tag, "card-show-tab") != NULL)
		{
			char content[20] = {0};
			int len = html_node_extract_content(html_tag->html_node, content, 19);
			content[len]='\0';

			strncpy(data->title[data->number_title], content, len);
			//printf("data->title=%s\n", content);	
			data->number_title++;
		}

	}
	if(html_tag->tag_type == TAG_IMG)
	{
		char *attr = get_attribute_value(html_tag, ATTR_CLASS);
		char *attr_src = get_attribute_value(html_tag, ATTR_SRC);
		if(attr != NULL && strstr(attr, "card-image") != NULL && attr_src != NULL && data->count1 == 1)
		{
			int len = strlen(attr_src);
			if(len > 0)
			{
				StructureKV temp_kv;
				temp_kv.key = 11;
				temp_kv.value = string(attr_src, len);
				data->stdata->all.push_back(temp_kv);		
				data->count1 = 0;
			}
		}
	}

	return 0;
}

int start_visit_jy(html_tag_t* html_tag, void* result, int flag)
{
	visit_for_jy_t *data = (visit_for_jy_t *)result;

	//����
	if (html_tag->tag_type == TAG_SPAN)
	{
		char *attr = get_attribute_value(html_tag, ATTR_CLASS);
		if(attr != NULL && strcmp(attr, "exp-title") == 0)
		{

			char content[1024] = {0};
			int len = html_node_extract_content(html_tag->html_node, content, 1024);

			StructureKV temp_kv;
			temp_kv.key = 0;
			temp_kv.value = string(content, len);
			data->stdata->all.push_back(temp_kv);

		}
	}

	//�׶�����
	else if (html_tag->tag_type == TAG_DIV)
	{
		char *attr = get_attribute_value(html_tag, ATTR_CLASS);
		if(attr != NULL && strcmp(attr, "content-listblock-text") == 0 && data->count == 1)
		{

			char content[40960] = {0};
			int len = html_node_extract_content(html_tag->html_node, content, 40960);
			if(len > 0)
			{
				StructureKV temp_kv;
				temp_kv.key = 2;
				temp_kv.value = string(content, len);
				data->stdata->all.push_back(temp_kv);
				data->count = 0;
			}

		}
	}

	//ͼƬҳ�������Լ�ͼƬ��������
	else if (html_tag->tag_type == TAG_LI)
	{
		char *attr = get_attribute_value(html_tag, ATTR_CLASS);
		if(attr != NULL && strcmp(attr, "thumb-item") == 0 && html_tag->html_node->child != NULL && html_tag->html_node->child->html_tag.tag_type == TAG_A)
		{
			//html_node_t *child = html_tag->html_node->child;
			char *attr_a = get_attribute_value(&html_tag->html_node->child->html_tag, ATTR_HREF);
			char *attr_img = get_attribute_value(&html_tag->html_node->child->child->html_tag, ATTR_SRC);
			int len1 = strlen(attr_a);
			int len2 = strlen(attr_img);

			if (data->avail_img + data->url_len + len1 + len2 + 1 >= data->size)
				return VISIT_ERROR;
			data->avail_img += sprintf(data->buf_img+data->avail_img, "%s%s ", data->url, attr_a);
			data->avail_img += sprintf(data->buf_img+data->avail_img, "%s ", attr_img);


		}

	}

	//��ؾ������ּ�����pos:relate_title
	else if (html_tag->tag_type == TAG_A)
	{
		char *attr_log = get_attribute_value(html_tag, "log");
		if(attr_log != NULL && strstr(attr_log, "pos:relate_title") != NULL)
		{
			char content[1024] = {0};
			int len = html_node_extract_content(html_tag->html_node, content, 1024);
			content[len] = '\0';

			char content_black[1024] = {0};
			remove_str_blank(content, content_black);
			len = strlen(content_black);
			char *attr_a = get_attribute_value(html_tag, ATTR_HREF);
			int len1 = strlen(attr_a);

			if (data->avail_jy + data->url_len + len + len1 + 1 >= data->size)
				return VISIT_ERROR;

			data->avail_jy += sprintf(data->buf_jy+data->avail_jy, "%s %s%s ", content_black, data->url, attr_a);

		}
		//��ط���/���ɴ�ȫ���ּ�����article-thumbnail-img
		char *attr = get_attribute_value(html_tag, ATTR_CLASS);
		if(attr != NULL && strcmp(attr, "article-thumbnail-img") == 0)
		{
			char *attr_tit = get_attribute_value(html_tag, ATTR_TITLE);
			char attr_tit_black[1024] = {0};
			remove_str_blank(attr_tit, attr_tit_black);
			char *attr_href = get_attribute_value(html_tag, ATTR_HREF);
			int len1 = strlen(attr_tit_black);
			int len2 = strlen(attr_href);
			if (data->avail_jq + data->url_len + len1 + len2 + 1 >= data->size)
				return VISIT_ERROR;

			data->avail_jq += sprintf(data->buf_jq+data->avail_jq, "%s %s%s ", attr_tit_black, data->url, attr_href);
		}



	}

	return 0;
}

int start_visit_dp(html_tag_t* html_tag, void* result, int flag)
{
	visit_for_dp_t *data = (visit_for_dp_t *)result;

	//����
	if (html_tag->tag_type == TAG_H1)
	{
		char *attr = get_attribute_value(html_tag, ATTR_CLASS);
		if(attr != NULL && strcmp(attr, "shop-title") == 0)
		{

			char content[1024] = {0};
			int len = html_node_extract_content(html_tag->html_node, content, 1024);

			StructureKV temp_kv;
			temp_kv.key = 0;
			temp_kv.value = string(content, len);
			data->stdata->all.push_back(temp_kv);
			
		}
	}

	//��ַ
	else if (html_tag->tag_type == TAG_A)
	{
		char *attr = get_attribute_value(html_tag, ATTR_CLASS);
		if(attr != NULL && strcmp(attr, "link-dk") == 0 && html_tag->html_node->next != NULL)
		{

			char content1[2048] = {0};
			char content2[1024] = {0};
			int len1 = html_node_extract_content(html_tag->html_node, content1, 1024);
			int len2 = html_node_extract_content(html_tag->html_node->next, content2, 1024);
			content1[len1] = '\0';
			content2[len2] = '\0';

			memcpy(content1+len1, content2, len2);
			StructureKV temp_kv;
			temp_kv.key = 1;
			temp_kv.value = string(content1, len1+len2);
			data->stdata->all.push_back(temp_kv);


		}
	}

	//������ or �Ǳ�
	else if (html_tag->tag_type == TAG_EM || html_tag->tag_type == TAG_SPAN)
	{
		char *attr = get_attribute_value(html_tag, "itemprop");
		if(attr != NULL && strcmp(attr, "count") == 0)
		{

			char content[100] = {0};
			int len = html_node_extract_content(html_tag->html_node, content, 100);

			StructureKV temp_kv;
			temp_kv.key = 2;
			temp_kv.value = string(content, len);
			
			data->stdata->all.push_back(temp_kv);

		}
		//�Ǳ�-���Ǽ�
		if(html_tag->tag_type == TAG_SPAN && html_tag->html_node->parent != NULL && html_tag->html_node->parent->html_tag.tag_type == TAG_DIV)
		{

			char *attr_div = get_attribute_value(&html_tag->html_node->parent->html_tag, ATTR_CLASS);
			char *attr_class = get_attribute_value(html_tag, ATTR_CLASS);
			char *attr_title = get_attribute_value(html_tag, ATTR_TITLE);
			if(attr_class != NULL && attr_title != NULL && attr_div != NULL && strcmp(attr_div, "comment-rst") == 0 && strstr(attr_class, "item-rank-rst irr-star") != NULL && (strstr(attr_title, "���̻�") != NULL || strstr(attr_title, "�����Ǽ�")))
			{
				int star_i = atoi(attr_class + 22);//22Ϊitem-rank-rst�ȵĴ���
				float star_f = 0.1 * star_i;
				char star_str[3] = {0};
				int len = sprintf(star_str, "%.1f", star_f);

				StructureKV temp_kv;
				temp_kv.key = 6;
				temp_kv.value = string(star_str, len);

				data->stdata->all.push_back(temp_kv);
			}
		}
	}
	//�˾��۸�
	else if (html_tag->tag_type == TAG_STRONG)
	{
		char *attr = get_attribute_value(html_tag, ATTR_CLASS);
		//printf("attr=%s\n\n\n", attr);
		if(attr != NULL && strcmp(attr, "stress") == 0)
		{

			char content[100] = {0};
			int len = html_node_extract_content(html_tag->html_node, content, 100);

			char con[20] = {0};
			int len_con = sprintf(con, "%s", "&yen;189");
			len_con = copy_html_text(con, 0, 20, con);
			printf("con=%s,len_con=%d\n", con, len_con);

			StructureKV temp_kv;
			temp_kv.key = 3;
			temp_kv.value = string(content, len);
			data->stdata->all.push_back(temp_kv);

		}
	}

	//meta������
	else if (html_tag->tag_type == TAG_META)
	{
		char *attr = get_attribute_value(html_tag, ATTR_NAME);
		if(attr != NULL && strcmp(attr, "Description") == 0 && html_tag->html_node->next != NULL)
		{

			char *attr_content = get_attribute_value(html_tag, ATTR_CONTENT);
			int len =  strlen(attr_content);

			StructureKV temp_kv;
			temp_kv.key = 4;
			temp_kv.value = string(attr_content, len);
			data->stdata->all.push_back(temp_kv);

		}
	}

	//mypos | �˾�
	else if (html_tag->tag_type == TAG_DIV)
	{
		char *attr = get_attribute_value(html_tag, ATTR_CLASS);
		if(attr != NULL && strcmp(attr, "breadcrumb") == 0 && html_tag->html_node->child != NULL)
		{

			html_node_t *node = html_tag->html_node->child;
			for(; node != NULL; node = node->next)
			{

				if(node->html_tag.tag_type == TAG_B && node->child != NULL)
				{
					char *attr_a = get_attribute_value(&node->child->html_tag, ATTR_HREF);
					int len_a = strlen(attr_a);

					char content[1024] = {0};
					int len = html_node_extract_content(node->child, content, 1024);//����
					content[len] = '\0';
					char content_black[1024] = {0};
					remove_str_blank(content, content_black);
					len = strlen(content_black);

					if (data->avail + len_a + len + 1 >= data->size)
						return VISIT_ERROR;

					data->avail += sprintf(data->buf+data->avail, "%s %s ", content_black, attr_a);
				}
				else if(node->html_tag.tag_type == TAG_STRONG)
				{
					char content[1024] = {0};
					int len = html_node_extract_content(node->child, content, 1024);//����

					if (data->avail + len + 1 >= data->size)
						return VISIT_ERROR;

					data->avail += sprintf(data->buf+data->avail, "%s", content);
				}
			}


		}
		else if(attr != NULL && strcmp(attr, "shop-name") == 0 && html_tag->html_node->next != NULL && html_tag->html_node->next->last_child != NULL)
		{
			html_node_t *node_last = html_tag->html_node->next->last_child;
			
			if(node_last->last_child != NULL && node_last->last_child->html_tag.tag_type == TAG_DD)
			{
				char content[50] = {0};
				int len = html_node_extract_content(node_last->last_child, content, 50);

				StructureKV temp_kv;
				temp_kv.key = 3;
				temp_kv.value = string(content, len);
				data->stdata->all.push_back(temp_kv);

			}


		}
	}


	return 0;
}

//�ṹ�����ݳ�ȡ
int html_tree_extract_stdata(const char *url, int url_len, unsigned int pagetype, html_tree_t *html_tree, StructureData *stdata)
{

	stdata->type = 0;
	stdata->version = 0;
	stdata->all.clear();

	stdata->version = 1;
	stdata->type = pagetype;

	char tmp_buf[40960] = {0};
	char tmp_buf1[40960] = {0};
	char tmp_buf2[40960] = {0};

	char tmp_buf_title[10][40960] = {{0}};
	char tmp_buf_desp[10][40960] = {{0}};

	//�ٶȰٿ�
	if(strncmp(url, "http://baike.baidu.com/view/", 28) == 0 || strncmp(url, "http://baike.baidu.com/subview/", 31) == 0)
	{
		
		//����
		StructureKV kv_data;
		kv_data.key = 1;
		kv_data.value = string(url, url_len);
		stdata->all.push_back(kv_data);

		visit_for_bk_t data;
		memset(&data, 0, sizeof(visit_for_bk_t));
		data.url = url;
		data.url_len = url_len;
		data.stdata = stdata;
		data.buf = tmp_buf;
		data.avail = 0;
		data.size = 40960;
		data.dir_num = 1;
		data.count = 1;
		data.count1 = 1;

		for(int i = 0; i < 10; i++)
		{
			data.title[i]= tmp_buf_title[i];
			data.desp[i] = tmp_buf_desp[i];
		}
		data.number_title = 0;
		data.number_desp = 0;

		html_tree_visit(html_tree, start_visit_bk, NULL, &data, 0);
		//data.buf[data.avail] = 0;

		if(data.avail > 0)
		{
			StructureKV temp_kv;
			temp_kv.value = string(data.buf, data.avail);
			temp_kv.key = 3;
			stdata->all.push_back(temp_kv);
		}

		for(int i = 0; i< data.number_title; i++)
		{
			int len = strlen(data.desp[i]);
			if(strcmp(data.title[i], "����") == 0)
			{
				StructureKV temp_kv;
				temp_kv.key = 4;
				temp_kv.value = string(data.desp[i], len);
				stdata->all.push_back(temp_kv);
			}
			else if(strcmp(data.title[i], "֢״") == 0)
			{
				StructureKV temp_kv;
				temp_kv.key = 5;
				temp_kv.value = string(data.desp[i], len);
				stdata->all.push_back(temp_kv);
			}
			else if(strcmp(data.title[i], "Ԥ��") == 0)
			{
				StructureKV temp_kv;
				temp_kv.key = 6;
				temp_kv.value = string(data.desp[i], len);
				stdata->all.push_back(temp_kv);
			}
			else if(strcmp(data.title[i], "����") == 0)
			{
				StructureKV temp_kv;
				temp_kv.key = 7;
				temp_kv.value = string(data.desp[i], len);
				stdata->all.push_back(temp_kv);
			}
			else if(strcmp(data.title[i], "���") == 0)
			{
				StructureKV temp_kv;
				temp_kv.key = 8;
				temp_kv.value = string(data.desp[i], len);
				stdata->all.push_back(temp_kv);
			}
			else if(strcmp(data.title[i], "���") == 0)
			{
				StructureKV temp_kv;
				temp_kv.key = 9;
				temp_kv.value = string(data.desp[i], len);
				stdata->all.push_back(temp_kv);
			}
			else if(strcmp(data.title[i], "����֢") == 0)
			{
				StructureKV temp_kv;
				temp_kv.key = 10;
				temp_kv.value = string(data.desp[i], len);
				stdata->all.push_back(temp_kv);
			}
		}
	}
	else if(strncmp(url, "http://jingyan.baidu.com/article/", 33) == 0)
	{ //�ٶȾ���

		//����
		StructureKV kv_data;
		kv_data.key = 1;
		kv_data.value = string(url, url_len);
		stdata->all.push_back(kv_data);

		visit_for_jy_t data1;
		memset(&data1, 0, sizeof(visit_for_jy_t));
		data1.url = "http://jingyan.baidu.com";
		data1.url_len = 24;
		data1.stdata = stdata;
		data1.buf_img = tmp_buf;
		data1.buf_jy = tmp_buf1;
		data1.buf_jq = tmp_buf2;
		data1.avail_img = 0;
		data1.avail_jy = 0;
		data1.avail_jq = 0;
		data1.size = 40960;
		data1.count = 1;
		html_tree_visit(html_tree, start_visit_jy, NULL, &data1, 0);
		//data1.buf_img[data1.avail_img] = 0;
		//data1.buf_jy[data1.avail_jy] = 0;
		//data1.buf_jq[data1.avail_jq] = 0;

		//ͼƬ
		if(data1.avail_img > 0)
		{
			StructureKV temp_kv;
			temp_kv.key = 3;
			temp_kv.value = string(data1.buf_img, data1.avail_img);
			stdata->all.push_back(temp_kv);
		}
		//��ؾ���
		if(data1.avail_jy > 0)
		{
			StructureKV temp_kv;
			temp_kv.key = 4;
			temp_kv.value = string(data1.buf_jy, data1.avail_jy);
			stdata->all.push_back(temp_kv);
		}
		//���ɴ�ȫ
		if(data1.avail_jq > 0)
		{
			StructureKV temp_kv;
			temp_kv.key = 5;
			temp_kv.value = string(data1.buf_jq, data1.avail_jq);
			stdata->all.push_back(temp_kv);
		}


	}

	//���ڵ���
	else if(strncmp(url, "http://www.dianping.com/shop/", 29) == 0 && strstr(url+29, "/") == NULL)
	{

		visit_for_dp_t data;
		memset(&data, 0, sizeof(visit_for_dp_t));
		data.stdata = stdata;
		data.buf = tmp_buf;
		data.avail = 0;
		data.size = 40960;
		html_tree_visit(html_tree, start_visit_dp, NULL, &data, 0);
		//data.buf[data.avail] = 0;
		
		//mypos
		if(data.avail > 0)
		{
			StructureKV temp_kv;
			temp_kv.key = 5;
			temp_kv.value = string(data.buf, data.avail);
			stdata->all.push_back(temp_kv);
		}

	}

	return 0;

}


//ͨ������ҳ��
enum extract_type_enum
{
	ENUM_TIME = 0, 
	ENUM_VERSION = 1, 
	ENUM_SIZE = 2, 
	ENUM_OS = 3, 
	ENUM_FREE = 4, 
	ENUM_INTRODUCE = 5, 
	ENUM_LAST = 6,
};

const static char *g_seps[] =
{ "��", ":", "]", "��", 0 };//�ָ�


//�����ı��еķָ����ĸ���
int find_num_seps(char *text, int &end)
{
	char seps[10] = {0};
	int ret = 0;
	for(int i = 0; g_seps[i]; i++)
	{
		char *str = strstr(text, g_seps[i]);
		if(str != NULL)
		{
			if(strlen(str) == strlen(g_seps[i]))
				end = 1;//�ָ�����β
			sprintf(seps, "%s", g_seps[i]);
			break;
		}
	}
	char content[2048] = {0};
	int len = sprintf(content, "%s", text);
	content[len] = '\0';
	while(seps[0] != '\0' && content != NULL)
	{
		char *sub_str = strstr(content, seps);
		if(sub_str != NULL)
		{
			ret++;
			len = sprintf(content, "%s", sub_str + strlen(seps));
			content[len] = '\0';
		}
		else
			break;
	}
	return ret;
}



const static char *g_game_time[] =
{ "����ʱ��", "��������", "����ʱ��", "��������", "����", "ʱ��", "[ʱ��]", 0 }; //����ʱ�䣺 �������ڣ� ���£� ����ʱ�䣺
const static char *g_game_version[] =
{ "�汾��Ϣ", "�汾", "[�� ��", "[�汾", 0 }; //�汾�� �汾��Ϣ��
const static char *g_game_size[] =
{ "�ļ���С", "��Դ��С", "��Ϸ��С", "Ӧ�ô�С", "�����С", "��С", "[�����С]", "[�ļ���С", "[��С", 0 }; //�ļ���С�� ��Դ��С�� ��С�� ��Ϸ��С��
const static char *g_game_os[] =
{ "ϵͳҪ��", "����ϵͳ", "���û���", "����ϵͳ", "������", "֧��", "ϵͳ", "��ǰ�������", "�������", "���л���", "[ϵͳҪ��]", 0 }; //���û��ͣ� �����ڣ�
const static char *g_game_free[] =
{ "�ʷ�", "�շ�����", "�Ƿ��շ�", "Ӧ�ü۸�", "��Ȩ��ʽ", "��Ȩ", "�������", "�۸�", "�����Ȩ", "[�ʷ�", "[�� ��", 0 }; //�ʷѣ� �շ����ͣ� �Ƿ��շѣ� ���
const static char *g_game_introduce[] =
{ "���", "����", 0 };


struct extract_desp_t
{
	extract_type_enum type;
	const char** words;
	int flag; //����ȥ���ظ� ��ֹһ���ؼ��ֳ��ֶ��
};

extract_desp_t g_desps[ENUM_LAST] =
{
	{ ENUM_TIME, g_game_time, 0 },
	{ ENUM_VERSION, g_game_version, 0 },
	{ ENUM_SIZE, g_game_size, 0 },
	{ ENUM_OS, g_game_os, 0 },
	{ ENUM_FREE, g_game_free, 0 },
	{ ENUM_INTRODUCE, g_game_introduce, 0 }, 
};


//��������а����ؼ����Լ��ָ�� �����������أ�1 �����ؼ��ֵ��������ָ������2   ���������Էָ����β ���� 1  ���Էָ����β ����0
//���ر�� ���ڼ�顢���ܿ��޷ָ����(���������key�����ǿո�)
int is_contain(char *text, extract_desp_t *desp, char *des)
{
	if (text == NULL)
		return -1;
	for (int i = 0; desp->words[i] && desp->flag == 0; i++)
	{
		char *key = strstr(text, desp->words[i]);
		if(key == NULL)
			continue;
		int len_key = strlen(key);
		char *seps;
		for (int j = 0; g_seps[j]; j++) //Ҫ��֤word֮����Ƿָ�� ��eg���°汾���ĳ���-�����ؼ��֣�
		{
			seps = strstr(key, g_seps[j]);
			if(seps == NULL || strlen(seps) + strlen(desp->words[i]) != strlen(key))
				continue;
			else if (strncmp((key + strlen(desp->words[i])), g_seps[j], strlen(g_seps[j])) == 0 && (desp->type == 5 || len_key == strlen(text))) //�Ƚ�ȥ���ո��ĳ���
			{
				if (strlen(seps) == strlen(g_seps[j]))
				{
					return 1;
				}
				else
				{
					char *src = seps + strlen(g_seps[j]);
					int len = sprintf(des, "%s", src);
					des[len] = '\0';
					return 0;
				}
			}
			else
				break;
		}
		if (seps == NULL)
			return 2;
	}
	return -1;
}

//����һ���ı��ڵ� �����ܰ����ָ���-������ʱ�䡢����ֶ� classify��ʾ0-app����1-music
char *find_next_text(html_vnode_t *vnode, int flag, int i, int classify)
{
	char content[2048] = {0};
	if (vnode->nextNode != NULL)
	{
		html_vnode_t *vnode_next = vnode->nextNode;
		int len = html_node_extract_content(vnode_next->hpNode, content, 2048);
		content[len] = '\0';
		if(flag == 1)
			return (len > 0 ? content : NULL);
		else if(flag == 2)
			return (len > 10 ? content : NULL);
	}
	else if (vnode->upperNode != NULL && vnode->upperNode->nextNode != NULL)
	{
		html_vnode_t *vnode_upper = vnode->upperNode->nextNode;
		if (vnode_upper->hpNode->html_tag.tag_type == TAG_PURETEXT)
		{
			char *text = vnode_upper->hpNode->html_tag.text;
			if(classify == 0)
			{
				if(flag == 1 && (i == ENUM_TIME || i == ENUM_INTRODUCE || (strstr(text, "��") == NULL && strstr(text, ":") == NULL)))
					return (strlen(text) > 0 ? text : NULL);
				else if(flag == 2 && (i == ENUM_TIME || i == ENUM_INTRODUCE || (strstr(text, "��") == NULL && strstr(text, ":") == NULL)))
					return (strlen(text) > 10 ? text : NULL);
			}
			else if(classify == 1)
			{
				if(flag == 1 && (i == 1 || (strstr(text, "��") == NULL && strstr(text, ":") == NULL)))
					return (strlen(text) > 0 ? text : NULL);
				else if(flag == 2 && (i == 1 || (strstr(text, "��") == NULL && strstr(text, ":") == NULL)))
					return (strlen(text) > 10 ? text : NULL);
			}
		}
		else
		{
			int len = html_node_extract_content(vnode_upper->hpNode->child, content, 2048);
			content[len] = '\0';
			if(classify == 0)
			{
				if(flag == 1 && (i == ENUM_TIME || i == ENUM_INTRODUCE || (strstr(content, "��") == NULL && strstr(content, ":") == NULL)))
					return (len > 0 ? content : NULL);
				else if(flag == 2 && (i == ENUM_TIME || i == ENUM_INTRODUCE || (strstr(content, "��") == NULL && strstr(content, ":") == NULL)))
					return (len > 10 ? content : NULL);
			}
			else if(classify == 1)
			{
				if(flag == 1 && (i == ENUM_TIME || i == ENUM_INTRODUCE || (strstr(content, "��") == NULL && strstr(content, ":") == NULL)))
					return (len > 0 ? content : NULL);
				else if(flag == 2 && (i == 1 || (strstr(content, "��") == NULL && strstr(content, ":") == NULL)))
					return (len > 10 ? content : NULL);
			}
		}
	}
	return NULL;
}

struct extract_mypos
{
	const char *url;//ҳ��url
	int url_len;
	char *content;//���mypos����+url
	int con_len;
	int size;
	char *seps;//���mypos�еķָ���
};

struct extract_download
{
	StructureData *stdata;
	int flag;//��Ҫ��֤���ص�ַֻȡһ��
	const char *url;//���ص�ַ�����·��ʱ�����в���
	int url_len;
};


void remove_space(char *text, int ret)//retΪ�ָ������� ����1ʱ Ҳ�ÿո�ָ�
{
	int len = strlen(text);
	if((ret == 0 || ret == 1) && (text[0] >= 'a' && text[0] <= 'z') || (text[0] >= 'A' && text[0] <= 'Z'))
	{
		for(int i = 0; i < len; i++)
		{
			if(i != 0 && (text[i] == '\n' || text[i] == '\t' || text[i] == '-' || text[i] == '_' || text[i] == '('))
			{
				text[i] = '\0';
				break;
			}
		}
	}
	else
	{	for(int i = 0; i < len; i++)
		{
			if(i != 0 && (text[i] == ' ' || text[i] == '\n' || text[i] == '\t' || text[i] == '-' || text[i] == '_' || text[i] == '('))
			{
				text[i] = '\0';
				break;
			}
		}
	}
}

//��Ҫ�������е�key_value��ʽ(http://down.tech.sina.com.cn/content/49535.html ���� �����ȡ��������ԭ����ȡ������������)
int start_for_key_value(html_vnode_t *vnode, void *data)
{
	extract_download *down = (extract_download *)data;

	char *attr_title = get_attribute_value(&vnode->hpNode->html_tag, ATTR_TITLE);
	char *attr_href = get_attribute_value(&vnode->hpNode->html_tag, ATTR_HREF);//����Ϊ���ص�ַ����֤����/����http://���ߣ���ͷ(����ʾ����ҳ�ڲ���ת)

	//if (!vnode->isValid)
	//	return VISIT_SKIP_CHILD;

	if (vnode->hpNode->html_tag.tag_type == TAG_PURETEXT)
	{
		char *text = vnode->hpNode->html_tag.text;
		if(strlen(text) > 2048)
			return VISIT_SKIP_CHILD;
		char text_copy[2048] = {0};
		char text_blank[2048] = {0};
		int len_text = sprintf(text_copy, "%s", text);
		text_copy[len_text] = '\0';

		int end = 0;
		int num = find_num_seps(text_copy, end);
		if(num == 1)//???�ָ���������ַ���Ӧȥ�ո�???
		{
			clean_string(text_copy);
			copy_html_text(text_copy, 0, 2048, text_copy);
			remove_str_blank(text_copy, text_blank);
			len_text = strlen(text_blank);
		}
		else
		{
			len_text = sprintf(text_blank, "%s", text);
			text_blank[len_text] = '\0';
		}

		for (int i = ENUM_TIME; i < ENUM_LAST; i++) //���ָ�����β������ȡ���Ȳ��ܳ���30
		{
			//printf("g_desps[%d].flag=%d\t", i, g_desps[i].flag);
			char des[2048] = {0};
			int ret = is_contain(text_blank, &(g_desps[i]), des); //�������ؼ��ַ�ͬһ���ı��飬��ᶪʧ???�������Ӳ���-�ָ�������������
			//printf("text=%s,ret=%d\n", text_blank, ret);
			if (ret == 0) //�ָ������β
			{
				char text_end[2048] = {0};
				//���text�Ƿ�����ؼ��� �� �����ָ�� �ָ����β �� ����β(ȥ��ת���ַ�)
				int len = copy_html_text(text_end, 0, 2048, des);
				//����ʷѣ����������ܺ��пհ��ַ�
				if(i == ENUM_FREE)
					remove_space(text_end, 0);	
				StructureKV temp_kv;
				temp_kv.key = i;
				temp_kv.value = string(text_end, strlen(text_end));
				if(len > 0)
				{
					down->stdata->all.push_back(temp_kv);	
					g_desps[i].flag = 1;
				}
			}
			else if (ret == 1 || (ret == 2 && i == ENUM_INTRODUCE)) //�ָ����β �����ֶȽڵ������ �����丸�ڵ���ֵܽڵ������
			{
				char *text_next = find_next_text(vnode, ret, i, 0);

				if (text_next != NULL)
				{
					char text_end[2048] = {0};//���ȡǰ100����
					int len = copy_html_text(text_end, 0, 2048, text_next);
					//printf("222i=%d\ttext=%s\t%s\n", i, text, text_end);
					StructureKV temp_kv;
					temp_kv.key = i;
					temp_kv.value = string(text_end, len);
					if(len > 0)
					{
						down->stdata->all.push_back(temp_kv);	
						g_desps[i].flag = 1;
					}

				}
			}
		}
		return VISIT_SKIP_CHILD;
	}
	//���������꣬��ֹ��ȡ��ͷ���Ĺ�����ص�ַ
	else if(vnode->ypos > 90 && vnode->hpNode->html_tag.tag_type == TAG_A && attr_href != NULL)
	{
		char content[2048] = {0};
		int len = html_node_extract_content(vnode->hpNode, content, 2048);
		content[len] = '\0';

		if(((attr_title != NULL && strstr(attr_title, "����") != NULL) || (content != NULL && strstr(content, "����") != NULL && strstr(content, "������ҳ") == NULL)) && (strstr(attr_href, "down") != NULL || strstr(attr_href, ".exe") != NULL || strstr(attr_href, ".jar") != NULL || strstr(attr_href, ".zip") != NULL || strstr(attr_href, ".rar") != NULL) && strstr(attr_href, "help") == NULL && down->flag == 0)
		{
			char url_end[2048] = {0};
			char url_full[2048] = {0};
			int len_url_end = copy_html_text(url_end, 0, 2048, attr_href);
			url_end[len_url_end] = '\0';
			int len_url_full = 0;

			//������·���Լ�ת���ַ����в���
			if (strncmp(url_end, "http://", 7) == 0)
			{
				len_url_full = sprintf(url_full, "%s", url_end);
			}
			else if(strncmp(url_end, "#", 1) == 0)
			{
				len_url_full = sprintf(url_full, "%s%s", down->url, url_end);

			}
			else if(strncmp(url_end, "/", 1) == 0 || strncmp(url_end, "../", 3) == 0)
			{
				strncpy(url_full, down->url, down->url_len);
				url_full[down->url_len] = '\0';
				char *ptr = strchr(url_full + 7, '/');
				int pos = ptr - url_full;
				len_url_full = sprintf(url_full+pos, "%s", strstr(url_end, "/"));
				len_url_full += pos;
				//strcpy(url_full + pos, url_end);
				
			}
			else
			{
				strncpy(url_full, down->url, down->url_len);
				url_full[down->url_len] = '\0';
				char *ptr = strrchr(url_full + 7, '/');
				int pos = ptr - url_full;
				len_url_full = sprintf(url_full+pos+1, "%s", url_end);
				len_url_full += pos+1;
			}

			StructureKV temp_kv;
			temp_kv.key = 8;
			temp_kv.value = string(url_full, len_url_full);
			if(len_url_full > 0)
			{
				down->stdata->all.push_back(temp_kv);
				down->flag = 1;
				if(strstr(content, "���") != 0 && g_desps[4].flag == 0)//����ê���г������������
				{
					StructureKV temp_kv;
					temp_kv.key = 4;
					temp_kv.value = string("���", 4);
					down->stdata->all.push_back(temp_kv);
					g_desps[4].flag = 1;
				}
			}
		}
		return VISIT_SKIP_CHILD;
	}
	return VISIT_NORMAL;
}

//��ȡmypos(����urlδȥ��mypos�еķָ���)
int start_for_mypos(html_vnode_t *vnode, void *data) //�����ֽڵ�  ��������ǰλ�� ���� mypos�ķָ���Ⱦ�Ҫȥ����
{
	extract_mypos *mypos = (extract_mypos *)data;
	if (!vnode->isValid)
		return VISIT_SKIP_CHILD;

	if (vnode->hpNode->html_tag.tag_type == TAG_PURETEXT && vnode->hpNode->html_tag.textlength < 20)
	{
		char content[4096] = {0};
		int len = html_node_extract_content(vnode->hpNode, content, 4096);
		content[len] = '\0';

		if (len < 4 || strstr(content, "λ�ã�") != 0 || strstr(content, "λ��:") != 0 || strstr(content, "��ǰλ��") != 0)
		{
			if(len < 4)
			{
				strncpy(mypos->seps, content, len);//��ָ���
				mypos->seps[len]= '\0';
			}
			return VISIT_SKIP_CHILD;
		}

		//��Ҫ�ж��Ƿ��пռ�д��
		//if (len + 1 >= mypos->size)
		//	return VISIT_ERROR;

		//ȥ���հ��ַ� �Լ� mypos�ķָ��� ��contentת��

		clean_string(content);
		clean_string(mypos->seps);

		//�ж��������Ƿ�����ָ���

		if(mypos->seps != NULL && strncmp(content, mypos->seps, strlen(mypos->seps)) == 0)
			mypos->con_len += sprintf(mypos->content + mypos->con_len, "%s ", content+strlen(mypos->seps));
		else if(strncmp(content, ">>", 2) == 0 || strncmp(content, "->", 2) == 0)
			mypos->con_len += sprintf(mypos->content + mypos->con_len, "%s ", content+2);
		else if(strncmp(content, ">", 1) == 0)
		{
			mypos->con_len += sprintf(mypos->content + mypos->con_len, "%s ", content+1);
		}
		else
			mypos->con_len += sprintf(mypos->content + mypos->con_len, "%s ", content);

		char *attr_href = get_attribute_value(&vnode->upperNode->hpNode->html_tag, ATTR_HREF);
		if (vnode->upperNode->hpNode->html_tag.tag_type == TAG_A && attr_href != NULL)
		{
			char href[2048] = {0};
			copy_html_text(href, 0, 2048, attr_href);
			if (strncmp(attr_href, "http://", 7) == 0)
			{
				mypos->con_len += sprintf(mypos->content + mypos->con_len, "%s ", href);
			}
			else if(strncmp(attr_href, "/", 1) == 0 || strncmp(attr_href, "../", 3) == 0)
			{
				char url_new[2048] = {0};
				strncpy(url_new, mypos->url, mypos->url_len);
				url_new[mypos->url_len] = '\0';
				char *ptr = strchr(url_new + 7, '/');
				int pos = ptr - url_new;
				strcpy(url_new + pos, strstr(href, "/"));
				mypos->con_len += sprintf(mypos->content + mypos->con_len, "%s ", url_new);
			}
			//���·��������/
			else
			{
				char url_new[2048] = {0};
				strncpy(url_new, mypos->url, mypos->url_len);
				url_new[mypos->url_len] = '\0';
				char *ptr = strrchr(url_new + 7, '/');
				int pos = ptr - url_new;
				strcpy(url_new + pos + 1, href);
				mypos->con_len += sprintf(mypos->content + mypos->con_len, "%s ", url_new);
			}
		}
	}
	return VISIT_NORMAL;
}

//��ȡĳ���ڵ��µ�img��ǩ��src����
char *get_sub_img_vnode(html_vnode_t *vnode, const char *url, int url_len)
{
	std::stack<html_vnode_t *> stack_node; //����ջ

	if (vnode == NULL || vnode->firstChild == NULL)
		return NULL;

	html_vnode_t *node = vnode->firstChild;

	while (node != NULL || !stack_node.empty())
	{
		while (node != NULL)
		{
			//ȥ������height��width��img��ǩ����ȥ����׺��Ϊ.gif��src
			char *attr_src = get_attribute_value(&node->hpNode->html_tag, ATTR_SRC);
			char *attr_height = get_attribute_value(&node->hpNode->html_tag, ATTR_HEIGHT);
			char *attr_width = get_attribute_value(&node->hpNode->html_tag, ATTR_WIDTH);
			//if (node->hpNode->html_tag.tag_type == TAG_IMG && attr_src != NULL && strstr(attr_src, ".gif") == NULL && attr_height == NULL && attr_width == NULL)
			if (node->hpNode->html_tag.tag_type == TAG_IMG && attr_src != NULL && attr_height == NULL && attr_width == NULL)
			{
				//printf("src=%s\txpos=%d, ypos=%d, wx=%d, hx=%d, trust=%d\n", attr_src, node->xpos, node->ypos, node->wx, node->hx, node->trust);
				//node = node->nextNode;
				if (strncmp(attr_src, "http://", 7) == 0)
					return attr_src;
				else if(strncmp(attr_src, "/", 1) == 0)
				{
					static char url_new[2048] = {0};
					strncpy(url_new, url, url_len);
					url_new[url_len] = '\0';
					char *ptr = strchr(url_new + 7, '/');
					int pos = ptr - url_new;
					strcpy(url_new + pos, attr_src);
					return url_new;
				}
				else
					//return NULL;	
					break;
			}
			else
			{
				stack_node.push(node);
				node = node->firstChild;
			}

		}
		if (!stack_node.empty())
		{
			node = stack_node.top();
			stack_node.pop();
			node = node->nextNode;
		}
	}
	return NULL;
}


//ͨ������ҳ��
int html_tree_extract_download_stdata(const char *url, int url_len, unsigned int pagetype, html_tree_t *html_tree, html_vtree_t *html_vtree, area_tree_t *atree, StructureData *stdata)
{
	stdata->type = 0;
	stdata->version = 0;
	stdata->all.clear();

	stdata->version = 1;
	stdata->type = pagetype;

	extract_download down;
	memset(&down, 0, sizeof(extract_download));
	down.stdata = stdata;
	down.flag = 0;
	down.url = url;
	down.url_len = url_len;
	
	//��ȡmypos ����+url

	extract_mypos mypos;
	//init
	char content[4096] = {0};
	char seps[4] = {0};
	memset(&mypos, 0, sizeof(extract_mypos));
	mypos.url = url;
	mypos.url_len = url_len;
	mypos.content = content;
	mypos.con_len = 0;
	mypos.size = 4096;
	mypos.seps = seps;

	
	//��ȡkey_value��ʽ �Լ����ص�ַurl
	//��Ϊ���޸�g_desps��flag����������Ҫÿ�γ�ʼ��
	for(int i = ENUM_TIME; i < ENUM_LAST; i++)
		g_desps[i].flag = 0;

	html_vtree_visit(html_vtree, start_for_key_value, NULL, &down);

	const area_list_t *alist = get_func_mark_result(atree, AREA_FUNC_MYPOS);
	if (alist && alist->head && alist->head->area->begin)
	{
		//mypos
		html_vnode_t *vnode_t = alist->head->area->begin;
		if(vnode_t->hpNode->html_tag.tag_type == TAG_DIV || vnode_t->hpNode->html_tag.tag_type == TAG_TABLE)
			html_vnode_visit_ex(vnode_t, start_for_mypos, NULL, &mypos);
		else
		{
			html_tag_type_t type = vnode_t->hpNode->html_tag.tag_type;
			for(html_vnode_t *v_node = vnode_t; v_node && (v_node->hpNode->html_tag.tag_type == type || v_node->hpNode->html_tag.tag_type == TAG_PURETEXT); v_node = v_node->nextNode)
				html_vnode_visit_ex(v_node, start_for_mypos, NULL, &mypos);
		}
		//LOGO ����Ч�ڵ�
		while(vnode_t->nextNode != NULL && !vnode_t->nextNode->isValid)
			vnode_t = vnode_t->nextNode;
		if(vnode_t->nextNode != NULL)
		{
			char *attr_value = get_sub_img_vnode(vnode_t->nextNode, url, url_len);
			if(attr_value != NULL)	
			{
				StructureKV temp_kv;
				temp_kv.value = string(attr_value, strlen(attr_value));
				temp_kv.key = 7;
				stdata->all.push_back(temp_kv);
			}

		}

	}

	
	if(mypos.con_len > 0)
	{
		StructureKV temp_kv;
		temp_kv.value = string(mypos.content, mypos.con_len);
		temp_kv.key = 6;
		stdata->all.push_back(temp_kv);
		
	}

	return 0;
}

//MP3��
enum extract_music_type_enum
{
	ENUM_MUSIC_SING = 0, 
	ENUM_MUSIC_TIME = 1, 
	ENUM_MUSIC_ALBUM = 2, 
	ENUM_MUSIC_COMPANY = 3, 
	ENUM_MUSIC_LYRICS = 4, 
	ENUM_MUSIC_COMPOSITION = 5, 
	ENUM_MUSIC_SONG = 6, 
	ENUM_MUSIC_VERTION = 7, 
	ENUM_MUSIC_SIZE = 8, 
	ENUM_MUSIC_LAST = 9,
};


const static char *g_music_sing[] =
{ "�ݳ���", "����", "������", "����", "�ݳ�", "ԭ��", "��������", "��������", 0 }; //������
const static char *g_music_time[] =
{ "����ʱ��", "����ʱ��", "ʱ��", 0 }; //����ʱ��
const static char *g_music_album[] =
{ "ר�����", "����ר��", "����ר��", "ר��", 0 }; //����ר��
const static char *g_music_company[] =
{ "��Ƭ��˾", "���й�˾", 0 }; //��Ƭ��˾
const static char *g_music_lyrics[] = 
{ "����", "����", "��/��", "����/����", "��", 0 }; //����
const static char *g_music_composition[] = 
{ "����", "����", "��/��", "����/����", "��", 0 }; //���� (���ؼ���������ؼ��ֳ�ͻ)
const static char *g_music_song[] =
{ "������", "����", "����", "��������", "����������", "��������", 0 }; //������"��������"
const static char *g_music_version[] =
{ "�汾��Ϣ", "�汾", 0 }; //�汾�� �汾��Ϣ��
const static char *g_music_size[] =
{ "�ļ���С", "��Դ��С", "��С", 0 }; //�ļ���С�� ��Դ��С�� ��С�� 


struct extract_music_desp_t
{
	extract_music_type_enum type;
	const char** words;
	int flag; //����ȥ���ظ� ��ֹһ���ؼ��ֳ��ֶ��
};


extract_music_desp_t g_music_desps[ENUM_MUSIC_LAST] =
{
	{ ENUM_MUSIC_SING, g_music_sing, 0 },
	{ ENUM_MUSIC_TIME, g_music_time, 0 },
	{ ENUM_MUSIC_ALBUM, g_music_album, 0 },
	{ ENUM_MUSIC_COMPANY, g_music_company, 0 },
	{ ENUM_MUSIC_LYRICS, g_music_lyrics, 0 },
	{ ENUM_MUSIC_COMPOSITION, g_music_composition, 0 }, 
	{ ENUM_MUSIC_SONG, g_music_song, 0 }, 
	{ ENUM_MUSIC_VERTION, g_music_version, 0 }, 
	{ ENUM_MUSIC_SIZE, g_music_size, 0 }, 
};


//��������а����ؼ����Լ��ָ�� �����������أ�1 �����ؼ��ֵ��������ָ������2   ���������Էָ����β ���� 1  ���Էָ����β ����0 des����ָ����������
int is_music_contain(char *text, extract_music_desp_t *desp, char *des, int ret)
{
	if (text == NULL)
		return -1;
	for (int i = 0; desp->words[i] && desp->flag == 0; i++)
	{
		//���зָ�������Ҫ��֤�ҵ����ı���С��ؼ��ִ�Сһ�����������Ƽ���������
		//if(strncmp(text, desp->words[i]))
		char *key = strstr(text, desp->words[i]);
		if (key == NULL)
			continue;
		char *seps = 0;
		for (int j = 0; g_seps[j]; j++) //Ҫ��֤word֮����Ƿָ�� ��eg���°汾���ĳ���-�����ؼ��֣�
		{
			seps = strstr(key, g_seps[j]);
			if(seps == NULL)	
				continue;
			else if(ret > 1 && strncmp((key + strlen(desp->words[i])), g_seps[j], strlen(g_seps[j])) == 0)
			{
				//ר�����http://music.show160.com/381973����ҳ�� http://www.mtv123.com/mp3/16426/195251.shtml
				char src[2048] = {0};
				sprintf(src, "%s", seps+strlen(g_seps[j]));
				char *sub = strstr(src, "��");
				int len_sub = 0;
				if(sub != NULL)//���ָ�
					len_sub = strlen(sub);
				else
					remove_space(src, ret);//�ո�ָ�
				int len = snprintf(des, strlen(src)-len_sub+1, "%s", src);
				des[len] = '\0';
				return 0;
			}
			else if (ret == 1 && strncmp((key + strlen(desp->words[i])), g_seps[j], strlen(g_seps[j])) == 0 && strlen(key)== strlen(text))
			{
					if (strlen(seps) == strlen(g_seps[j]))
						return 1;
					else
					{
						char *src = seps + strlen(g_seps[j]);
						int len = sprintf(des, "%s", src);
						des[len] = '\0';
						return 0;
					}
			}
			else
				break;
		}
		if (seps == NULL)
			return 2;
	}
	return -1;
}

struct extract_music
{
	StructureData *stdata;
	char *title;//<title>
	char *singer;//�ݳ������������1.��title���Ա� 2.��title�в���
	char *song;
	int num_br;//<BR>�ĸ���
	char *content;//����
	int len;
	unsigned int pagetype;
	const char *url;
	int url_len;
	int flag;
};


//��Ը�������������ȥ��������ַ���MP3����ʣ�����.��
void remove_false_str(char *text)
{
	char *wrong[] = {"MP3", "���", "(", "��", ".", 0};
	for(int i = 0; wrong[i] && text[0] != '\0'; i++)
	{
		char *str = strstr(text, wrong[i]);
		if(str != NULL)
			text[str - text] = '\0';
	}
}

//MP3���key_value
int start_for_music_key_value(html_vnode_t *vnode, void *data)
{
	extract_music *music = (extract_music *)data;

	char *attr_title = get_attribute_value(&vnode->hpNode->html_tag, ATTR_TITLE);
	char *attr_href = get_attribute_value(&vnode->hpNode->html_tag, ATTR_HREF);//����Ϊ���ص�ַ����֤����/����http://���ߣ���ͷ(����ʾ����ҳ�ڲ���ת)

	if (!vnode->isValid)
		return VISIT_SKIP_CHILD;

	if (vnode->hpNode->html_tag.tag_type == TAG_PURETEXT)
	{
		char *text = vnode->hpNode->html_tag.text;
		int len_text = strlen(text);
		if(len_text == 0 || len_text > 2048)
			return VISIT_SKIP_CHILD;

		char text_copy[2048] = {0};
		char text_blank[2048] = {0};
		len_text = sprintf(text_copy, "%s", text);
		text_copy[len_text] = '\0';

		int end = 0;
		int num = find_num_seps(text_copy, end); //����-ʱ�䣺2013-09-16 19:23
		if(num == 1)
		{
			clean_string(text_copy);
			copy_html_text(text_copy, 0, 2048, text_copy);
			if(strstr(text_copy, "ʱ��") == NULL && end == 1)
			{
				remove_str_blank(text_copy, text_blank);
				len_text = strlen(text_blank);
			}
			else
			{
				len_text = sprintf(text_blank, "%s", text_copy);
				text_blank[len_text] = '\0';

			}
		}
		else
		{
			len_text = sprintf(text_blank, "%s", text_copy);
			text_blank[len_text] = '\0';
		}
		for (int i = ENUM_MUSIC_SING; i < ENUM_MUSIC_LAST && len_text > 3; i++)//���˸�ʣ�����key_valueֵ��Ӧ��30����������
		{
			char des[2048] = {0};
			int ret = is_music_contain(text_blank, &(g_music_desps[i]), des, num);//�ؼ��־�������ȫƥ�����
			//printf("des=%s, ret=%d, i=%d\n", des, ret, i);

			if (ret == 0) //�ָ������β
			{
				char text_end[2048] = {0};
				//���text�Ƿ�����ؼ��� �� �����ָ�� �ָ����β �� ����β(ȥ��ת���ַ�)
				int len = copy_html_text(text_end, 0, 2048, des);
				clean_string(text_end);
				if(i != ENUM_MUSIC_TIME)
					remove_space(text_end, 0);
				if(i == 1 || i == 6)
					remove_false_str(des);

				len = strlen(text_end);
				//printf("111i=%d,text=%s,title=%s\n", i, text_end, music->title);//valueΪ�ָ���֮�������
				if(music->title[0] != '\0' && len > 0)
				{
					/*char titles[1024];
					char titles_non[1024];
					sprintf(titles, "%s", music->title);
					remove_str_blank(titles, titles_non);*/

					if(music->pagetype == 17 || (i != 0 && i != 6) || (strstr(music->title, text_end) != NULL))
					{
						g_music_desps[i].flag = 1;
						StructureKV temp_kv;
						temp_kv.key = i;
						temp_kv.value = string(text_end, len);
						music->stdata->all.push_back(temp_kv);
						if(i == 0 && len < 300)
							sprintf(music->singer, "%s", text_end);
						else if(i == 6 && len < 300)
							sprintf(music->song, "%s", text_end);
						return VISIT_SKIP_CHILD;
					}
				}
				else if(music->title[0] == '\0' && len > 0)
				{
					g_music_desps[i].flag = 1;
					StructureKV temp_kv;
					temp_kv.key = i;
					temp_kv.value = string(text_end, len);
					music->stdata->all.push_back(temp_kv);
					return VISIT_SKIP_CHILD;
				}
			}
			//else if (ret == 1 || ret == 2) //�ָ����β �����ֶȽڵ������ �����丸�ڵ���ֵܽڵ������
			else if (len_text < 50 && ret == 1) //�ָ����β �����ֶȽڵ������ �����丸�ڵ���ֵܽڵ������
			{
				char *text_next = find_next_text(vnode, ret, i, 1);
				if (text_next != NULL)
				{
					char text_end[2048] = {0};
					copy_html_text(text_end, 0, 2048, text_next);
					//printf("222i=%d,text=%s-%s\n", i, text, text_end);
					if(i == 1 || i == 6)
						remove_false_str(text_end);
					int len = strlen(text_end);
					if(music->title[0] != '\0' && len > 0)
					{
						if(music->pagetype == 17 || music->pagetype == 22 || (i != 0 && i != 6) || (strstr(music->title, text_end) != NULL))
						{
							clean_string(text_end);
							len = strlen(text_end);
							g_music_desps[i].flag = 1;
							StructureKV temp_kv;
							temp_kv.key = i;
							temp_kv.value = string(text_end, len);
							music->stdata->all.push_back(temp_kv);
							if(i == 0 && len < 300)
								sprintf(music->singer, "%s", text_end);
							else if(i == 6 && len < 300)
								sprintf(music->song, "%s", text_end);
							return VISIT_SKIP_CHILD;
						}
					}
				}
			}
		}
		return VISIT_SKIP_CHILD;
	}
	else if(vnode->ypos > 90 && vnode->hpNode->html_tag.tag_type == TAG_A && attr_href != NULL)
	{
		char content[2048] = {0};
		int len = html_node_extract_content(vnode->hpNode, content, 2048);
		content[len] = '\0';

		if(((attr_title != NULL && strstr(attr_title, "����") != NULL) || (content != NULL && strstr(content, "����") != NULL && strstr(content, "������ҳ") == NULL)) && (strstr(attr_href, "down") != NULL || strstr(attr_href, ".exe") != NULL || strstr(attr_href, ".jar") != NULL || strstr(attr_href, ".zip") != NULL || strstr(attr_href, ".rar") != NULL) && strstr(attr_href, "help") == NULL && music->flag == 0)
		{
			char url_end[2048] = {0};
			char url_full[2048] = {0};
			int len_url_end = copy_html_text(url_end, 0, 2048, attr_href);
			url_end[len_url_end] = '\0';
			int len_url_full = 0;

			//������·���Լ�ת���ַ����в���
			if (strncmp(url_end, "http://", 7) == 0)
			{
				len_url_full = sprintf(url_full, "%s", url_end);
			}
			else if(strncmp(url_end, "#", 1) == 0)
			{
				len_url_full = sprintf(url_full, "%s%s", music->url, url_end);

			}
			else if(strncmp(url_end, "/", 1) == 0 || strncmp(url_end, "../", 3) == 0)
			{
				strncpy(url_full, music->url, music->url_len);
				url_full[music->url_len] = '\0';
				char *ptr = strchr(url_full + 7, '/');
				int pos = ptr - url_full;
				len_url_full = sprintf(url_full+pos, "%s", strstr(url_end, "/"));
				len_url_full += pos;
				//strcpy(url_full + pos, url_end);
				
			}
			else
			{
				strncpy(url_full, music->url, music->url_len);
				url_full[music->url_len] = '\0';
				char *ptr = strrchr(url_full + 7, '/');
				int pos = ptr - url_full;
				len_url_full = sprintf(url_full+pos+1, "%s", url_end);
				len_url_full += pos+1;
			}

			StructureKV temp_kv;
			temp_kv.key = 9;
			temp_kv.value = string(url_full, len_url_full);
			if(len_url_full > 0)
			{
				music->stdata->all.push_back(temp_kv);
				music->flag = 1;
			}
		}
		return VISIT_SKIP_CHILD;
	}
	return VISIT_NORMAL;
}

void remove_time(char *text)
{
	char *str_so = strchr(text, '[');
	char *str_eo = strchr(text, ']');
	if(str_so != NULL && str_eo != NULL)
	{
		int pos_so = str_so - text;
		int pos_eo = str_eo - text;
		if(pos_eo - pos_so == 9)
		{
			if(pos_so == 0)
			{
				sprintf(text, "%s", text+pos_eo+1);
			}
			else
				text[pos_so] = '\0';
		}
	}

}
//<title>�е����� �Լ�TEXT-BR�ı�-���
int start_for_title_br(html_tag_t* html_tag, void* result, int flag)
{
	extract_music *music = (extract_music *)result;
	if(html_tag->tag_type == TAG_TITLE)
	{
		char content[1024] = {0};
		int len = html_node_extract_content(html_tag->html_node, content, 1024);
		content[len] = '\0';
		copy_html_text(content, 0, 1024, content);
		sprintf(music->title, "%s", content);
		return VISIT_SKIP_CHILD;
	}
	else if(html_tag->tag_type == TAG_PURETEXT && html_tag->html_node->next != NULL && html_tag->html_node->next->html_tag.tag_type == TAG_BR)
	{
		char text[2048] = {0};
		if(strlen(html_tag->text) > 2048)
			return VISIT_SKIP_CHILD;
		sprintf(text, "%s", html_tag->text);
		if(text != NULL && music->num_br < 10)//ȡǰ10��BR�����֣������������10�����ޣ��ָ���
		{
			remove_time(text);
			if(strlen(text) < 50 && strstr(text, "��") == NULL && strstr(text, "www.") == NULL && strstr(text, ":") == NULL && strstr(text, "-") == NULL && strstr(text, "=") == NULL && strstr(text, "��") == NULL && strstr(text, "���") == NULL)
			{
				music->num_br++;
				if(music->num_br < 4)
				{
					//��ȥ��content�е�ʱ���Լ���ͷ�Ļ��з� eg[00:00:00]
					copy_html_text(text, 0, 2048, text);
					music->len += sprintf(music->content + music->len, "%s ", text);
					music->content[music->len] = '\0';
				}
				return VISIT_SKIP_CHILD;
			}
			else
			{
				music->num_br = 0;
				music->len = 0;
				music->content[0] = '\0';
			}
		}
	}
	return VISIT_NORMAL;
}



//MP3��ҳ�����ȡ
int html_tree_extract_music_stdata(const char *url, int url_len, unsigned int pagetype, html_tree_t *html_tree, html_vtree_t *html_vtree, area_tree_t *atree, StructureData *stdata)
{

	stdata->type = 0;
	stdata->version = 0;
	stdata->all.clear();

	stdata->version = 1;
	stdata->type = pagetype;

	char title[1024] = {0};
	char singer[300] = {0};
	char song[300] = {0};
	char content[2048] = {0};
	extract_music music;
	memset(&music, 0, sizeof(extract_music));
	music.stdata = stdata;
	music.title = title;
	music.singer = singer;
	music.song = song;
	music.num_br = 0;
	music.content = content;
	music.len = 0;
	music.pagetype = pagetype;
	music.url = url;
	music.url_len = url_len;
	music.flag = 0;

	for(int i = ENUM_MUSIC_SING; i < ENUM_MUSIC_LAST; i++)
		g_music_desps[i].flag = 0;

	printf("parser-url=%s\n", url);		
	html_tree_visit(html_tree, start_for_title_br, NULL, &music, 0);

	html_vtree_visit(html_vtree, start_for_music_key_value, NULL, &music);


	char songs[300] = {0};
	char singers[300] = {0};
	
	//��Ը�����i=6�������i=0 �����ݳ��� �Ҹ���
	if(music.title[0] != '\0' && g_music_desps[0].flag == 1 && g_music_desps[6].flag == 0)
	{
		char *str = strstr(music.title, music.singer);
		if(str != NULL && strlen(str) < strlen(music.title))//title�ĸ�ʽΪ����+����
		{
			snprintf(songs, strlen(music.title) - strlen(str) +1, "%s", music.title);
			clean_string(songs);
			remove_space(songs, 0);
			remove_false_str(songs);
			sprintf(music.song, "%s", songs);

		}
		else if(str != NULL && strlen(str) == strlen(music.title)) //title�ĸ�ʽΪ����+����
		{
			sprintf(songs, "%s", music.title + strlen(music.singer));
			clean_string(songs);
			char *space = strchr(songs, ' ');
			if(space != NULL)
			{
				int pos = space - songs;
				if(pos < 5)
					sprintf(songs, "%s", songs+pos+1);
			}
			remove_space(songs, 0);
			remove_false_str(songs);
			if(songs[0] != '\0')
				sprintf(music.song, "%s", songs);
		}
		if(strlen(music.song) > 0)
		{
			g_music_desps[6].flag = 1;
			StructureKV temp_kv;
			temp_kv.key = 6;
			temp_kv.value = string(music.song, strlen(music.song));
			music.stdata->all.push_back(temp_kv);
		}
	}
	//���ڸ����� ���ݳ���
	else if(music.title[0] != '\0' && g_music_desps[0].flag == 0 && g_music_desps[6].flag == 1)
	{
		char *str = strstr(music.title, music.song);
		if(str != NULL && strlen(str) < strlen(music.title))//title�ĸ�ʽΪ����+����
		{
			snprintf(singers, strlen(music.title) - strlen(str) +1, "%s", music.title);
			clean_string(singers);
			remove_space(singers, 0);
			remove_false_str(singers);
			sprintf(music.singer, "%s", singers);

		}
		else if(str != NULL && strlen(str) == strlen(music.title))
		{
			sprintf(singers, "%s", music.title + strlen(music.song));
			clean_string(singers);
			char *space = strchr(singers, ' ');
			if(space != NULL)
			{
				int pos = space - singers;
				if(pos < 5)
					sprintf(singers, "%s", singers + pos + 1);
			}
			remove_space(singers, 0);
			remove_false_str(singers);
			sprintf(music.singer, "%s", singers);

		}
		if(strlen(music.singer) > 0)
		{
			g_music_desps[0].flag = 1;
			StructureKV temp_kv;
			temp_kv.key = 0;
			temp_kv.value = string(music.singer, strlen(music.singer));
			music.stdata->all.push_back(temp_kv);
		}

	}

	//���
	if(pagetype != 18 && music.num_br > 8 && music.len > 0)
	{
		StructureKV temp_kv;
		temp_kv.key = 10;
		temp_kv.value = string(music.content, music.len);
		music.stdata->all.push_back(temp_kv);
	}

	return 0;
}

//���л�
int html_tree_extract_serial(StructureData *stdata, uint8_t *buf_ptr, uint32_t size)
{
	shared_ptr<TMemoryBuffer> mem_buf(new TMemoryBuffer);
	shared_ptr<TBinaryProtocol> bin_proto(new TBinaryProtocol(mem_buf));

	stdata->write(bin_proto.get());

	uint8_t* ptr;
	uint32_t len;
	mem_buf->getBuffer(&ptr, &len);

	if (len >= size)
		return -1;

	memcpy(buf_ptr, ptr, len);
	buf_ptr[len] = 0;

	return len;
}

//�����л���
int html_tree_extract_deserial(StructureData *stdata, uint8_t *buf_ptr, uint32_t sz)
{

	if(sz <= 0)
		return -1;
	shared_ptr<TMemoryBuffer> membuffer(new TMemoryBuffer());
	shared_ptr<TProtocol> protocol(new TBinaryProtocol(membuffer));

	membuffer->resetBuffer(buf_ptr, sz);
	stdata->read(protocol.get());
	return 0;
}

