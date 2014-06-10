/**
 * easou_vhtml_basic.h
 * Description: vtree����ʱ�Լ������һЩ�����жϺ����ò���,���ݽṹ�Ķ���
 *  Created on: 2011-11-13
 * Last modify: 2012-11-10 sue_zhang@staff.easou.com shuangwei_zhang@staff.easou.com
 *      Author: xunwu_chen@staff.easoucom
 *     Version: 1.2
 */

#ifndef EASOU_VHTML_BASIC_H_
#define EASOU_VHTML_BASIC_H_

#include <sys/types.h>
#include <stdint.h>
#include "nodepool.h"
#include "easou_html_tree.h"
#include "easou_css_parser.h"
#include "easou_css_utils.h"
#include "easou_vhtml_inner.h"
#include "easou_vstruct_profiler.h"

#define SET_LINE_BREAK(prop) (prop |= 0x1)
#define IS_LINE_BREAK(prop) (prop & 0x1)
#define SET_TEXT_WARP(prop) (prop |= (0x1<<1))
#define IS_TEXT_WARP(prop) (prop & (0x1<<1))
#define SET_ABSOLUTE(prop) (prop |= (0x1<<2))
#define IS_ABSOLUTE(prop) (prop & (0x1<<2))
#define SET_STYLED(prop) (prop |= (0x1<<3))
#define IS_STYLED(prop) (prop & (0x1<<3))
#define SET_BREAK_BEFORE(prop) (prop |= (0x1<<4))
#define IS_BREAK_BEFORE(prop) (prop & (0x1<<4))
#define SET_LINE_BREAK_WARP(prop) (prop |= (0x1<<5))
#define IS_LINE_BREAK_WARP(prop) (prop & (0x1<<5))

#define IS_FLOAT_LEFT(prop)	(prop & (0x1<<6))
#define SET_FLOAT_LEFT(prop)	(prop |= (0x1<<6))
#define IS_FLOAT_RIGHT(prop)	(prop & (0x1<<7))
#define SET_FLOAT_RIGHT(prop)	(prop |= (0x1<<7))
#define IS_FLOAT(prop)	(prop & ((0x1<<6)|(0x1<<7)))
#define IS_CLEAR_LEFT(prop) (prop & (0x1<<8))
#define SET_CLEAR_LEFT(prop) (prop |= (0x1<<8))
#define IS_CLEAR_RIGHT(prop) (prop & (0x1<<9))
#define SET_CLEAR_RIGHT(prop) (prop |= (0x1<<9))

#define IS_TEXT_VNODE(prop)	(prop & (0x1<<10))
#define SET_TEXT_VNODE(prop)	(prop |= (0x1<<10))

#define IS_BLOCK_TAG(prop)	(prop & (0x1<<11))
#define SET_BLOCK_TAG(prop)	(prop |= (0x1<<11))

#define IS_EMBODY_NOWX_IMG(prop)	(prop & (0x1<<12))		  /**< �Ƿ�������޿�ȵ�ͼƬ       */
#define SET_EMBODY_NOWX_IMG(prop)	(prop |= (0x1<<12))
#define CLEAR_EMBODY_NOWX_IMG(prop)	(prop &= ~(0x1<<12))

#define IS_INCLUDE_INTER(prop)	(prop & (0x1<<17))		  /**< �Ƿ�����н�����ǩ       */
#define SET_INCLUDE_INTER(prop)	(prop |= (0x1<<17))
#define CLEAR_INCLUDE_INTER(prop)	(prop &= ~(0x1<<17))

#define IS_BORDER(prop)	(prop & (0x1<<18))		  /**< �Ƿ��б߿�      */
#define SET_BORDER(prop)	(prop |= (0x1<<18))
#define CLEAR_BORDER(prop)	(prop &= ~(0x1<<18))

#define TOP_BIT 	(0x1<<28)
#define LEFT_BIT 	(0x1<<29)
#define BOTTOM_BIT	(0x1<<30)
#define RIGHT_BIT	(0x1<<31)

#define IS_IN_STYLE(prop)	(prop & (0x1<<13) )		  /**< �Ƿ���������STYLE������ */
#define SET_IN_STYLE(prop)	(prop |= (0x1<<13) )		  /**< ��������STYLE������ */
#define HAS_HTML_FONT_ATTR(prop)	(prop & (0x1<<14) )		  /**< �Ƿ�HTML��������������Ϣ */
#define SET_HAS_HTML_FONT_ATTR(prop)	(prop |= (0x1<<14) )		  /**< ����HTML�����к���������Ϣ */

#define SET_REPEAT_STRUCT_PARENT(prop)	(prop |= (0x1<<15))
#define IS_REPEAT_STRUCT_PARENT(prop)	(prop & (0x1<<15))
#define SET_REPEAT_STRUCT_CHILD(prop)	(prop |= (0x1<<16))
#define IS_REPEAT_STRUCT_CHILD(prop)	(prop & (0x1<<16))
#define CLEAR_REPEAT_STRUCT_CHILD(prop)	(prop &= ~(0x1<<16))

#define VTREE_ERROR	(-1)
#define	VTREE_NORMAL	1
#define	VTREE_FETCH_CSS_FAIL	2

#define DEFAULT_FONT_SIZE	16		  			/**< ��ҳĬ�������СΪ16px  */
#define DEFAULT_BGCOLOR	(0xffffff)		  		/**< ��ҳĬ�ϱ�����ɫ����ɫ  */
#define DEFAULT_COLOR	(0x000000)		  		/**< ��ҳĬ��ǰ����ɫ����ɫ  */
#define DEFAULT_LINK_COLOR	(0x0000ff)		    /**< ����Ĭ��ǰ����ɫ����ɫ  */

#define DEFAULT_IMG_SIZE	15		  			/**< Ĭ�ϵ�ͼƬ��С      */
#define DEFAULT_PAGE_WX 1200		  			/**< Ĭ�ϵ�ҳ����  */
#define MAX_TRUST_VALUE_FOR_VNODE	10		  	/**<  vnode->trust�����ֵ */

#define COLOR_SIGN_MAX_COLLI_NUM	4		  /**< ǩ����ͬ�������ɫ����  */
#define MAX_FONT_SIZE	10000

#define FONT_SIZE_TO_CHR_WX(px)	(px/2)		  /**< ���������С���㵥�ַ��Ŀ��  */
#define CSS_PAGE_LEN 512000

/**
 * @brief ��ɫ��RGB��ʾ.
 */
typedef struct _rgb_t
{
	unsigned int r :8;
	unsigned int g :8;
	unsigned int b :8;
} rgb_t;

/**
 * @brief �ڵ��CSS����.
 */
typedef struct _css_prop_node_t
{
	easou_css_prop_type_t type; /**< ��������  */
	int priority; /**< ����Ȩֵ  */
	char *value; /**< ����ֵ  */
	struct _css_prop_node_t *next;
} css_prop_node_t;

typedef enum _text_align_t
{
	VHP_TEXT_ALIGN_LEFT = 0, /**< ����룬Ĭ��  */
	VHP_TEXT_ALIGN_CENTER = 1, /**< ����       */
	VHP_TEXT_ALIGN_RIGHT = 2 /**< �Ҷ���       */
} text_align_t;

/**
 * @brief vnode�Ľڵ�����
 */
typedef enum _vnode_type_t
{
	TYPE_UNKNOWN = 0, //δ֪
	TYPE_INTERACTION = 1, //�����ڵ�
} vnode_type_t;

/**
 * @brief �Ա߿������
 * @author sue
 * @date 2013/05/21
 */
struct border_t
{
	unsigned int top;	/**< �ϱ߿�Ŀ�� */
	unsigned int left;	/**< ��߿�Ŀ�� */
	unsigned int right;	/**< �ұ߿�Ŀ�� */
	unsigned int bottom;	/**< �±߿�Ŀ�� */
	unsigned int pad_top;	/**< �ϼ�� */
	unsigned int pad_left;	/**< ���� */
	unsigned int pad_right;	/**< �Ҽ�� */
	unsigned int pad_bottom;/**< �¼�� */
};

/**
 * @brief �����������
 */
typedef struct _font_t
{
	unsigned int in_link :1; /**< �Ƿ��������У����Ƿ�������  */
	unsigned int align :2; /**< �ı���������       */
	unsigned int header_size :3; /**< �������壺h1~h6,���Ӧheader_sizeΪ1��6*/
	unsigned int is_bold :1; /**< ����  */
	unsigned int is_strong :1; /**< �ص�ǿ��	*/
	unsigned int is_big :1; /**< �ϴ�����  */
	unsigned int is_small :1; /**< ��С����  */
	unsigned int is_italic :1; /**< б��  */
	unsigned int is_underline :1; /**< �»���   */
	int size :20; /**< �����С����pxΪ��λ�������Ĭ�������СΪ16px  */
	int line_height :20; /**< �иߣ���pxΪ��λ��Ĭ��ֵΪ��ǰ�����С  */
	unsigned int bgcolor :24; /**< ������ɫ����RGB��ʾ����#ffffff   */
	unsigned int color :24; /**< ������ɫ(��ǰ����ɫ)����RGB��ʾ����#000000  */
} font_t;

typedef struct _vhtml_struct_prof_t vhtml_struct_prof_t;
typedef struct _html_area_t html_area_t;

/**
 * @brief vtree��һ���ڵ������.
 */
typedef struct _html_vnode_t
{
	html_node_t *hpNode; /**< �ýڵ��Ӧ��html_tree�ϵĽڵ�  */
	unsigned int isValid :1; /**< �Ƿ�	ռ�ݿռ�Ľڵ㣬���Ƿ����  */
	unsigned int inLink :1; /**< �Ƿ���������   */

	vnode_type_t type; //�ڵ�����
	font_t font; /**< ������Ϣ  */
	border_t border; /**< �߿���Ϣ */
	int subtree_diff_font; /**< �����ж��ٲ�ͬ���������壬�����Լ���ֻͳ������С��40�ģ� */
	int subtree_max_font_size; /**< ������������壬�����Լ� */
	int subtree_border_num; /**< �����������Լ�������border���Եĸ��� */
	char fontSizes[40];
	int wx; /**< �ڵ�Ŀ��  */
	int hx; /**< �ڵ�ĸ߶�  */
	int xpos, ypos; /**< �ڵ����Ͻǵ�X,Y����,ҳ�����Ͻ�����Ϊ(0,0)   */
	int textSize; /**< �ı��ڵ���ı���С  */
	int cn_num; /**< �ı��ڵ�ĺ��ָ��� */
	int subtree_textSize; /**< �Ե�ǰ�ڵ�Ϊ���ڵ��������textSize  */
	int subtree_anchorSize; /**< �Ե�ǰ�ڵ�Ϊ���ڵ��������anchorSize  */
	int subtree_cn_num; /**< �Ե�ǰ�ڵ�Ϊ���ڵ���������еĺ��ָ��� */
	int depth; /**< �ڵ�����, ���ڵ����Ϊ0 */
	int id; /**< �ڵ��ΨһID, ��0��ʼ+1������� */
	u_int property; /**< �ڵ㰴λ���õ�����   */
	vstruct_info_t *struct_info; /**<  ��ǰ�ڵ��Ӧ�������ṹ��Ϣ */
	css_prop_node_t *css_prop; /**< ��ǰ�ڵ��Ӧ��CSS��������  */
	html_area_t *hp_area; /**< �����ýڵ����С�ֿ�*/
	struct _html_vnode_t * firstChild; /**< ��һ������  */
	struct _html_vnode_t * prevNode; /**< ǰһ���ֵ�  */
	struct _html_vnode_t * nextNode; /**< ��һ���ֵ�   */
	struct _html_vnode_t * upperNode; /**< ���׽ڵ� */

	short trust :5; /**< ��С��λ�õĿ��Ŷȡ�
	 ����ͼƬ�ڵ���˵��trust�������壺
	 a)	����trust = 10;
	 b)	�����δ֪��trust -= 3;
	 c)	���߶�δ֪, trust -= 2;
	 d)	�������ΪĬ��ֵ����δ���㣩��trust -= 3��
	 e)	���߶���ΪĬ��ֵ����δ���㣩��trust -= 2��
	 ����
	 ====================================================================

	 �߶���֪	�߶�δ֪�ҹ���	�߶�δ֪��ΪĬ��ֵ
	 �����֪		10				8				6
	 ���δ֪�ҹ���		7				5				3
	 ���δ֪��ΪĬ��ֵ		4				2				0
	 ====================================================================
	 */
	// for inner use
	short wp, hp; /**< �ڵ�İٷֱȿ�Ⱥ͸߶�  */
	int min_wx; /**< �ڵ���С���ܿ��  */
	int max_wx; /**< �ڵ�����ܳ����Ŀ�� */
	int colspan; /**< ���ĵ�Ԫ����п��  */
	html_vtree_t *vtree; //�ڵ�������vtree
	void *user_ptr;//�û��Զ���ָ�룬����ָ���Լ���Ҫ�Ľṹ
	//int subnodetype;//��ʾ�ýڵ�ĺ���ڵ������type�����һλ�Ƿ���P���ұ�2�Ƿ���DIV���ұ�3λ�Ƿ���table
   int whxy; //css �Ƿ�ָ����ȡ��߶ȡ�xpos��ypos
} html_vnode_t;

/**
 * @brief Vtree���ṹ.
 *   ά��vtree�����ṹ,�ڴ�.
 */
typedef struct _html_vtree_t
{
	html_tree_t *hpTree; /**<  vtree��Ӧ��html_tree  */
	html_vnode_t *root; /**<  vtree�ĸ��ڵ�  */

	/** Below for inner use only */
	nodepool_t np; /**< html_vnode�Ľڵ��   */
	nodepool_t css_np; /**<  css_prop_node_t�Ľڵ��  */
	nodepool_t struct_np; /**< һ��ṹ��Ϣ�Ľڵ�� */
	unsigned int struct_np_inited :1; /**< �Ƿ�����˽ṹ��Ϣ�Ĵ洢�ռ� */
	unsigned int normal_struct_info_added :1; /**< �Ƿ��Ѽ�����һ��Ľṹ��Ϣ */
	unsigned int repeat_struct_info_added :1; /**< �Ƿ��Ѽ������ظ��ṹ��Ϣ*/
	html_vnode_t *body; /**<  vtree��body�ڵ�  */
} html_vtree_t;

/**
 * @brief vtree����ṹ.���ڰ�װCSS��ص�����..
 *	ÿ��VTREE��Ӧ����һ���ṹ.
 */
typedef struct _vtree_in_t
{
	easou_css_env_t *css_env; /**< CSS�������� */
	char CSS_OUT_PAGE[CSS_PAGE_LEN];
	unsigned long long url_num;
	unsigned long long request_css_num;
	unsigned long long missing_css_num;
} vtree_in_t;

/**
 * @brief ����ɫ����ֵ��ʾת��ΪRGB��ʾ.
 *
 * @param [in] color   : unsigned int ��ɫ����ֵ��ʾ����0xffffff
 * @return  rgb_t
 * @retval
 * @see
 * @author xunwu
 * @date 2011/06/27
 **/
rgb_t int2rgb(unsigned int color);

/**
 * @brief �������Ƿ���ȫ��ͬ.
 *
 * @author xunwu
 * @date 2011/06/27
 **/
bool is_same_font(font_t *a, font_t *b);

/**
 * @brief �Ƿ��ɫ��
 * R,G,Bֵ��ͬ�����ֲ�Ϊ�ڻ�ף���Ϊ��ɫ.
 * @param [in/out] color   : unsigned int
 * @return  bool
 * @retval
 * @see
 * @author xunwu
 * @date 2011/06/27
 **/
bool is_gray_color(unsigned int color);

/**
 * @brief	�������ֵ�λ�ĳ��ȣ�ͳһ���ΪpxΪ��λ�ĳ��ȡ�
 * @param [in] value   : const char*	�����ַ���
 * @param [in] base_size   : int ��׼���ȣ�������λ������Ҫ
 * @param [out] _unit   : const char** ����ԭ���ȵ�λ
 * @return  int	���ؽ���������px���ȣ���û�е�λ������-1��
 * @author xunwu
 * @date 2011/06/20
 **/
int parse_length(const char *value, int base_size, const char **_unit);

/**
 * @brief ��ȡ���ı��ĳ���
 * @author sue
 * @date 2013/05/21
 */
int get_text_length(int font_size, int cn_num, int chr_num);

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
int html_vnode_visit_ex(html_vnode_t *html_vnode, int (*start_visit)(html_vnode_t *, void *), int (*finish_visit)(html_vnode_t *, void *),
		void *result);

/**
 * @brief
 * @author xunwu
 * @date 2011/06/27
 **/
int html_vtree_visit_ex(html_vtree_t *html_vtree, int (*start_visit)(html_vnode_t *, void *), int (*finish_visit)(html_vnode_t *, void *), void *result);

void print_font(font_t *pfont);

#endif /* EASOU_VHTML_BASIC_H_ */
