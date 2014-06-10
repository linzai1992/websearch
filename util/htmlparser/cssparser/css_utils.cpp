/**
 * css_utils.cpp
 * Description: cssparser����ӿ�
 *  Created on: 2011-06-20
 * Last modify: 2012-10-26 sue_zhang@staff.easou.com shuangwei_zhang@staff.easou.com
 *      Author: xunwu_chen@staff.easoucom
 *     Version: 1.2
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "easou_html_pool.h"
#include "easou_html_attr.h"
#include "easou_html_tree.h"
#include "easou_css_pool.h"
#include "easou_css_parser.h"
#include "easou_css_utils.h"
#include "easou_debug.h"
#include "easou_string.h"

using namespace EA_COMMON;

/**
 * @brief ����css����
 * @author xunwu
 * @date 2011/06/20
 **/
void css_env_destroy(easou_css_env_t *cc)
{
	if (cc)
	{
		css_pool_destroy(&cc->css_pool);
		free(cc);
	}
}

/**
 * @brief CSS�����Ĵ���.
 * @param [in] max_css_page_size   : int ���css
 * @param [in] css_num   : int	������css������.
 * @return  css_env_t*	css��������.
 * @author xunwu
 * @date 2011/06/20
 **/
easou_css_env_t *css_env_create(int max_css_page_size, int css_num)
{
	easou_css_env_t *cc = (easou_css_env_t *) calloc(1, sizeof(easou_css_env_t));
	if (NULL == cc)
	{
		Fatal((char*) "%s:%d:alloc error!", __FILE__, __LINE__);
		goto ERR;
	}
	if (css_pool_init(&cc->css_pool, max_css_page_size, css_num) == -1)
	{
		goto ERR;
	}
	return cc;
	ERR: css_env_destroy(cc);
	return NULL;
}

/**
 * @brief ���css��Ϣ
 */
static void cssinfo_keep_clean(easou_cssinfo_keep_t *css_keep)
{
	css_keep->url = NULL;
	css_keep->is_skip_child = false;
}

bool is_apply_for_screen_media_list(const char *pvalue)
{
	if (pvalue == NULL || pvalue[0] == '\0')
	{
		return true;
	}
	const char *nextp = NULL;
	for (const char *p = pvalue; *p; p = nextp)
	{
		p = easou_skip_space(p);
		const char *sep = strchr(p, ',');
		int l = 0;
		if (sep != NULL)
		{
			l = sep - p;
			nextp = p + l + 1;
		}
		else
		{
			l = strlen(p);
			nextp = p + l;
		}
		if (strncasecmp(p, "all", strlen("all")) == 0)
		{
			if (is_only_space_between(p + strlen("all"), p + l))
			{
				return true;
			}
		}
		else if (strncasecmp(p, "screen", strlen("screen")) == 0)
		{
			if (is_only_space_between(p + strlen("screen"), p + l))
			{
				return true;
			}
		}
	}
	return false;
}

/**
 * @brief �жϸýڵ��Ƿ��Ӱ��screen
 * @author xunwu
 * @date 2011/06/20
 **/
bool is_apply_for_screen_media(html_tag_t *html_tag)
{
	assert(html_tag->tag_type == TAG_LINK || html_tag->tag_type == TAG_STYLE);
	const char *pvalue = get_attribute_value(html_tag, ATTR_MEDIA);
	return is_apply_for_screen_media_list(pvalue);
}

/**
 * @brief �жϱ�ǩ�Ƿ���css link
 * @author xunwu
 * @date 2011/06/20
 **/
bool is_css_link_tag(html_tag_t *html_tag)
{
	/**������link*/
	if (html_tag->tag_type != TAG_LINK)
	{
		return false;
	}
	/**ATTR_REL���Ա�����stylesheet*/
	const char *pvalue = get_attribute_value(html_tag, ATTR_REL);
	if (pvalue != NULL && strcasecmp(pvalue, "stylesheet") == 0 && is_apply_for_screen_media(html_tag))
	{
		return true;
	}
	return false;
}

static void get_style_text(easou_page_css_t *css_keep, html_tag_t *html_tag, const char *url)
{
	if (!is_apply_for_screen_media(html_tag))
	{
		return;
	}
	char *ptxt = html_tag->text;
	if (ptxt && ptxt[0] != '\0')
	{
		if ((unsigned) css_keep->style_txt_num < sizeof(css_keep->style_txt) / sizeof(char *))
		{
			css_keep->style_txt[css_keep->style_txt_num++] = ptxt;
		}
		else
		{
//			Warn("%s:too many style text:%s!", __FUNCTION__, url);
		}
		/**����style��import������css��������*/
		//	collect_import_css_url(css_keep, ptxt, url);
	}
}

static int start_visit_for_cssinfo(html_tag_t *html_tag, void *result, int flag)
{
	easou_cssinfo_keep_t *css_keep = (easou_cssinfo_keep_t *) result;
	if (html_tag->tag_type == TAG_STYLE)
	{
		get_style_text(css_keep->page_css, html_tag, css_keep->url);
	}
	return VISIT_NORMAL;
}

/**
 * @brief ��ȡҳ���е�css.
 * @see
 * @author xunwu
 * @date 2011/06/20
 **/
static void get_cssinfo(easou_cssinfo_keep_t *css_keep, const html_tree_t *html_tree, const char *url)
{
	css_keep->url = url;
	html_tree_visit((html_tree_t *) html_tree, &start_visit_for_cssinfo, NULL, css_keep, 0);
}

/**
 * @brief	��ҳ���л�ȡcss��Ϣ
 * @param [out] page_css   : page_css_t*	ҳ���е�css��Ϣ
 * @param [in] html_tree   : const html_tree_t*	�����õ�dom��
 * @param [in] url   : const char*	ҳ���url
 * @author xunwu
 * @date 2011/06/20
 **/
void get_page_css_info(easou_page_css_t *page_css, const html_tree_t *html_tree, const char *url)
{
	easou_cssinfo_keep_t css_keep;
	css_keep.page_css = page_css;
	cssinfo_keep_clean(&css_keep);
	get_cssinfo(&css_keep, html_tree, url);
}

/**
 * @brief ����ҳ���е�css
 * @author xunwu
 * @date 2011/06/20
 **/
void parse_internal_css(easou_css_env_t *css_env, easou_page_css_t *page_css, const char *page_url, bool test_import)
{
	easou_css_pool_t *css_pool = &css_env->css_pool;
	css_pool_clean(css_pool);
	int order = css_pool->used_css_num;
	int success = 0;
	int fail = 0;
	int select_id = 0;
	for (int i = 0; i < page_css->style_txt_num; i++)
	{
		if (css_pool->used_css_num < css_pool->alloc_css_num)
		{
			int ret = css_parse(css_pool->css_array[css_pool->used_css_num], page_css->style_txt[i], css_env->page_css.css_url[css_pool->used_css_num], false, test_import);
			if (ret < 0)
			{
//				Warn("%s:parse css error:%s.", __FUNCTION__, page_url);
				fail++;
			}
			else
			{
				css_pool->order[css_pool->used_css_num] = order++;

				for (easou_css_ruleset_t* ruleset = css_pool->css_array[css_pool->used_css_num]->all_ruleset_list; ruleset != NULL; ruleset = ruleset->next)
					ruleset->id = select_id++;

				css_create_hash_index(css_pool->css_array[css_pool->used_css_num], css_pool->hm);

				css_pool->used_css_num++;
				success++;
			}
		}
		else
		{
//			Warn("%s:css pool full:%s!", __FUNCTION__, page_url);
			break;
		}
	}
	css_pool_sort(css_pool);

	counter_add("css_selector_num", select_id); record_max("css_selector_max", (unsigned int)select_id);
}

/**
 * @brief	��ȡ������ҳ���е�css.
 * @param [out] css_env   : easou_css_env_t*	css��������.
 * @param [in] html_tree   : html_tree_t*	��������html��.
 * @param [in] url   : const char*	ҳ��URL.
 * @author xunwu
 * @date 2011/06/20
 **/
int get_parse_css_inpage(easou_css_env_t *css_env, const html_tree_t *html_tree, const char *url, bool test_import)
{
	get_page_css_info(&(css_env->page_css), html_tree, url);
	parse_internal_css(css_env, &(css_env->page_css), url, test_import);
#ifdef DEBUG_INFO
	csspool_print_selector(&css_env->css_pool, "css_selector.txt");
#endif
	return css_env->page_css.style_txt_num;
}

void csspool_print_selector(const easou_css_pool_t *csspool, const char* filename)
{
	if (csspool->used_css_num <= 0)
		return;
	FILE *fp = fopen(filename, "w");
	if (fp == NULL)
		return;

	fprintf(fp, "csspool->used_css_num\t%d\n\n", csspool->used_css_num);
	for (int i = 0; i < csspool->used_css_num; i++)
		print_css(csspool->css_array[i], fp);
	fclose(fp);
}

/**
 * ��λcss����
 */
void css_env_reset(easou_css_env_t *cc)
{
	if (cc)
	{
		css_pool_clean(&cc->css_pool);
		cc->page_css.style_txt_num = 0;
		memset(cc->page_css.style_txt, 0, sizeof(cc->page_css.style_txt));
	}
}

void add_out_style_text(easou_page_css_t *css_keep, char * ptxt, char *css_url)
{
	if (css_keep == NULL)
	{
		return;
	}
	if (ptxt && ptxt[0] != '\0')
	{
		if ((unsigned) css_keep->style_txt_num < sizeof(css_keep->style_txt) / sizeof(char *))
		{
			css_keep->style_txt[css_keep->style_txt_num] = ptxt;
			memcpy(css_keep->css_url[css_keep->style_txt_num], css_url, strlen(css_url));
			css_keep->style_txt_num++;
		}
		else
		{
//			Warn("%s:too many style!", __FUNCTION__);
		}
		/**����style��import������css��������*/
//		collect_import_css_url(css_keep, ptxt, url);
	}
}

