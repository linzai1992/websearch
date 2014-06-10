/**
 * easou_css_utils.h
 * Description: CSS��������ӿ�
 *  Created on: 2011-06-20
 * Last modify: 2012-10-31 sue_zhang@staff.easou.com shuangwei_zhang@staff.easou.com
 *      Author: xunwu_chen@staff.easoucom
 *     Version: 1.2
 */
#ifndef EASOU_CSS_UTILS_H_
#define EASOU_CSS_UTILS_H_

#include "easou_css_pool.h"
#include "easou_html_tree.h"

/**
 * @brief ����ҳ��css������
 */
typedef struct _easou_page_css_t
{
	short style_txt_num; /**< style css�ı�������  */
	const char *style_txt[MAX_CSS_NUM_IN_POOL]; /**< css style �ı�  */
	char css_url[MAX_CSS_NUM_IN_POOL][MAX_URL_SIZE]; /**< �����ⲿcss��Ӧ��url */
} easou_page_css_t;

/**
 * @brief CSS��������.
 */
typedef struct _easou_css_env_t
{
	easou_page_css_t page_css; /**< ҳ����css     */
	easou_css_pool_t css_pool; /**< CSS�Ľ������      */
} easou_css_env_t;

/**
 * @brief ҳ���css��Ϣ����Ҫ���ڲ�ʹ��
 */
typedef struct _easou_cssinfo_keep_t
{
	const char *url; /**< ҳ��URL  */
	bool is_skip_child; /**< �Ƿ�������ڵ�  */
	easou_page_css_t *page_css; /**ҳ���е�css��Ϣ*/
} easou_cssinfo_keep_t;

/**
 * @brief �жϸýڵ��Լ��ýڵ㵽�ӽڵ��Ƿ�֧��screen_media
 * @param [in]html_tag: html_tag_t *  �ڵ������.
 * @author xunwu
 * @date 2011/06/20
 **/
bool is_apply_for_screen_media(html_tag_t *html_tag);

/**
 * @brief CSS�����Ĵ���.
 * @param [in] max_css_page_size   : int ���css
 * @param [in] css_num   : int	������css������.
 * @return  css_env_t*	css��������.
 * @author xunwu
 * @date 2011/06/20
 **/
easou_css_env_t *css_env_create(int max_css_page_size, int css_num);

/**
 * @brief ����css��������.
 * @author xunwu
 * @date 2011/06/20
 **/
void css_env_destroy(easou_css_env_t *env);

/**
 * @brief	��ҳ���л�ȡcss��Ϣ
 * @param [out] page_css   : page_css_t*	ҳ���е�css��Ϣ
 * @param [in] html_tree   : const html_tree_t*	�����õ�dom��
 * @param [in] url   : const char*	ҳ���url
 * @author xunwu
 * @date 2011/06/20
 **/
void get_page_css_info(easou_page_css_t *page_css, const html_tree_t *html_tree, const char *url);

/**
 * @brief ����ҳ���е�css
 * @param [in] test_import, �Ƿ����css�ļ���import��css
 * @author xunwu
 * @date 2011/06/20
 * @last modify on 2012-10-26 sue_zhang@staff.easou.com
 **/
void parse_internal_css(easou_css_env_t *css_env, easou_page_css_t *page_css, const char *url, bool test_import = false);

/**
 * @brief	��ȡ������ҳ���е�css.
 * @param [out] css_env   : easou_css_env_t*	css��������.
 * @param [in] html_tree   : html_tree_t*	��������html��.
 * @param [in] url   : const char*	ҳ��URL.
 * @param [in] test_import, �Ƿ����css�ļ���import��css
 * @author xunwu
 * @date 2011/06/20
 * @last modify on 2012-10-26 sue_zhang@staff.easou.com
 **/
int get_parse_css_inpage(easou_css_env_t *css_env, const html_tree_t *html_tree, const char *url, bool test_import = false);

/**
 * @brief ���Խӿڣ���ӡ�����õ�css��Ϣ���ļ���
 * @param [in] csspool	:	�����õ�css���
 * @param [in] filename	:	������ļ�����
 * @author sue
 * @date 2012/04/09
 */
void csspool_print_selector(const easou_css_pool_t *csspool, const char* filename);

/**
 * @brief ��λcss��������
 * @param [in] cc:easou_css_env_t * css����
 */
void css_env_reset(easou_css_env_t *cc);

/**
 * @brief ����ⲿcss
 * @param [in/out]easou_page_css_t *css_keep css����
 * @param [in] ptxt �ⲿcss�ı���ptxt�ռ��ڽ������֮ǰ�����ͷ�
 * @param [in] css_url, �ⲿcss��url
 * @last modify on 2012-10-26 sue_zhang@staff.easou.com
 */
void add_out_style_text(easou_page_css_t *css_keep, char *ptxt, char *css_url);

bool is_css_link_tag(html_tag_t *html_tag);

#endif /*EASOU_CSS_UTILS_H_*/
