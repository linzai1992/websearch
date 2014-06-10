/*
 * easou_mark_parser.cpp
 *
 *  Created on: 2011-11-22
 *      Author: ddt
 */
#include "easou_html_extractor.h"
#include "easou_mark_conf.h"
#include "easou_mark_inner.h"
#include "easou_mark_baseinfo.h"
#include "easou_mark_innerinfo.h"
#include "easou_mark_com.h"
#include "easou_mark_pos.h"
#include "easou_mark_srctype.h"
#include "easou_mark_func.h"
#include "easou_mark_sem.h"
#include "easou_mark_parser.h"
#include "nodepool.h"


/**
 * @brief ����atree����Ϣ��mark��׼��
 **/
bool preparation_mark(area_tree_t * atree){

	//shuangwei modify for reset,only init at first
//	if((atree->function_switch = function_switch_create()) == NULL){
//		goto FAIL;
//	}
//	atree->internal_info = area_tree_internal_info_create();
//	if(atree->internal_info == NULL){
//		goto FAIL;
//	}
	if(atree->function_switch==NULL){
		atree->function_switch = function_switch_create();
	}
	if(atree->function_switch  == NULL){
		goto FAIL;
	}

	if(NULL==atree->internal_info){
		atree->internal_info = area_tree_internal_info_create();
	}
 // shuangwei modify over 20120331
	if(atree->internal_info == NULL){
		goto FAIL;
	}
	if(-1 == init_mark_type_handler(atree)){
		goto FAIL;
	}
	return true;
FAIL:
	if(atree->function_switch){
		function_switch_destroy(atree->function_switch);
		atree->function_switch = NULL;
	}
	if(atree->internal_info){
		area_tree_internal_info_destroy(atree->internal_info);
		atree->internal_info = NULL;
	}
	return false;
}

/**
 * @brief ����atree�е�mark_info����
 * @param [in/out] atree   : area_tree_t* �������atree
 * @param [in] page_url   : const char *  ҳ��url
 * @param [in] in   : vmark_input_t* ��עǰ���û����룬��Ҫ����ָ������ҳ��ê��
 * @return  bool
 * @retval   �ɹ� true ��ʧ��false
 * @see
 * @author xunwu
 * @date 2011/07/05
 **/
static bool add_mark_info(area_tree_t * atree, const char * page_url, vmark_input_t *in)
{
	assert(atree !=NULL && atree->root !=NULL );
	if(atree->mark_info == NULL){
		atree->mark_info = mark_info_create();
		if(atree->mark_info == NULL){
			goto FAIL;
		}
	}
	atree->mark_info->area_tree = atree ;
	//todo
	snprintf(atree->mark_info->page_url, MAX_URL_LEN, "%s", page_url) ;
	ef_trans2pt(page_url,atree->mark_info->page_url_pat);
	snprintf(atree->mark_info->base_href, MAX_URL_LEN, "%s", page_url);
	get_base_url(atree->mark_info->base_href, atree->hp_vtree->hpTree);

	if(in != NULL){
		if(in->anchor != NULL){
			snprintf(atree->mark_info->anchor,sizeof(atree->mark_info->anchor), "%s",in->anchor);
		}
	}
	html_tree_extract_title(atree->hp_vtree->hpTree,atree->mark_info->tag_title, sizeof(atree->mark_info->tag_title));
	return true ;

FAIL:
	if(atree->mark_info != NULL){
		free(atree->mark_info) ;
		atree->mark_info = NULL ;
	}
	return false ;
}

/**
 * @brief ��ATREE�ֿ���б�ע.
 * 	������shut_mark_type()�ӿڶ�ĳЩ��ע���ͽ��йر�.
 * @param [in/out] atree   : area_tree_t*	����ע��atree.
 * @param [in] url   : const char*	ҳ��URL ��
 * @param [in] vmark_in   : vmark_input_t*	��ע�����.
 * @return  bool	�Ƿ�ɹ�.
 * @author xunwu
 * @date 2011/07/07
**/
bool mark_area_tree(area_tree_t * atree , const char *url, vmark_input_t *vmark_in)
{
	if(atree == NULL ){
		return false ;
	}
	if (atree->base_info == NULL)
	{
		atree_baseinfo_t *base_info = (atree_baseinfo_t*) malloc(sizeof(atree_baseinfo_t));
		if (base_info == NULL)
		{
			return false;
		}
		base_info->max_text_area_no = 0;
		base_info->max_text_leaf_area_no = 0;
		base_info->max_text_rate = 0;
		base_info->max_text_rate_leaf = 0;
		atree->base_info = base_info;
	}
	assert(atree->mark_status == 0);
	/**����atree����Ϣ��mark��׼��*/
	if(!preparation_mark(atree)){
		return false;
	}
	/**��ʼ��markinfo*/
	add_mark_info(atree, url, vmark_in);
	/**�ж϶�base_info�ı�ע�Ƿ�ر�*/
	if(is_shutted_type(atree, MARK_TYPE_BASE_INFO)){
//		Debug("%s:Base info computing is shutted!",__FUNCTION__);
	}
	else{
		/**��עbaseinfo�������ⲿ��Ϣ��������Ϣ��ͼƬ��Ϣ��������Ϣ���ı���Ϣ*/
		if(!fill_base_info( atree )){
			return false ;
		}
	}
	/**�ж϶�pos���Եı�ע�Ƿ�ر�*/
	if(is_shutted_type(atree, MARK_TYPE_POS)){
//		Debug("%s:mark area position is shutted!",__FUNCTION__);
	}
	else{
		/*markÿ���ֿ�ڵ�����ڸ��ֿ�����λ��*/
		area_markPos(atree);
		/**markÿ���ֿ������ҳ���λ��*/
		if(!mark_pos_area(atree)){
			return false ;
		}
	}
	/**��עÿ���ֿ����Դ����*/
	if(!mark_srctype_area(atree)){
		return false ;
	}
	/**��עÿ���ֿ�Ĺ�������*/
	if(!mark_func_area(atree)){
		return false ;
	}
	/**��עÿ���ֿ����������*/
	if(!mark_sem_area(atree)){
		return false ;
	}
	bool isok= get_mark_result(atree) ;
	for(int i=0;i<atree->internal_info->n_func_marker;i++){
		atree->internal_info->marked_func[i]=HAS_BUILD_RESULT_LIST;
	}
	for(int i=0;i<atree->internal_info->n_sem_marker;i++){
			atree->internal_info->marked_sem[i]=HAS_BUILD_RESULT_LIST;
	}
	for(int i=0;i<atree->internal_info->n_srctype_marker;i++){
			atree->internal_info->marked_srctype[i]=HAS_BUILD_RESULT_LIST;
	}
	return isok ;
}

/**
 * @brief �ͷ�mark tree�Ŀռ�
**/
void mark_tree_destory(area_tree_t * atree){
	if(atree){
		if(atree->function_switch){
			function_switch_destroy(atree->function_switch);
			atree->function_switch = NULL;
		}
		if(atree->internal_info){
			area_tree_internal_info_destroy(atree->internal_info);
			atree->internal_info = NULL;
		}
		if(atree->mark_info != NULL){
			mark_info_destroy(atree->mark_info);
			atree->mark_info = NULL ;
		}
		if(atree->area_binfo_mgr != NULL){
			area_baseinfo_mgr_des(atree->area_binfo_mgr);
			atree->area_binfo_mgr = NULL;
		}
		if(atree->base_info)
		{
			free(atree->base_info);
			atree->base_info = NULL;
		}
		area_tree_del(atree);
		atree = NULL;
	}
}

/**
 * @brief �ͷ�mark tree�Ŀռ�
**/
void mark_tree_clean(area_tree_t * atree){
	if(atree){
		if(atree->function_switch){
			function_switch_destroy(atree->function_switch);
			atree->function_switch = NULL;
		}
		if(atree->internal_info){
			area_tree_internal_info_destroy(atree->internal_info);
			atree->internal_info = NULL;
		}
		if(atree->mark_info != NULL){
			mark_info_destroy(atree->mark_info);
			atree->mark_info = NULL ;
		}
		if(atree->area_binfo_mgr != NULL){
			area_baseinfo_mgr_des(atree->area_binfo_mgr);
			atree->area_binfo_mgr = NULL;
		}
	}
}

/**
 * @brief ���ֿ���ݱ�ע���͹ҵ���Ӧ���б���.
 * @return  int
 * @retval   -1:����.1:�ɹ�.
 * @see
 * @author xunwu
 * @date 2011/07/07
**/
static int add_area_to_list(area_list_t area_list_arr[], unsigned int list_arr_size,
		html_area_t *area, unsigned int mark, nodepool_t *area_pool)
{
	assert(list_arr_size <= sizeof(unsigned int)*8);

	for(unsigned int i = 1; i < list_arr_size; i++){
		/**
		 * �ҵ�Ϊ1��λ,�����ѱ���˶�Ӧ�ı�ע����.
		 */
		if(mark & (0x1)){
			area_list_node_t * area_node = (area_list_node_t *)nodepool_get(area_pool) ;
			if(area_node == NULL ){
//				Warn("%s : can't nodepool_get" , __FUNCTION__) ;
				return -1;
			}
			area_node->area = area ;
			area_node->next = NULL ;
			insert_area_list(&area_list_arr[i],area_node);
		}
		mark >>= 1;
	}
	return 1;
}

static int start_visit_for_mark_info(html_area_t * area ,void * data )
{
	assert(area !=NULL  && data!=NULL ) ;
	mark_area_info_t * mark_info = (mark_area_info_t * )data ;
	if(area->isValid == false )
		return AREA_VISIT_SKIP ;

	if(!IS_VOID_AREA_MARK(area->srctype_mark)){
		int ret =
			add_area_to_list(mark_info->srctype, sizeof(mark_info->srctype) / sizeof(area_list_t),
					area, area->srctype_mark._mark_bits, &mark_info->area_pool);
		if(ret == -1)
			return AREA_VISIT_ERR;
	}

	if(!IS_VOID_AREA_MARK(area->func_mark)){
		int ret =
			add_area_to_list(mark_info->func,
					sizeof(mark_info->func) / sizeof(area_list_t),
					area, area->func_mark._mark_bits,
					&mark_info->area_pool);
		if(ret == -1)
			return AREA_VISIT_ERR;
	}

	if(!IS_VOID_AREA_MARK(area->sem_mark)){
		int ret =
			add_area_to_list(mark_info->sem,
					sizeof(mark_info->sem) / sizeof(area_list_t),
					area, area->sem_mark._mark_bits,
					&mark_info->area_pool);
		if(ret == -1)
			return AREA_VISIT_ERR;
	}

	return AREA_VISIT_NORMAL ;
}

bool get_mark_result( area_tree_t * atree)
{
	assert(atree!=NULL&& atree->mark_info!=NULL) ;
	mark_area_info_t * mark_info = atree->mark_info ;
	return areatree_visit(atree , start_visit_for_mark_info , NULL , mark_info ) ;
}


/**
 * @brief ��λmark tree�Ŀռ䣬�ظ���ʼ��״̬
**/
void mark_tree_reset(area_tree_t * atree){
	if(atree){
//		//��mark_area_tree�е���preparation_mark�������·���ռ䣬��˱����ͷſռ�
//		if(atree->function_switch){
//					function_switch_destroy(atree->function_switch);
//					atree->function_switch = NULL;
//				}
//		if(atree->internal_info){
//			area_tree_internal_info_destroy(atree->internal_info);
//			atree->internal_info = NULL;
//		}
//
		if(atree->internal_info){
			area_tree_internal_info_clean(atree->internal_info);

		}

		if(atree->mark_info != NULL){
			mark_info_reset(atree->mark_info);//��λ��ע��Ϣ

		}
		if(atree->area_binfo_mgr != NULL){
			area_baseinfo_mgr_reset(atree->area_binfo_mgr);//����baseinfo������

		}
		if(atree->base_info)
		{
			atree->base_info->max_text_area_no = 0;
			atree->base_info->max_text_leaf_area_no = 0;
			atree->base_info->max_text_rate = 0;
			atree->base_info->max_text_rate_leaf = 0;
		}
		area_tree_clean(atree);//��λahtml��

	}
}
