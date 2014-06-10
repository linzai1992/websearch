/*
 * easou_ahtml_baseinfo.h
 *
 *  Created on: 2011-11-18
 *      Author: ddt
 */

#ifndef EASOU_AHTML_BASEINFO_H_
#define EASOU_AHTML_BASEINFO_H_
#include "nodepool.h"
#include "easou_vhtml_basic.h"
#include "easou_ahtml_tree.h"

/*vnode�ڵ�����*/
typedef struct vnode_list_t
{
	html_vnode_t * vnode ;
	struct vnode_list_t * next;
	struct vnode_list_t * pre;
} vnode_list_t ;

/**�����������*/
/*����ⲿ��Ϣ*/
typedef struct _AOI4ST_extern_t
{
	int extern_area ; /*�ⲿ�����*/
	vnode_list_t * extern_vnode_list_begin ;		  /**<  ���ⲿ�ڵ���������      */
	vnode_list_t * extern_vnode_list_end ;			  /**<  ���ⲿ�ڵ���������      */
}AOI4ST_extern_t;

/*��Ľ�����Ϣ*/
typedef struct _AOI4ST_interaction_t
{
	bool is_have_form;				//�Ƿ���form��ǩ
	int textarea_num;				//textarea��ǩ��
	int input_num ;  		  		/**<  input ��ǩ����      */
	int input_radio_num ;		    /**<��radio���Ե�input��ǩ����        */
	int input_radio_txtlen ;	    /**<��radio���Ե�input��ǩ�ı�����        */
	int select_num ;		        /**<select��ǩ����        */
	int option_num ;		        /**<option��ǩ����        */
	int option_txtlen ;		        /**<option��ǩ�ı�����        */
	int in_area ;		            /**<������ǩ���       */
	int cursor_num;								/**<����cursor���Եı�ǩ����       */
	int script_num;								/**<����script��ǩ�ĸ���	*/
	int spec_word_num;		/**<���н����鳣���ؼ��ʵ��ı�����     */
	vnode_list_t * interaction_vnode_list_begin ; /**<�������ڵ���������      */
	vnode_list_t * interaction_vnode_list_end ;   /**<�������ڵ���������      */
}AOI4ST_interaction_t;

/*���ͼƬ��Ϣ*/
typedef struct  _AOI4ST_pic_t
{
	int pic_num ;		  	  /**<img ��ǩ����        */
	int pic_area ;		      /**<ͼƬ���        */
	int link_pic_num ; 		  /**<ͼƬ���Ӹ���        */
	int link_pic_area ;		  /**<ͼƬ�������        */
	int size_fixed_num;			/**<ͼƬ�������趨�õ�ͼƬ����		*/
	vnode_list_t *pic_vnode_list_begin ; /**<  ��ͼƬ�ڵ���������      */
	vnode_list_t * pic_vnode_list_end ;  /**<  ��ͼƬ�ڵ���������      */
}AOI4ST_pic_t;

/*���������Ϣ*/
typedef struct _AOI4ST_link_t
{
	vnode_list_t *url_vnode_list_begin ;/**<  ��link�ڵ���������      */
	vnode_list_t *url_vnode_list_end ;  /**<  ��link�ڵ���������      */
	int num ;		                    /**<  �����ĳ�������       */
	int inner_num;		  				/**< �������������������ж�       */
	int out_num;		  				/**< �������������������ж�       */
	int other_num;		  				/**< "javascript:","mailto:"���Ӹ���       */
	int anchor_size ;		            /**< anchor��С       */
	int link_area ;		                /**< ���       */
	int anchor_size_before;		  		/**< ��ǰ��֮ǰ(��������ȱ�����)��anchor size       */
}AOI4ST_link_t ;

/*����ı���Ϣ*/
typedef struct _AOI4ST_text_t
{
	vnode_list_t *cont_vnode_list_begin ; /**<  ��pure_text�ڵ���������      */
	vnode_list_t *cont_vnode_list_end ;   /**<  ��pure_text�ڵ���������      */
	int con_num ;		  				  /**<puretext�ڵ����        */
	int no_use_con_num ;		          /**<��������ı����������������ո��        */
	int con_size ;						  /**<�ı�����        */
	int no_use_con_size;		          /**< ��������ı�����       */
	int cn_num;	/**< ���ĺ��ָ��� */
	int text_area ;						  /**<�ı������        */
	int no_use_text_area ;				  /**<�������ı������        */
	int time_num ;						  /**<ʱ���ַ������������ܲ�̫׼ȷ��������ʶ���� */
	int con_size_before;		          /**< ��ǰ��֮ǰ����������ȱ����㣩��anchor size       */
	bool recommend_spec_word; /**< �Ƿ����Ƽ��ؼ��� */
	float pure_text_rate; /**< ���ı��ڵ�ǰ��֮ǰ����ռ�ı���     */
}AOI4ST_text_t ;


/*��Ļ�����Ϣ,ÿһ���ֿ鶼����ôһ����Ϣ*/
typedef struct _area_baseinfo_t{
	AOI4ST_extern_t extern_info ;		  /**<����ⲿ��Ϣ        */
	AOI4ST_interaction_t inter_info ;	  /**<��Ľ�����Ϣ        */
	AOI4ST_pic_t  pic_info ;		      /**<���ͼƬ��Ϣ        */
	AOI4ST_link_t link_info ;		      /**<���������Ϣ        */
	AOI4ST_text_t text_info ;		      /**<����ı���Ϣ        */
}area_baseinfo_t;

/**
 * @brief A���Ļ�����Ϣ
 */
typedef struct _atree_baseinfo_t
{
	int max_text_area_no; /**< ��������ı������ķֿ��      */
	float max_text_rate; /**< ����ı�����      */
	int max_text_leaf_area_no; /**< ��������ı������ķֿ��      */
	float max_text_rate_leaf; /**< ����ı�����      */
} atree_baseinfo_t;

typedef struct _all_vnode_list_t
{
	vnode_list_t * ex_begin ;
	vnode_list_t * ex_end ;
	vnode_list_t * in_begin ;
	vnode_list_t * in_end ;
	vnode_list_t * pic_begin ;
	vnode_list_t * pic_end ;
	vnode_list_t * url_begin ;
	vnode_list_t * url_end  ;
	vnode_list_t * text_begin ;
	vnode_list_t * text_end ;
}all_vnode_list_t ;

/**
* @brief ����ֿ��ע�Ĺ����м���Ϣʹ�õ��ڴ��������atree����
*/
typedef struct _area_baseinfo_mgr_t
{
	nodepool_t np_area_out_info ;
	nodepool_t np_vnode_list ;
	all_vnode_list_t  all_vnode_list ;
}area_baseinfo_mgr_t ;

/**
 * @brief ��ʼ��baseinfo������
**/
int area_baseinfo_mgr_init(area_baseinfo_mgr_t * area_mgr , int m  ,int n);
/**
 * @brief ����baseinfo������
**/
void area_baseinfo_mgr_reset(area_baseinfo_mgr_t * area_mgr);
/**
 * @brief ����baseinfo������
**/
void area_baseinfo_mgr_des( area_baseinfo_mgr_t * area_mgr );

/**
 * @brief ����baseinfo
**/
void area_baseinfo_reset(area_baseinfo_mgr_t * area_mgr);
/**
 * @brief ��ȡbaseinfo
**/
area_baseinfo_t * get_area_baseinfo_node(area_baseinfo_mgr_t * area_mgr);
/**
 * @brief ��ȡbaseinfo�е�list
**/
vnode_list_t * get_vnode_list_node(area_baseinfo_mgr_t * area_mgr);

/**
 * @brief ����ÿ��area�е�baseinfo
**/
bool fill_base_info(area_tree_t * atree );

#endif /* EASOU_AHTML_BASEINFO_H_ */
