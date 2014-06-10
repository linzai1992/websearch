/**
 * @brief ��ҳ���ȡdemo
 * @author sue
 * @date 2013-06-12
 */

//�������ͷ�ļ�
#include "easou_mark_parser.h"
#include "easou_html_extractor.h"
#include "easou_html_attr.h"
#include "easou_vhtml_tree.h"
#include "easou_vhtml_basic.h"
#include "easou_ahtml_tree.h"
#include "easou_mark_parser.h"
#include "easou_extractor_com.h"       //��ȡ�ֿ�����
#include "easou_extractor_title.h"     //��ȡ����
#include "easou_extractor_content.h"   //��ȡ����
#include "easou_extractor_mypos.h"
#include "easou_debug.h"
#include "easou_vhtml_utils.h"

#include "../../vhtmlparser/test/vtree_print_utils.h"
#include "../../test/area_visual_tool.h"
#include "../../test/tool_global.h"
//����ת��
#include "../../test/pagetranslate.h"

#include "mysql.h"

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#define PAGE_SIZE (1<<20) //1M
int main(int argc, char** argv)
{
	timeinit();
	if (argc < 2)
	{
		printf("Usage: %s url [path|mark]\n", argv[0]);
		exit(-1);
	}

	bool mark = false;
	char *path = NULL;
	if (argc == 3)
	{
		if (strcmp(argv[2], "mark") == 0)
			mark = true;
		else
			path = argv[2];
	}

	char *url = argv[1];
	int url_len = strlen(url);

	char *page = (char*) malloc(PAGE_SIZE);
	assert(page != NULL);

	//��ʼ��css������
	if (!init_css_server("./config.ini", "./log", 2000, 10))
	{
		printf("init css server fail\n");
		exit(-1);
	}
	else
		printf("init css server success\n");

	int page_len = 0;
	if (path)
	{
		FILE* fp = fopen(path, "r");
		assert(fp);
		page_len = fread(page, 1, PAGE_SIZE, fp);
		fclose(fp);
	}
	else
	{  //��mysql��ȡpage
		MYSQL* conn = connect_mysql();
		page_len = get_page_from_mysql(conn, url, page, PAGE_SIZE);
		if (page_len <= 0)
		{
			printf("get page fail %s, not exist in mysql\n", url);
			mysql_close(conn);
			return -1;
		}
		mysql_close(conn);
	}
	page[page_len] = 0;

	dva_trees_t* dvm_trees = create_dva_trees("../../vhtmlparser/conf");
	assert(dvm_trees);

	for (int i = 0; i < 5; i++)
	{
		timestart();
		reset_dvm_trees(dvm_trees);
		if (!parse_dvm_trees(dvm_trees, url, url_len, page, page_len))
		{
			printf("parse tree fail, url:%s\n", url);
			timeend("parse", "");
			return false;
		} timeend("parse", "");
	}

	//�ӱ�����ϳ�ȡ��ʵ����
	timestart();
	char realtitle[2048];
	int len = html_atree_extract_realtitle(dvm_trees->atree, realtitle, 2048);
	timeend("html_atree_extract_realtitle", "");  //��ӡʱ�������Ϣ
	printf("[html_atree_extract_realtitle] - len:%d\n%s\n", len, realtitle);

	//��dom���ϳ�ȡtagtitle
	timestart();
	char tagtitle[2048];
	int tagtitle_len = html_tree_extract_tagtitle(dvm_trees->tree, tagtitle, 2048);
	timeend("html_tree_extract_tagtitle", "");
	printf("[html_tree_extract_tagtitle] - len:%d\n%s\n", tagtitle_len, tagtitle);

	//�����Ż����Եı����ȡ
	timestart();
	char realtitle2[2048];
	realtitle_input_t rt_input;
	rt_input.atree = dvm_trees->atree;
	rt_input.url = url;
	rt_input.url_len = url_len;
	rt_input.tagtitle = tagtitle;
	rt_input.tagtitle_len = tagtitle_len;
	int rt_len2 = easou_extract_realtitle(&rt_input, realtitle2, 2048);
	timeend("easou_extract_realtitle", "");
	printf("[easou_extract_realtitle] - len:%d\n%s\n", rt_len2, realtitle2);

	int extract_buf_size = 128000;
	char *extract_buf = (char*) malloc (extract_buf_size);
	assert(extract_buf);
	int extract_len;

	//��������ֳ�ȡ
#ifdef MAIN_CONTENT
	extract_len = html_atree_extract_main_content(dvm_trees->atree, extract_buf, extract_buf_size, url);
	printf("[html_atree_extract_main_content] - len:%d\n%s\n", extract_len, extract_buf);

	if (mark)
	{ //�ѳ�ȡ������浽���ݿ��У����������Ϊ��ע����ʹ��
		if (SUCCESS == mark_maincont(url, extract_buf))
			printf("mark main content success\n");
		else
			printf("mark main content fail\n");
	}
#endif

	//���������ֳ�ȡ
#ifdef NAVIGATION
	extract_len = get_all_area_content(extract_buf, extract_buf_size, dvm_trees->atree, AREA_FUNC_NAV);
	printf("[get_all_area_content AREA_FUNC_NAV] - len:%d\n%s\n", extract_len, extract_buf);

	if (mark)
	{
		if (SUCCESS == mark_navigation(url, extract_buf))
			printf("mark navigation success\n");
		else
			printf("mark navigation fail\n");
	}
#endif

#ifdef SUBTITLE
	extract_len = get_all_area_content(extract_buf, extract_buf_size, dvm_trees->atree, AREA_FUNC_SUBTITLE);
	printf("[get_all_area_content AREA_FUNC_SUBTITLE] - len:%d\n%s\n", extract_len, extract_buf);

	if (mark)
	{
		if (SUCCESS == mark_subtitle(url, extract_buf))
			printf("mark subtitle success\n");
		else
			printf("mark subtitle fail\n");
	}
#endif

	//mypos��ȡ
#ifdef MYPOS
	mypos_t tmp;
	memset(&tmp, 0, sizeof(mypos_t));
	extract_len = html_atree_extract_mypos(&tmp, dvm_trees->atree, url);
	printf("[html_atree_extract_mypos] - len:%d\n%s\n", extract_len, tmp.text);

	if (mark)
	{
		if (SUCCESS == mark_mypos(url, extract_buf))
			printf("mark mypos success\n");
		else
			printf("mark mypos fail\n");
	}
#endif

	free(extract_buf);

	//��ӡMARK����html��
	char* buf = (char*) malloc(1 << 25);
	assert(buf);
	//��ӡÿһ��ֿ�
	for (int i = 0; i < dvm_trees->atree->max_depth; i++)
	{
		printAreaTreeHtml(dvm_trees->atree, buf, 1 << 25, 1, i);
		char name[256];
		int len = sprintf(name, "atree_%d.html", i);
		name[len] = 0;
		FILE *fp2 = fopen(name, "w");
		fwrite(buf, 1, strlen(buf), fp2);
		fclose(fp2);
	}
	//��ӡ�����
	printSemHtml(dvm_trees->atree, buf, 1 << 25, AREA_SEM_CENTRAL);
	FILE *fpCentral = fopen("central.html", "w");
	fwrite(buf, 1, strlen(buf), fpCentral);
	fclose(fpCentral);

	//��ӡV����Ϣ��html��
	int avail = 0;
	vhtml_print_info(dvm_trees->vtree->body, buf, 1 << 25, avail);
	FILE *fpVtree = fopen("vtree.html", "w");
	fwrite(buf, 1, strlen(buf), fpVtree);
	fclose(fpVtree);

	free(buf);
	free(page);
	destroy_dvm_trees(dvm_trees);
	printtime(1);  // ��ӡʱ����Ϣ
	return 0;
}
