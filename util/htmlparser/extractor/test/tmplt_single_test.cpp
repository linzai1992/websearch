/**
 * @brief ��ҳ���ȡdemo - ģ���ȡ
 * @author sue
 * @date 2013-06-12
 */

//�������ͷ�ļ�
#include "easou_extractor_template.h" //ģ���ȡ��Ҫ�����ͷ�ļ�
#include "easou_html_extractor.h"
#include "easou_html_attr.h"
#include "easou_debug.h"

//����ת��
#include "pagetranslate.h"

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define PAGE_SIZE (1<<20) //1M
int main(int argc, char** argv)
{
	timeinit();
	if (argc < 3)
	{
		printf("Usage: %s path url\n", argv[0]);
		exit(-1);
	}

	char *path = argv[1];
	char *url = argv[2];
	int url_len = strlen(url);

	char *page = (char*) malloc(PAGE_SIZE);
	assert(page != NULL);
	char* pageGBBuf = (char*) malloc(PAGE_SIZE * 2);
	assert(pageGBBuf);

	FILE *fp = fopen(path, "r");
	assert(fp != NULL);
	int page_len = fread(page, 1, PAGE_SIZE, fp);
	page[page_len] = 0;

	timestart();
	/**********************1. ��ʼ����������ģ�棬ȫ�ֿ�ʼ��ʼ��һ�Σ�templates_t�ɶ��߳�ʹ��******************/
	templates_t* dicts = create_templates("../conf");
//	templates_t* dicts = create_templates("extractor/conf"); //���߳�ʹ��templates_t

	if (dicts == NULL)
	{
		printf("create template dict error\n");
		exit(-1);
	}

	int try_count = 0;
	int load_ret = 0;
	while ((load_ret = load_templates(dicts)) != 0) //����mysql��ʼ��ģ��
	{
		if (++try_count >= 5) //��mysql���ظ�ʱ����ʼ����ʧ�ܣ�������Ҫ��˳���
		{
			printf("already try load templates 5 times, program will exist\n");
			exit(-1);
		}
		printf("load templates from mysql fail, try later...\n");
		sleep(1);
	}
	printf("template load success\n");
	/**********************1. ��ʼ����������ģ�棬ȫ�ֿ�ʼ��ʼ��һ�Σ�templates_t�ɶ��߳�ʹ��******************/
	timeend("extractor_news", "init");

	//�����page��Ҫת���GB18030����
	PageTranslator pt;
	int pageGBLen = pt.translate(page, page_len, pageGBBuf, PAGE_SIZE * 2);
	if (pageGBLen <= 0)
	{
		printf("page code conversion to utf fail\n");
		exit(-1);
	}
	pageGBBuf[pageGBLen] = 0;

	//����DOM����ģ���ȡ��������Ҫ�����õ�DOM��
	html_tree_t* tree = html_tree_create(MAX_PAGE_SIZE);
	html_tree_parse(tree, pageGBBuf, pageGBLen);

	/**********************2. �����������ģ���ȡ�������ֻ�ܵ��߳�ʹ��******************/
	tmplt_result_t* result = create_tmplt_result();
	/**********************2. �����������ģ���ȡ�������ֻ�ܵ��߳�ʹ��******************/

	timestart();
	/**********************3. ���ó�ȡ����******************/
	int extract_ret = try_template_extract(dicts, pageGBBuf, pageGBLen, url, url_len, tree, result);
	/**********************3. ���ó�ȡ����******************/
	timeend("extractor_news", "one_page");

	/**********************4. ʹ��ģ���ȡ�Ľ��******************/
	if (extract_ret != 0)
	{
		//�����ڶ�Ӧ��ģ��
		printf("there is no template for this url\n");
	}
	else if (result->extract_result[TMPLT_EXTRACT_REALTITLE].str_len == 0)
	{
		//���ڶ�Ӧ��ģ�壬����ȡ����title����Ϊ0��˵��ģ��Ը���ҳ������
		printf("template extract realtitle fail for this url\n");
	}
	else if (result->extract_result[TMPLT_EXTRACT_ARTICLE].str_len == 0)
	{
		//���ڶ�Ӧ��ģ�壬����ȡ����article����Ϊ0��˵��ģ��Ը���ҳ������
		printf("template extract article fail for this url\n");
	}
	else
	{
		//ģ���ȡ�ɹ�
		printf("extract success\n");
		//��ӡ�����ȡ�����iֵ�Ķ���ο�enum tmplt_extract_type_enum(��ȡ���Ͷ���)
		for (int i = 0; i < g_tmplt_extract_num; i++)
		{
			printf("%s %s\n", tmplt_extract_type_desp[i], result->extract_result[i].str ? result->extract_result[i].str : "NULL");
		}
		//��ӡ����ʱ��
		printf("year:%d mon:%d day:%d hour:%d min:%d sec:%d\n", result->pubtime_tm.tm_year, result->pubtime_tm.tm_mon, result->pubtime_tm.tm_yday, result->pubtime_tm.tm_hour, result->pubtime_tm.tm_min, result->pubtime_tm.tm_sec);
		//��ӡ����ͼƬ��Ϣ

		//����ȡ�����Ǿ���·����δ��http://
		html_node_list_t* list = result->extract_result[TMPLT_EXTRACT_ARTICLE].selected_imgs;
		link_t img_links[300];
		int img_num = 300;
		html_tree_extract_link(list, url, img_links, img_num);
		for (int i = 0; i < img_num; i++)
		{
			printf("img_url:http://%s\n", img_links[i].url);
		}

//����ȡ���п��������·��
//		while (list)
//		{
//			const char* src = get_attribute_value(&list->html_node->html_tag, "src");
//			if (src)
//				printf("%s\n", src ? src : "NULL");
//			list = list->next;
//		}
	}
	/**********************4. ʹ��ģ���ȡ�Ľ��******************/

	/**********************5. �ͷ�ģ���ȡ����Դ******************/
	del_tmplt_result(result); //ÿ���߳��ͷ�
	del_templates(dicts); //ȫ���ͷ�
	/**********************5. �ͷ�ģ���ȡ����Դ******************/
	printf("free templates dict success\n");

	free(page);
	free(pageGBBuf);
	html_tree_del(tree);
	printtime(1); // ��ӡʱ����Ϣ
	return 0;
}
