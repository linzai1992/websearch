/**
 * @file ahtml_tree.h

 * @date 2011/06/27
 * @version 1.0 
 * @brief ��vtree�Ļ����Ͻ��зֿ����
 *  
 **/

#ifndef  __EASOU_AHTML_TREE_H
#define  __EASOU_AHTML_TREE_H

#include "util/htmlparser/ahtmlparser/ahtml_area.h"

typedef struct _atree_baseinfo_t atree_baseinfo_t;

/**
 * @brief Ԥ������
 */
typedef struct _mark_area_info_t	mark_area_info_t;
typedef struct _area_baseinfo_mgr_t	area_baseinfo_mgr_t;
typedef struct _area_uid_binfo_mgr_t	area_uid_binfo_mgr_t;
typedef struct _function_switch_t	function_switch_t;
typedef struct _area_tree_internal_info_t area_tree_internal_info_t;
/**
 * @brief �ֿ����Ľṹ��
 */
typedef struct _area_tree_t {
	html_vtree_t *hp_vtree;       /**< �ֿ�����Ӧ��vtree  */
	html_area_t *root;	  		  /**< �ֿ����ĸ��ڵ�  */
	unsigned int area_num;	      /**< �ֿ�ڵ�����    */
	nodepool_t np;		          /**< �ֿ����Ľڵ��  */
	area_config_t config;	      /**< �ֿ���������    */
	unsigned int mark_status;     /**< ���ı�ע״̬    */
//	area_uid_binfo_mgr_t *area_uid_binfo_mgr;/**< */
	area_baseinfo_mgr_t * area_binfo_mgr ;   /**< ��ʼ��ʱΪ�գ���Ҫʱ���أ��ڲ��� */
	mark_area_info_t * mark_info ;           /**< ��ʼ��ʱΪ�գ���Ҫʱ����, �洢��ע�����Ϣ */
	function_switch_t *function_switch;		 /**< ���ܿ��� */
	area_tree_internal_info_t *internal_info;/**�ڲ��õ�mark��Ϣ*/
	atree_baseinfo_t *base_info;
	int max_depth;//tree max depth
}area_tree_t;


/**
 * @brief	����һ�÷ֿ���.
 * @param [in] cfg   : area_config_t*	�������÷ֿ����ķֿ�����.��ΪNULL.
 * @return  area_tree_t*	�ѷ���ռ�ķֿ���.

 * @date 2011/07/05
 **/
area_tree_t *area_tree_create(area_config_t *cfg);

/**
 * @brief ���÷ֿ����ķֿ�����.

 * @date 2011/07/05
 **/
void area_tree_setConfig(area_tree_t *atree,area_config_t *cfg);

/**
 * @brief ɾ���ֿ���.

 * @date 2011/07/05
 **/
void area_tree_del(area_tree_t *atree);

/**
 * @brief ���һ�÷ֿ����ϵķֿ���Ϣ.

 * @date 2011/07/05
 **/
void area_tree_clean(area_tree_t *atree);

/**
 * @brief ��VTREE�ϻ��ַֿ���.
 * @param [out] atree   : area_tree_t*	�ֿ���.
 * @param [in] vtree   : const html_vtree_t*	�����ֵ�VTREE.
 * @param [in] base_url : ��ҳԭʼurl
 * @return  int 
 * @retval  -1:�ֿ����;1:�ɹ�.

 * @date 2011/07/05
 **/
int area_partition(area_tree_t *atree, html_vtree_t *vtree, const char *base_url);

/**
 * @brief ����һ���ֿ�.
 * @param [in] area   : html_area_t*	�����ֵķֿ�.
 * @param [in] cfg   : const area_config_t*	�ֿ����������.
 * @param [in/out] np   : nodepool_t*	�ֿ�ڵ��.
 * @param [in] depth   : unsigned int	�����ֵķֿ�����.`
 * @return  int 
 * @retval   -1:�ֿ����.1:�ɹ�.

 * @date 2011/07/05
 **/
int areaNode_divide(html_area_t *area,const area_config_t *cfg,nodepool_t *np, unsigned int depth);

/*����������������������*/
typedef int (* FUNC_START_T)(html_area_t * ,void * ) ;
/*�����ĺ���������������*/
typedef int (* FUNC_FINISH_T)(html_area_t * ,void * ) ;

#define AREA_VISIT_ERR	(-1)
#define AREA_VISIT_NORMAL  1
#define AREA_VISIT_FINISH  2
#define AREA_VISIT_SKIP 3

/**
 * @brief �ֿ����ı���������ͬ html_tree_visit ����
 * @param [in/out] atree   : area_tree_t*
 * @param [in/out] start   : FUNC_START_T
 * @param [in/out] finish   : FUNC_FINISH_T
 * @param [in/out] data   : void*
 * @return  bool 
 * @retval   �ɹ� true ��ʧ��false

 * @date 2011/07/05
 **/
bool areatree_visit(area_tree_t * atree, FUNC_START_T start,  FUNC_FINISH_T finish, void * data ) ;

/**
 * @brief ���غ�����������˵��
 * @param [in/out] arearoot   : html_area_t*
 * @param [in/out] start   : FUNC_START_T
 * @param [in/out] finish   : FUNC_FINISH_T
 * @param [in/out] data   : void*
 * @return  int 
 * @retval   ʧ��0 ���ɹ�>0

 * @date 2011/07/05
 **/
int areatree_visit(html_area_t * arearoot, FUNC_START_T start,  FUNC_FINISH_T finish, void * data);

/**
 * print area tree
 */
void printAtree(area_tree_t * atree);

void printArea(html_area_t * area,int level);

void printSingleArea(html_area_t * area);
#endif  //__EASOU_AHTML_TREE_H

