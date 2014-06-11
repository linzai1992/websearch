#ifndef EASOU_AHTML_AREA_H_
#define EASOU_AHTML_AREA_H_

#include <limits.h>
#include "util/htmlparser/utils/nodepool.h"
#include "util/htmlparser/vhtmlparser/vhtml_basic.h"
#include "util/htmlparser/vhtmlparser/vhtml_tree.h"

/** �����ͨ����Ϣ�����ꡢ����*/
typedef struct _area_info_t {
  int xpos;
  int ypos;
  int width;
  int height;
} area_info_t;

/**
 * @brief �ֿ�ľ���λ�ã������ҳ��
 */
typedef enum _html_area_abspos_mark_t {
  PAGE_UNDEFINED = 0,
  PAGE_HEADER,
  PAGE_LEFT,
  PAGE_RIGHT,
  PAGE_FOOTER,
  PAGE_MAIN,
  PAGE_MAIN_RELATED, /**< ��ʱδ��,�ϰ汾����       */
  PAGE_INVALID
} html_area_abspos_mark_t;

/**
 * @brief �ֿ�����λ�ã�����ڸ��ֿ�
 */
typedef enum _html_area_relpos_mark_t {
  RELA_UNDEFINED = 0,
  RELA_HEADER,
  RELA_LEFT,
  RELA_RIGHT,
  RELA_FOOTER,
  RELA_MAIN
} html_area_relpos_mark_t;

/**
 * @brief ��Ͼ���λ�ú����λ�ö���ǳ���ÿ���ֿ���ҳ���е�λ�ã��÷ݿ���ԱȽ�Ȩ��
 */
typedef enum _html_area_pos_plus_t {
  IN_PAGE_UNDEFINED = 0,
  IN_PAGE_HEADER,
  IN_PAGE_LEFT,
  IN_PAGE_RIGHT,
  IN_PAGE_FOOTER,
  IN_PAGE_MAIN
} html_area_pos_plus_t;

/**
 * @brief �ֿ�����ýṹ��������partition�Ĺ�����ʹ��.
 */
typedef struct _area_config_t {
  int min_width; /**< ���С�ڴ�ֵ�ķֿ鲻�ٻ���  */
  int min_height; /**< �߶�С�ڴ�ֵ�ķֿ鲻�ٻ���  */
  int min_size; /**< ���С�ڴ�ֵ�ķֿ鲻�ٻ���  */
  unsigned int max_depth; /**< ��ȴ��ڵ��ڴ�ֵ�ķֿ鲻�ٻ���  */
  const char *indivisible_tag_name; /**< ���ɷֵ�tag����,����tag���ַ���,����ΪNULL */
} area_config_t;

/**< ���ýṹ�ĳ�ʼֵ */
static const area_config_t AREA_CONFIG_INIT_VALUE ={
  min_width : 0,
  min_height : 0,
  min_size : 0,
  max_depth : INT_MAX,
  indivisible_tag_name : NULL
};

/**
 * @brief ���ô˽ṹ������ʾ��λ�������ֶΣ���ֱֹ����==�ж�.
 */
typedef struct {
  unsigned int _mark_bits;
} bits_field_t;

typedef struct _area_tree_t area_tree_t;
typedef struct _area_uid_binfo_t area_uid_binfo_t;
typedef struct _area_baseinfo_t area_baseinfo_t;

/**
 * @brief �ֿ�ڵ�ṹ.
 */
typedef struct _html_area_t {
  /**�ֿ��λ������*/
  html_vnode_t *begin; /**< �ֿ鿪ʼ�ڵ�  */
  html_vnode_t *end; /**< �ֿ�Ľ����ڵ�  */
  html_vnode_t *main_vnode;
  bool isValid :1; /**< �ֿ��Ƿ�ɼ�  */
  bool is_pos_partition_level :1; /**< ���������ֵ����丸�ڵ��λ�ñ�ע(pos_plus)����ȫһ�� */
  area_info_t area_info; /**< �ֿ���������� */
  html_area_abspos_mark_t abspos_mark; /**< �����ҳ���λ�� */
  html_area_relpos_mark_t pos_mark; /**< ����ڸ��ֿ��λ�� */
  html_area_pos_plus_t pos_plus; /**< ��Ͼ���λ�ú���Ը��ֿ��λ�ö����е�λ�ñ�ʶ����׼ȷ������ʹ�� */

  /**�ֿ�Ĺ�ϵ����*/
  unsigned int depth; /**< ��ǰ�ֿ��ڷֿ����е����,���ֿ�(��Ӧ����ҳ��)���Ϊ0  */
  unsigned int max_depth;
  unsigned int subArea_num; /**< �ӷֿ������  */
  unsigned int valid_subArea_num; /**< �ɼ��ӷֿ������  */

  struct _html_area_t *parentArea; /**< ���ֿ�  */
  struct _html_area_t *subArea; /**< ��һ���ӷֿ�  */
  struct _html_area_t *nextArea; /**< ��һ���ֿ�  */
  struct _html_area_t *prevArea; /**< ǰһ���ֿ�  */

  /**�ֿ����Դ�����ܡ���������*/
  bits_field_t srctype_mark; /**< ��ǰ�����Դ���ͱ�ע  */
  bits_field_t func_mark; /**< ��ǰ��Ĺ������ͱ�ע  */
  bits_field_t sem_mark; /**< ��ǰ����������ͱ�ע  */

  bits_field_t subtree_srctype_mark; /**< �Ե�ǰ��Ϊ���ڵ�������к��е���Դ���ͱ�ע  */
  bits_field_t subtree_func_mark; /**< �Ե�ǰ��Ϊ���ڵ���������еĹ������ͱ�ע  */
  bits_field_t subtree_sem_mark; /**< �Ե�ǰ��Ϊ���ڵ���������е��������ͱ�ע  */

  bits_field_t upper_srctype_mark; /**< ��ǰ�鼰��ǰ���ϲ�Ŀ����Դ���ͱ�ע       */
  bits_field_t upper_func_mark; /**< ��ǰ�鼰��ǰ���ϲ�Ĺ������ͱ�ע       */
  bits_field_t upper_sem_mark; /**< ��ǰ�鼰��ǰ���ϲ���������ͱ�ע       */

  /**base info*/
  char *uid; /**<  �û���      */
  area_uid_binfo_t *uidbinfo; /**<  ָ���û�������ǰ����Ļ�����Ϣ,��mark���      */
  area_baseinfo_t *baseinfo; /**< ָ����עǰ��ǰ����Ļ�����Ϣ,����valid�Ŀ��д����ԣ�mark���*/
  unsigned int no; /**< ÿ���ֿ��Ψһ��ʶ�ţ���0��ʼ+1��������������Ϊ�û��Զ���������±�  */
  int order; /**< ͬһ���ֿ���ϵ���� */
  area_tree_t *area_tree;
  int areaattr;                        // area attribute
  int nodeTypeOfArea;  //���ɿ�Ľڵ��������һλ�Ƿ���P���ұ�2�Ƿ���DIV���ұ�3�Ƿ���H1-H6,�ұ�4λ�Ƿ���table;��5�Ƿ���ul��ol;��6�Ƿ���form,
  struct _html_area_t *titleArea; /**�����ÿ�ı����  */
} html_area_t;

#define SET_LEAF_AREA(x)	((x)|=0x1)
#define IS_LEAF_AREA(x)	((x)&0x1)
#endif /* EASOU_AHTML_AREA_H_ */
