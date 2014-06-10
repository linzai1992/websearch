/**
 * easou_vhtml_tree.h
 * Description: V������
 *  Created on: 2011-11-13
 * Last modify: 2012-10-26 sue_zhang@staff.easou.com shuangwei_zhang@staff.easou.com
 *      Author: xunwu_chen@staff.easoucom
 *     Version: 1.2
 */
#ifndef EASOU_VHTML_TREE_H_
#define EASOU_VHTML_TREE_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nodepool.h"
#include "easou_html_tree.h"
#include "easou_css_parser.h"
#include "easou_css_utils.h"
#include "easou_vhtml_basic.h"
#include "easou_vhtml_parser.h"
#include "easou_vstruct_profiler.h"

/**
 * @brief	��������ʼ��Vtree����ṹ.
 * @author xunwu
 * @date 2011/06/27
 **/
vtree_in_t *vtree_in_create(int max_css_page_size = DEFAULT_MAX_CSS_PAGE_SIZE, int css_num = DEFAULT_CSS_NUM_INPOOL);

/**
 * @brief
 * @author xunwu
 * @date 2011/07/12
 **/
void vtree_in_destroy(vtree_in_t *vtree_in);

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
html_vtree_t *html_vtree_create_with_tree(html_tree_t *html_tree);

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
void html_vtree_clean(html_vtree_t *html_vtree);

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
void html_vtree_del(html_vtree_t *html_vtree);

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
void html_vtree_clean_with_tree(html_vtree_t *html_vtree);

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
void html_vtree_del_with_tree(html_vtree_t *html_vtree);

/**
 * @brief ����V���ڵ��λ�úͳ����Լ�һЩ������Ϣ����ʹ�õ������õ�css��Ϣ��
 * @author xunwu
 * @date 2011/06/27
 **/
int html_vtree_parse_with_css(html_vtree_t *html_vtree, easou_css_pool_t* css_pool, int page_width = DEFAULT_PAGE_WX);

/**
 * @brief ��DOM���Ļ����Ͻ�����V��
 * @param [in/out] html_vtree, �������������V��
 * @param [in/out] vtree_in, 
 * @param [in] page_width, ҳ��Ĭ�Ͽ��
 * @param [in] test_import, �Ƿ�����ⲿcss�ļ���import��css�ļ�
 * @param [in] useoutcss,true:ʹ������css��������css��false����ʹ������css��������css
 * @author xunwu
 * @date 2011/06/27
 * @last modify on 2012-10-26 sue_zhang@staff.easou.com
 **/
int html_vtree_parse_with_tree(html_vtree_t *html_vtree, vtree_in_t *vtree_in, const char *url, int page_width = DEFAULT_PAGE_WX, bool test_import = false,bool useoutcss=true);

/**
 * @brief ��ʼ��cssserver
 * @author shuangwei
 * @date 2012/08/01
 * @param
 * 	config_path, [in], edb�����ļ�·��
 * 	log_dir, [in], ��־���Ŀ¼
 * 	timeout, [in], ��ȡcss��ʱʱ�䣬Ĭ��10ms
 * 	thread_num, [in], ��ȡcss���̸߳�����Ĭ��20
 * 	cache_size, [in], css����������Ĭ��5W
 * @return
 * 	true����ʼ���ɹ�
 * 	false,��ʼ��ʧ��
 */
bool init_css_server(char *config_path, char *log_dir, int timeout = 10, int thread_num = 20, int cache_size = 35000);

/**
 * @brief �ͷ�css server�����Դ����һ��ȫ�ֱ���������ÿinitһ�μ�1��ÿfreeһ�μ�1����ֵΪ0ʱ���Ż�ȥ�����ͷš�
 * @author sue
 * @date 2013/12/16
 */
void free_css_server();

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
int html_vnode_visit(html_vnode_t *html_vnode, int (*start_visit)(html_vnode_t *, void *), int (*finish_visit)(html_vnode_t *, void *), void *result);

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
int html_vtree_visit(html_vtree_t *html_vtree, int (*start_visit)(html_vnode_t *, void *), int (*finish_visit)(html_vnode_t *, void *), void *result);

/**
 * ��λvtree_in_t�ṹ
 * @param [in] vtree_in:vtree_in_t *
 */
void vtree_in_reset(vtree_in_t *vtree_in);

/**
 *��λvhtml�����ָ���ʼ״̬
 */
void html_vtree_reset(html_vtree_t *html_vtree);

#endif /* EASOU_VHTML_TREE_H_ */
