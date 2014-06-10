/**
 * @file css_pool.h
 * @author xunwu
 * @date 2011/06/20
 * @version 1.0(create)
 * @brief css_pool����:������CSS�ṹ�Ŀռ�,��Ŷ�������õ�CSS�ṹ.
 *
 **/
#ifndef EASOU_CSS_POOL_H_
#define EASOU_CSS_POOL_H_

#include "easou_css_parser.h"

#define MAX_CSS_NUM_IN_POOL 512
#define DEFAULT_CSS_NUM_IN_POOL	8		  /**< Ĭ�Ϸ���ռ��CSS�ṹ����  */

/**css�Ľ������*/
typedef struct _easou_css_pool_t
{
	easou_css_t *css_array[MAX_CSS_NUM_IN_POOL]; /**< CSS����ָ�� */
	short order[MAX_CSS_NUM_IN_POOL]; /**< ��Ӧ��CSS����ţ�order����Խ�����ȼ�Խ��*/
	int alloc_css_num; /**< �ѷ���ռ��CSS�ṹ����  */
	int used_css_num; /**< ��ʹ��(��װ�н�����Ľṹ)��CSS����*/
	hashmap_t *hm;
} easou_css_pool_t;

/**
 * @brief ���css_pool,ʹCSS�ṹ�ص�δ������״̬.
 * @author xunwu
 * @date 2011/06/21
 **/
void css_pool_clean(easou_css_pool_t *css_pool);

/**
 * @brief	��ȡcss_pool��CSS���������.
 * @author xunwu
 * @date 2011/06/21
 **/
int get_css_pool_array_size(easou_css_pool_t *css_pool);

/**
 * @brief	����css_pool,�����ѷ���Ŀռ�.
 * @author xunwu
 * @date 2011/06/20
 **/
void css_pool_destroy(easou_css_pool_t *css_pool);

/**
 * @brief ��ʼ��css_pool,ΪCSS�ṹ����ռ�.
 * @param [in/out] css_pool   : css_pool_t*	�ѷ���ռ��css_pool.
 * @param [in/out] max_css_page_size   : int	css_page�Ĵ�С,�������ֵΪCSS�ṹ����ռ�.
 * @param [in/out] css_num   : int	����ռ��CSS�ṹ����.
 * @return  int
 * @retval  -1:ʧ��;1:�ɹ�.
 * @author xunwu
 * @date 2011/06/20
 **/
int css_pool_init(easou_css_pool_t *css_pool, int max_css_page_size, int css_num);

/**
 * @brief ��csspool�е�CSS��orderֵ��С��������.orderֵԽ��,���ȼ�Խ��.
 * @author xunwu
 * @date 2011/06/20
 **/
void css_pool_sort(easou_css_pool_t *css_pool);

#endif /*EASOU_CSS_POOL_H_*/
