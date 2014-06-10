#ifndef _EASOU_HTMLPS_H_
#define _EASOU_HTMLPS_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nodepool.h"
#include "easou_html_tree.h"
#include "easou_css_parser.h"
#include "easou_css_utils.h"
#include "easou_vhtml_inner.h"
#include "easou_vhtml_basic.h"
#include "easou_vstruct_profiler.h"

#define DEFAULT_TABLE_COL_NUM	128		  /**<   */
#define MAX_COLSPAN_NUM	128 /**<  */
#define MAX_AVAIL_SPACE_NUM  64		  /**<   */

#define PARSE_ERROR   -1		  /**<        */
#define PARSE_SUCCESS   1		  /**<       */
#define PARSE_TABLE_FAIL   2		  /**<        */

/**
 * @brief
 */
typedef struct easou_avail_space_t
{
	int x; /**<  */
	int y; /**<   */
	int width; /**<  */
} avail_space_t;

typedef struct _space_mgr_t
{
	avail_space_t space[MAX_AVAIL_SPACE_NUM]; /**< */
	int top; /**<   */
} space_mgr_t;

typedef struct _table_col_t
{
	int wx; /**<  */
	int wxlimit; /**<  */
	int span;
	int min_wx; /**< */
	int max_wx; /**<  */
} table_col_t;

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
int html_vtree_init();

/**
 * @brief ����V��
 * @author xunwu
 * @date 2011/06/27
 **/
html_vnode_t *construct_vtree(nodepool_t *np, html_node_t *node, int depth, int &id, html_vtree_t * vtree);

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
void html_vtree_get_html_property(html_vtree_t *html_vtree);

/**
 * @brief ʹ��css��Ⱦvtree
 * @author xunwu
 * @date 2011/06/27
 **/
void html_vtree_add_info_with_css(html_vtree_t *html_vtree, easou_css_pool_t *css_pool);

/**
 * @brief ʹ��css��Ⱦvtree���÷���ʹ��css��hash����������Ⱦ
 * @author sue
 * @date 2013/04/12
 */
void html_vtree_add_info_with_css2(html_vtree_t *html_vtree, easou_css_pool_t *css_pool);

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
void get_root_wx(html_vnode_t *root, int page_width);

/**
 * @brief ����V���ڵ�Ŀ��
 * @author xunwu
 * @date 2011/06/27
 **/
int html_vtree_compute_wx(html_vnode_t *vnode, table_col_t *default_col);

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
void layout_down_top(html_vnode_t *vnode, space_mgr_t *space_mgr, int depth = 0);

/**
 * @brief �������λ��
 * @author xunwu
 * @date 2011/06/27
 **/
void compute_absolute_pos(html_vnode_t *vnode);

/**
 * @brief �������
 * @author xunwu
 * @date 2011/06/27
 **/
void get_page_width(html_vnode_t *vnode);

/**
 * @brief �������塣
 *	vtree�﷨���ѽ��ã���������δ����λ�úʹ�С����.
 * @author xunwu
 * @date 2011/06/27
 **/
int html_vtree_parse_font(html_vtree_t *html_vtree);

/**
 * @brief �ǵݹ�ģ��������V��
 * @author xunwu
 * @date 2011/06/27
 **/
void trans_down_top(html_vnode_t *root, int page_width);

/**
 * @brief ��ȡ�ڵ��css����ֵ
 * @author xunwu
 * @date 2011/06/27
 **/
char *get_css_attribute(html_vnode_t *html_vnode, easou_css_prop_type_t type);

/**
 * @brief �ж�vnode���ӽڵ��Ƿ���ͬһ����
 * @author xunwu
 * @date 2011/06/27
 **/
bool is_child_in_aline(html_vnode_t *vnode);

#endif /* _EASOU_HTMLPS_H_ */
