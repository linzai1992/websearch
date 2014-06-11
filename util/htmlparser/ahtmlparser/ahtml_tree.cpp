
#include <pthread.h>
#include "util/htmlparser/htmlparser/html_attr.h"
#include "util/htmlparser/vhtmlparser/vhtml_tree.h"
#include "util/htmlparser/ahtmlparser/ahtml_tree.h"
#include "util/htmlparser/ahtmlparser/ahtml_dtd.h"
#include "util/htmlparser/vhtmlparser/vstruct_profiler.h"
#include "util/htmlparser/utils/debug.h"

#define BIG_SHORT_INT	30000
#define DEFAULT_AREA_BLOCK_SIZE	100

#define WX_PERCENT_BE_DIVIDE	55		  /**< һ���ڵ����븸�ֿ�������ڴ�ֵ,�Ҹ߶�Ҳ����һ����,����Ҫ����  */
#define HX_PERCENT_BE_DIVIDE	40		  /**< һ���ڵ�߶��븸�ֿ�������ڴ�ֵ,�ҿ��Ҳ����һ����,����Ҫ����  */
#define	WX_PERCENT_DIVE_IN	100		  /**< ��һ���ڵ����븸�ֿ����С�ڴ�ֵ,�����ýϴֲڵķֿ鷽ʽ*/
#define HX_PERCENT_DIVE_IN	100		  /**< ��һ���ڵ�߶��븸�ֿ����С�ڴ�ֵ,�����ýϴֲڵķֿ鷽ʽ */
#define SMALL_HX_VAL	1		  /**< ���ڸ߶�С�ڴ�ֵ�Ľڵ�,�������������ڵ���ϳɿ�  */
#define SMALL_WX_VAL	1		  /**< ���ڿ��С�ڴ�ֵ,����Ϊ̫С  */

/**
 * @brief	����һ�÷ֿ���.
 * @param [in] cfg   : area_config_t*	�������÷ֿ����ķֿ�����.��ΪNULL.
 * @return   area_tree_t*	�ѷ���ռ�ķֿ���.

 * @date 2011/07/05
 **/
area_tree_t * area_tree_create(area_config_t *cfg) {
  area_tree_t *atree = (area_tree_t *) malloc(sizeof(area_tree_t));
  if (atree == NULL) {
    goto FAIL;
  }
  memset(atree, 0, sizeof(area_tree_t));
  if (!nodepool_init(&(atree->np), sizeof(html_area_t),
                     DEFAULT_AREA_BLOCK_SIZE)) {
    goto FAIL;
  }
  area_tree_clean(atree);
  area_tree_setConfig(atree, cfg);
  return atree;

  FAIL: if (atree) {
    area_tree_del(atree);
  }
  return NULL;
}

/**
 * @brief ���һ�÷ֿ���.

 * @date 2011/07/12
 **/
void area_tree_clean(area_tree_t *atree) {
  nodepool_reset(&atree->np);
  atree->hp_vtree = NULL;
  atree->root = NULL;
  atree->mark_status = 0;
}

/**
 * @brief ɾ���ֿ���.

 * @date 2011/07/05
 **/
void area_tree_del(area_tree_t *atree) {
  if (atree == NULL) {
    return;
  }
  nodepool_destroy(&atree->np);
  free(atree);
  atree = 0;
}

/**
 * @brief ���÷ֿ����ķֿ�����.

 * @date 2011/07/05
 **/
void area_tree_setConfig(area_tree_t *atree, area_config_t *cfg) {
  if (cfg) {
    atree->config = *cfg;
  } else {
    atree->config = AREA_CONFIG_INIT_VALUE;
  }
}

static html_area_t *createAreaNode(nodepool_t *np) {
  html_area_t *area = (html_area_t *) nodepool_get(np);
  if (area == NULL) {
    return NULL;
  }
  memset(area, 0, sizeof(html_area_t));
  area->isValid = true;
  return area;
}

/**
 * @brief ���ַֿ�����б������Ϣ,�൱�ڷֿ��α꣬��Ҫ��¼һ�������γɵĿ�ĵ�ǰ��Ϣ.
 */
typedef struct _dividing_t {
  area_tree_t *atree; /**< �����ķֿ���*/
  html_vnode_t *begin; /**< �ֿ�Ŀ�ʼ�ڵ�  */
  html_vnode_t *end; /**< �ֿ�Ľ����ڵ�  */
  int xpos; /**< ��ǰ���X����  */
  int ypos; /**< ��ǰ���Y����  */
  int wx; /**< ��ǰ��Ŀ��  */
  int hx; /**< ��ǰ��ĸ߶�  */
  unsigned int depth; /**< ��ǰ����ڷֿ����е����  */
  html_vnode_t * last_valid; /**< ��һ����Ч�Ľڵ�  */
  html_vnode_t * last_validword; /**< ��һ����Ч���������ֳ��ȴ���0�Ľڵ�  */
  html_area_t *parent_area; /**< ���ֿ�  */
  html_area_t **cur_tail; /**< �¿���Ҫ���ҵ�ָ��  */
  html_area_t *prev_area; /**< ǰһ���ֵֿܷ�  */
  nodepool_t *np; /**< �ֿ�Ľڵ��  */
  const area_config_t *config; /**< �Էֿ鹦�ܵ��ⲿ���� */
  bool wap_page; /**< �Ƿ�Ϊwapҳ�� */
} dividing_t;

/**
 * @brief	����ֿ�����б������Ϣ.

 * @date 2011/07/05
 **/
static void dividing_collect_clr(dividing_t *divider) {
  divider->begin = NULL;
  divider->end = NULL;
  divider->xpos = -1;
  divider->ypos = -1;
  divider->wx = 0;
  divider->hx = 0;
  divider->last_valid = NULL;
  divider->last_validword = NULL;
}

/**
 * @brief ����α�.

 * @date 2011/07/05
 **/
static void dividing_clr(dividing_t *divider) {
  dividing_collect_clr(divider);
  divider->depth = 0;
  divider->cur_tail = NULL;
  divider->prev_area = NULL;
  divider->wap_page = false;
}

/**
 * @brief �����ռ�����Ϣ�����ֿ�ڵ�.

 * @date 2011/07/05
 **/
static html_area_t *blockToAreaNode(dividing_t *divider) {
  html_area_t *aNode = createAreaNode(divider->np);
  if (aNode == NULL) {
    return NULL;
  }
  aNode->area_tree = divider->atree;
  aNode->begin = divider->begin;
  aNode->end = divider->end;
  aNode->area_info.xpos = divider->xpos;
  aNode->area_info.ypos = divider->ypos;
  aNode->area_info.width = divider->wx;
  aNode->area_info.height = divider->hx;
  aNode->depth = divider->depth;
  aNode->parentArea = divider->parent_area;
  aNode->prevArea = divider->prev_area;

  if (divider->last_valid) {
    aNode->isValid = true;
  } else {
    aNode->isValid = false;
  }

  // update parent area
  aNode->parentArea->subArea_num++;
  if (aNode->isValid) {
    aNode->parentArea->valid_subArea_num++;
  }

  // update the dividing structure
  *(divider->cur_tail) = aNode;
  divider->cur_tail = &(aNode->nextArea);
  divider->prev_area = aNode;
  return aNode;
}

#define SET_TO_RIGHT(x)	((x)|=0x1)
#define IS_TO_RIGHT(x)	((x)&0x1)
#define SET_TO_LEFT(x)	((x)|=0x2)
#define IS_TO_LEFT(x)	((x)&0x2)
#define SET_TO_UPPER(x)	((x)|=0x4)
#define IS_TO_UPPER(x)	((x)&0x4)
#define	SET_TO_LOWER(x)	((x)|=0x8)
#define	IS_TO_LOWER(x)	((x)&0x8)

/**
 * @brief �жϵ�ǰ�ڵ���ǰ��ڵ��λ�ù�ϵ.

 * @date 2011/07/05
 **/
static unsigned int check_pos(html_vnode_t *vnode, dividing_t *divider) {
  int pos = 0;
  int xdiff = vnode->xpos - divider->xpos;
  if (xdiff >= divider->wx && xdiff > 0) {
    SET_TO_RIGHT(pos);
  } else if (xdiff <= 0 - vnode->wx && xdiff < 0) {
    SET_TO_LEFT(pos);
  }
  int ydiff = vnode->ypos - divider->ypos;
  if ((ydiff >= divider->hx || (divider->last_valid && divider->last_valid->hpNode->html_tag.tag_type == TAG_BR))
      && ydiff > 0) {
    SET_TO_LOWER(pos);
  } else if (ydiff <= 0 - vnode->hx && ydiff < 0) {
    SET_TO_UPPER(pos);
  }
  return pos;
}

#define	NO_FIT		0
#define FIT_LOWER	1
#define FIT_RIGHT	2
#define FIT_LEFT	3

#define AREA_TOBE_DIVIDE	0		    /**< ��ǰ�ڵ���Ҫ����  */
#define AREA_NEW_BEGIN	1		  	 	/**< �Ե�ǰ�ڵ㿪ʼ,�����µķֿ�  */
#define	AREA_END	2		  			/**< �Ե�ǰ�ڵ���Ϊ��ǰ�ֿ�Ľ���  */
#define AREA_END_BOTH	3		  		/**< ֮ǰ�ռ��Ľ����Ϊһ���ֿ�,��ǰ�ڵ��Ϊһ���ֿ�  */
#define AREA_CONTINUE	4		  		/**< �����ռ��ڵ�  */

//�۲���
typedef struct _observer_t {
  unsigned int fit_pos :16; /**< ���λ��  */
  unsigned int act :16; /**< ��ȡ�ķֿ���Ϊ  */
} observer_t;

/**
 * @brief �жϵ�ǰ�ڵ���ǰ���ռ��Ľڵ��λ�������ϵ.

 * @date 2011/07/05
 **/
static observer_t check_fit_prev(html_vnode_t *vnode, dividing_t *divider) {
  observer_t obsv = { 0, 0 };
  if (vnode->hpNode->html_tag.tag_type == TAG_BR) {
    if (divider->wap_page == true) {
      //��ǰ�ڵ���br��WAP�������ں�
      debuginfo(DIVIDING_AREA,
                "[depth:%d] node(id=%d)<%s> FIT_LOWER & AREA_NEW_BEGIN",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      obsv.fit_pos = FIT_LOWER;
      obsv.act = AREA_NEW_BEGIN;
    } else {
      //��ǰ�ڵ���br��WEB�����ں�
      if (divider->last_valid && divider->last_valid->ypos <= vnode->ypos) {
        debuginfo(DIVIDING_AREA,
                  "[depth:%d] node(id=%d)<%s> FIT_LOWER & AREA_CONTINUE",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
        obsv.fit_pos = FIT_LOWER;
        obsv.act = AREA_CONTINUE;
      }

    }
    return obsv;
  }
  //�����ǰ�ֿ黹û�нڵ㣬��ô��ǰ���ռ��Ľڵ㲻����
  if (divider->begin == NULL) {
    obsv.fit_pos = NO_FIT;
    debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> NO_FIT",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return obsv;
  }
  //�鿴�ڵ��뵱ǰ�ֿ�����λ�ã��ϡ��¡����ң�
  unsigned int pos = check_pos(vnode, divider);
  //��ǰ�ڵ��ڵ�ǰ�ֿ������
  if (IS_TO_LOWER(pos) && divider->last_valid) {
    //����������·�����ô������
    if (IS_TO_LEFT(pos) || IS_TO_RIGHT(pos)) {
      obsv.fit_pos = NO_FIT;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> NO_FIT",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    }
    int wxdiff = divider->wx - vnode->wx;
    //�����ǰ�ڵ�͵�ǰ�ֿ�Ŀ��֮��С�ڵ�ǰ�ֿ��ȵ�30%��������֮һ�Ŀ��Ϊ0�Ļ�����������ĵ�ǰ�ֿ������
    if (divider->last_valid
        && (IS_TEXT_VNODE(divider->last_valid->property)
            || divider->last_valid->hpNode->html_tag.tag_type == TAG_BR)
        && IS_TEXT_VNODE(vnode->property)) {
      obsv.fit_pos = FIT_LOWER;
      //obsv.act = AREA_CONTINUE;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_CONTINUE",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    }

    if (abs(wxdiff) * 100 <= divider->wx * 30 || 0 == divider->wx
        || 0 == vnode->wx) {
      obsv.fit_pos = FIT_LOWER;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> FIT_LOWER",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    }
  }
  //ͬ���Ĳ���Ӧ�������������
  if (IS_TO_RIGHT(pos)) {
    if ((IS_TO_LOWER(pos) || IS_TO_UPPER(pos))&&divider->last_valid) {
      obsv.fit_pos = NO_FIT;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> NO_FIT",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    }
    int hxdiff = divider->hx - vnode->hx;
    if (vnode->hx < 40 && divider->hx < 40) {
      obsv.fit_pos = FIT_RIGHT;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> FIT_RIGHT",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    } else if (abs(hxdiff) * 100 <= divider->hx * 30 || 0 == divider->hx
        || 0 == vnode->hx) {
      obsv.fit_pos = FIT_RIGHT;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> FIT_RIGHT",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    }
  } else if (IS_TO_LEFT(pos)) {
    if ((IS_TO_LOWER(pos) || IS_TO_UPPER(pos))&&divider->last_valid) {
      obsv.fit_pos = NO_FIT;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> NO_FIT",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    }
    int hxdiff = divider->hx - vnode->hx;
    if (abs(hxdiff) * 100 <= divider->hx * 30 || 0 == divider->hx
        || 0 == vnode->hx) {
      obsv.fit_pos = FIT_LEFT;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> FIT_LEFT",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    }
  }
  //��ǰ�ڵ��ڵ�ǰ�ֿ�����棬�޷�����
  obsv.fit_pos = NO_FIT;
  debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> NO_FIT", divider->depth,
            vnode->id, vnode->hpNode->html_tag.tag_name);
  return obsv;
}

/**
 * @brief ��С�Ƿ�����Գ�Ϊһ�������ķֿ�.

 * @date 2011/07/05
 **/
static bool is_suite_size(html_vnode_t *vnode, html_area_t *parentArea) {
  if (vnode->wx * 100 >= parentArea->area_info.width * 50) {
    if (vnode->hx >= SMALL_HX_VAL
        && vnode->hx * 100 >= parentArea->area_info.height * 10) {
      return true;
    }
  }

  if (vnode->hx * 100 >= parentArea->area_info.height * 50) {
    if (vnode->wx >= SMALL_HX_VAL
        && vnode->wx * 100 >= parentArea->area_info.width * 10) {
      return true;
    }
  }

  return false;
}

static inline bool is_link(html_vnode_t *vnode) {
  if (vnode->hpNode->html_tag.tag_type
      == TAG_A && get_attribute_value(&vnode->hpNode->html_tag,ATTR_HREF) != NULL) {return true;
}

return false;
}

  /**
   * @brief ��ЩTAGӦ����Ϊһ���ֿ���������.

   * @date 2011/07/05
   **/
static bool is_structure_tag(const html_tag_type_t tag_type) {
  if (tag_type == TAG_ROOT || tag_type == TAG_HTML || tag_type == TAG_BODY
      || tag_type == TAG_TABLE || tag_type == TAG_TBODY || tag_type == TAG_DIV
      || tag_type == TAG_TR || tag_type == TAG_TD || tag_type == TAG_TH
      || tag_type == TAG_COLGROUP || tag_type == TAG_CENTER
      || tag_type == TAG_FORM) {
    return true;
  }

  return false;
}

/**
 * @brief ����ڵ��Ƿ�Ӧ���ڵ�ǰ����Ϊ��ϸ�Ľṹ.

 * @date 2011/07/05
 **/
static bool can_vnode_split(html_vnode_t *vnode) {
  /**
   * ��ǰ�ڵ�Ϊ�ṹ�ͱ�ǩ�������ӽڵ㺬�нṹ�ͱ�ǩ��
   * �Һ��и߶Ƚϴ���ӽڵ㣬
   * ����Բ��Ϊ��ϸ�Ľṹ.
   */
  if (!is_structure_tag(vnode->hpNode->html_tag.tag_type)) {
    return false;
  }
  bool has_struct_tag = false;
  //�жϸýڵ��е��ӽڵ��Ƿ��нṹ�ͱ�ǩ���ṹ�ͱ�ǩ�������Ϊһ���ֿ��������ڣ�
  for (html_vnode_t *child = vnode->firstChild; child; child =
      child->nextNode) {
    if (child->isValid && is_structure_tag(child->hpNode->html_tag.tag_type)) {
      has_struct_tag = true;
      break;
    }
  }
  //������нṹ�ͱ�ǩ������Ҫ�����ӽڵ���������ԣ������ӽڵ����С��10�����и߶ȴ��ڵ�ǰ�ڵ�40%���ӽڵ����
  if (has_struct_tag) {
    bool has_high_child = false;
    int valid_child_num = 0;
    for (html_vnode_t *child = vnode->firstChild; child; child =
        child->nextNode) {
      if (child->isValid) {
        if (child->hx * 100 >= vnode->hx * 40) {
          has_high_child = true;
        }
        if (++valid_child_num >= 10) {
          return false;
        }
      }
    }
    if (has_high_child) {
      return true;
    }
  }
  return false;
}

/*
 static int isMyposPosition(int page_height,int page_width,html_vnode_t *vnode){
 int header_value = 0;
 // it seems small page rule is not applicable after 100 equality sample tes
 if (page_height>=1200){
 //header_value = 600;//
 header_value = page_height*3/7;//shuangwei modify,����	CASEPS-169
 }
 else if(page_height <= 300){
 header_value = page_height / 2;
 }
 else{
 header_value = page_height*3/5;
 }
 if (vnode->ypos<header_value){
 return (vnode->xpos <= page_width/2);
 }
 else{
 return 0;
 }
 }
 */

/**
 * @brief �Ƿ���ҪΪtable
 * @date 2012-10-28
 * @author sue
 */
static bool is_main_table_vnode(html_vnode_t *vnode) {
  if (vnode->hpNode->html_tag.tag_type == TAG_TABLE) {
    return true;
  }
  if (!vnode->firstChild || vnode->firstChild->nextNode
      || vnode->firstChild->hpNode->html_tag.tag_type != TAG_TABLE) {
    return false;
  }
  return true;
}

/**
 * @brief �Ƿ���ҪΪ�б�
 * @date 2012-10-27
 * @author sue
 */
static bool is_main_ul_vnode(html_vnode_t *vnode) {
  if (vnode->hpNode->html_tag.tag_type == TAG_UL) {
    return true;
  }
  if (!vnode->firstChild || vnode->firstChild->nextNode
      || vnode->firstChild->hpNode->html_tag.tag_type != TAG_UL) {
    return false;
  }
  return true;
}

/**
 * @brief �жϽڵ��Ƿ����<div><h[1-4]/><ul/></div>�ṹ
 * @date 2012-10-27
 * @author sue
 */
static bool is_h_ul_vnode(html_vnode_t *vnode) {
  if (vnode->hpNode->html_tag.tag_type != TAG_DIV) {
    return false;
  }
  if (vnode->hx > 200 || vnode->wx > 800) {
    return false;
  }
  if (vnode->subtree_diff_font > 2) {
    return false;
  }
  if (!vnode->firstChild
      || !ahtml_small_header_map[vnode->firstChild->hpNode->html_tag.tag_type]) {
    return false;
  }
  if (!vnode->firstChild->nextNode
      || vnode->firstChild->nextNode->hpNode->html_tag.tag_type != TAG_UL) {
    return false;
  }
  return true;
}

/**
 *  @brief �Ƿ����ĳ���ڵ�
 */
static bool is_contain_tag(html_vnode_t *vnode, html_tag_type_t tag_type) {
  for (html_vnode_t *child = vnode->firstChild; child; child =
      child->nextNode) {
    if (!child->isValid) {
      continue;
    }
    if (child->hpNode->html_tag.tag_type == tag_type) {
      return true;
    }
    bool flag = is_contain_tag(child, tag_type);
    if (flag) {
      return flag;
    }
  }
  return false;
}

/**
 * @brief ��ǰ�ڵ��Ƿ���Ҫ���֣�ע����Ե����ȼ���һ�Ǿ������ַֿ����Ĳ�θУ�����ʹҶ�ӷֿ鲻���ڷ�̫ϸ��

 * @date 2011/07/05
 **/
static bool is_need_divide(html_vnode_t *vnode, html_area_t *pArea,
                           bool is_divid_text, dividing_t *divider) {
  html_tag_type_t tag_type = vnode->hpNode->html_tag.tag_type;
  //Ҷ�ӽڵ��޷����л���
  debuginfo(
      DIVIDING_AREA,
      "[depth:%d] node(id=%d)<%s> ,subnodetype=%x,areawidth=%d,areaheight=%d, divide",
      divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name,
      vnode->hpNode->subnodetype, pArea->area_info.width,
      pArea->area_info.height, __FILE__, __LINE__, __FUNCTION__);
  if (vnode->firstChild == NULL) {
    debuginfo(DIVIDING_AREA,
              "[depth:%d] node(id=%d)<%s> don't have child, not divide",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return false;
  }
  if (vnode->struct_info->valid_child_num <= 2) {  //��ֹ�ֿ��β���
    if (vnode->hpNode->subnodetype & (1 << 2)) {  //�����Ϻ���H��1��6����ǩ���������֣���ֹrealtitle�ֿ����ȹ���
      debuginfo(
          DIVIDING_AREA,
          "[depth:%d] node(id=%d)<%s> ,subnodetype=%x,areawidth=%d,areaheight=%d, to be divide",
          divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name,
          vnode->hpNode->subnodetype, pArea->area_info.width,
          pArea->area_info.height, __FILE__, __LINE__, __FUNCTION__);
      return true;
    }
  }
  if (IS_BORDER(vnode->property) && vnode->subtree_border_num == 1
      && vnode->subtree_diff_font < 3) {  //����border���ԣ��������������Լ�����ֻ���Լ�����border���ԣ�������������Ŀ������2�Ĳ��ټ������� (sue)
    if (vnode->hx < 400 && vnode->wx < 800) {
      debuginfo(DIVIDING_AREA,
                "[depth:%d] node(id=%d)<%s> not divide for it has border",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return false;
    }
  }
  if (vnode->wx * 2 < vnode->vtree->root->wx
      && (is_h_ul_vnode(vnode) || is_main_ul_vnode(vnode))) {  //��ֹ���ֵ�̫ϸ��
    debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> not divide",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return false;
  }
  if (vnode->subtree_textSize < 300 && is_main_table_vnode(vnode)
      && !is_contain_tag(vnode, TAG_TABLE) && divider->depth > 2
      && vnode->hx < 400) {  //��ֹtable���ֵ�̫ϸ��
    if (tag_type == TAG_TABLE) {
      if (vnode->subtree_diff_font > 1) {
        debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> divide",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
        return true;
      }
    }
    debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> not divide",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return false;
  }
  //��ֹ�ظ����ϸ��
  char *class_value = get_attribute_value(&vnode->hpNode->html_tag, ATTR_CLASS);
  char *id_value = get_attribute_value(&vnode->hpNode->html_tag, ATTR_ID);
  if (vnode->struct_info->interaction_tag_num > 1
      && (class_value != NULL || id_value != NULL)) {
    bool flag = false;
    if (class_value && strstr(class_value, "comment")) {
      flag = true;
    }
    if (!flag && id_value && strstr(id_value, "comment")) {
      flag = true;
    }
    if (flag) {
      vnode->type = TYPE_INTERACTION;
      debuginfo(
          DIVIDING_AREA,
          "[depth:%d] node(id=%d)<%s> not divide for it is interaction node",
          divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return false;
    }
  }
  if (divider->depth == 1 && vnode->hpNode->html_tag.tag_type == TAG_TABLE
      && vnode->upperNode
      && vnode->upperNode->hpNode->html_tag.tag_type == TAG_BODY) {
    if (vnode->hx < vnode->upperNode->hx - 20
        || vnode->wx < vnode->upperNode->wx - 20) {
      //��һ��ֿ�ʱ������table��ǩ��Ϊ���ǲ����õģ���Ҫ������
      debuginfo(DIVIDING_AREA,
                "[depth:%d] node(id=%d)<%s> is table of root, not divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return false;
    }
  }
  if (divider->depth > 2) {
    //���������realtitle�飬�ֿ���������ҪϸһЩ
    if (vnode->ypos >= 50 && vnode->ypos <= 360 && vnode->hx < 250
        && vnode->hx >= 30 && vnode->wx >= 250) {
      if (vnode->hx < 50 && vnode->hpNode->html_tag.tag_type == TAG_TD
          && vnode->subtree_diff_font == 1) {
        debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> not divide",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
        return false;
      }
      bool flag = false;
      if (vnode->hpNode->html_tag.tag_type == TAG_P
          && vnode->subtree_diff_font <= 2) {
        flag = true;
      }
      if (!flag) {
        debuginfo(
            DIVIDING_AREA,
            "[depth:%d] node(id=%d)<%s> divide for it maybe realtitle area",
            divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
        return true;
      }
    }
    if (vnode->struct_info->hr_num > 0) {
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return true;
    }
    if (vnode->hpNode->html_tag.tag_type == TAG_TABLE && divider->depth > 1
        && vnode->subtree_textSize > 400) {
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return true;
    }
    if (vnode->subtree_diff_font > 1 && divider->depth > 1) {
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return true;
    }
  }
  if (divider->depth == 1) {
    //��һ���м���ֹ����̫��
    int page_width = vnode->vtree->root->wx;
    if (vnode->hx * 5 > page_width * 2 && vnode->wx > 900) {
      debuginfo(DIVIDING_AREA,
                "[depth:%d] node(id=%d)<%s> divide for it is too big",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return true;
    }
  }
  //��ֹ<h1>�ڵ��ϸ��
  int child_num = 0;
  for (html_vnode_t *child = vnode->firstChild; child; child =
      child->nextNode) {
    child_num++;
  }
  if (vnode->hpNode->html_tag.tag_type == TAG_H1 && child_num <= 3
      && vnode->subtree_diff_font == 1) {
    debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> H1 not divide",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return false;
  }
  //�ı��ڵ����軮��
  if (!is_divid_text && IS_TEXT_VNODE(vnode->property)) {
    debuginfo(DIVIDING_AREA,
              "[depth:%d] node(id=%d)<%s> is text node, not divide",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return false;
  }
  //���Ӻͻ�ζ����������軮��
  if (is_link(vnode) || vnode->hpNode->html_tag.tag_type == TAG_MARQUEE) {
    debuginfo(DIVIDING_AREA,
              "[depth:%d] node(id=%d)<%s> is marrquee node, not divide",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return false;
  }
  //textarea�ڵ����軮��
  if (vnode->hpNode->html_tag.tag_type == TAG_TEXTAREA) {
    debuginfo(DIVIDING_AREA,
              "[depth:%d] node(id=%d)<%s> is textarea node, not divide",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return false;
  }
  //shuangwei add 20120605
  if ((vnode->hpNode->html_tag.tag_type == TAG_TABLE)
      && IS_DOMTREE_SUBTYPE(vnode->hpNode->owner->treeAttr)
      && !(vnode->hpNode->subnodetype & 31)) {
    int colNum = 0;
    html_vnode_t *ptdNode = vnode;
    if (ptdNode && ptdNode->firstChild
        && ptdNode->firstChild->hpNode->html_tag.tag_type == TAG_THEAD) {
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> not divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return false;
    }
    while (ptdNode && ptdNode->firstChild) {
      if (ptdNode->firstChild->hpNode->html_tag.tag_type == TAG_TD) {
        ptdNode = ptdNode->firstChild;
        break;
      }
      ptdNode = ptdNode->firstChild;
    }
    if (ptdNode) {
      char *pcolspan = html_node_get_attribute_value(ptdNode->hpNode,
                                                     "colspan");
      if (pcolspan
          && (((*pcolspan) > '1' && (*pcolspan) <= '9') || strlen(pcolspan) > 1)) {
        debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> not divide",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
        return false;
      }
    }
    while (ptdNode && ptdNode->hpNode->html_tag.tag_type == TAG_TD) {
      colNum++;
      ptdNode = ptdNode->nextNode;
    }
    if (colNum > 1) {
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> not divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return false;
    }
  }
  if ((vnode->hpNode->html_tag.tag_type == TAG_UL
      || vnode->hpNode->html_tag.tag_type == TAG_OL)
      && IS_DOMTREE_SUBTYPE(vnode->hpNode->owner->treeAttr)
      && !(vnode->hpNode->subnodetype & 15)) {
    debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> not divide",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return false;
  }
  if (vnode->hpNode->html_tag.tag_type == TAG_DL
      && vnode->subtree_diff_font == 1) {
    debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> not divide",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return false;
  }
  if ((vnode->hpNode->html_tag.tag_type == TAG_FORM) && vnode->wx < 400
      && vnode->hx < 400) {
    debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> not divide",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return false;
  }
  if (vnode->subtree_max_font_size >= 20 && vnode->subtree_diff_font == 2) {
    char *class_value = get_attribute_value(&vnode->hpNode->html_tag,
                                            ATTR_CLASS);
    if (class_value && strstr(class_value, "title")) {
      debuginfo(DIVIDING_AREA,
                "[depth:%d] node(id=%d)<%s> divide for it maybe realtitle",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return true;
    }
  }
  if (vnode->hpNode->html_tag.tag_type == TAG_P) {
    if (vnode->subtree_max_font_size >= 20 && vnode->subtree_diff_font == 2) {
      debuginfo(DIVIDING_AREA,
                "[depth:%d] node(id=%d)<%s> divide for it maybe realtitle",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return true;
    }
    if (IS_DOMTREE_SUBTYPE(vnode->hpNode->owner->treeAttr)
        && !(vnode->hpNode->subnodetype & 31)
        && (vnode->hpNode->owner->treeAttr & 5) != 0) {
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> not divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return false;
    }
  }
  if (tag_type == TAG_TABLE) {
    if (divider->depth > 1 && vnode->subtree_diff_font > 1) {
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return true;
    }
    if (vnode->wx <= 600 && vnode->hx <= 200 && vnode->prevNode
        && vnode->nextNode && (vnode->ypos > 350 || vnode->ypos < 80)) {
      debuginfo(DIVIDING_AREA,
                "[depth:%d] node(id=%d)<%s> is table, not divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return false;
    }
  }
  //�ڵ�ĳ���͸��ֿ�һ����С����ýڵ���Ҫ����
  if (vnode->wx == pArea->area_info.width
      && vnode->hx == pArea->area_info.height
      && IS_DOMTREE_SUBTYPE(vnode->hpNode->owner->treeAttr)
      && (vnode->hpNode->subnodetype & 127)) {
    bool flag = false;  //�Ƿ����ò���

    html_tag_type_t tag_type = vnode->hpNode->html_tag.tag_type;
    if ((tag_type == TAG_TABLE || tag_type == TAG_DIV)
        && !(vnode->hpNode->subnodetype & 32) && vnode->hx < 500
        && vnode->wx < 500) {  //�����л����һЩ��񣬲�ϣ�������ı�񱻷�ϸ�ˡ������ڲ����õı���е�����Ҫ�������ֵ�
      flag = true;
    }
    if (!flag) {
      debuginfo(DIVIDING_AREA,
                "[depth:%d] node(id=%d)<%s> is equal area, divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return true;
    }
  }
  //����ýڵ������������ṹ��һ���ظ��ṹ����ô�ýڵ����뻮��
  unsigned int limit = 1;
  if (vnode->hx > 600) {
    limit = 2;
  }
  if (divider->depth > limit) {
    if (vnode->struct_info && vnode->struct_info->is_self_repeat
        && vnode->struct_info->self_similar_value > 90) {
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> not divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return false;
    }
  }
  //�����ǰ�ڵ�ĳ����뵱ǰ�ֿ�ı������󣬻��߽ڵ㱾���������
  if (vnode->wx * 100 > pArea->area_info.width * WX_PERCENT_BE_DIVIDE
      && vnode->hx * 100 > pArea->area_info.height * HX_PERCENT_BE_DIVIDE
      && vnode->wx >= 400 && vnode->hx >= 200 && can_vnode_split(vnode)) {
    if (vnode->hpNode->html_tag.tag_type != TAG_TABLE) {
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return true;
    }
  }
  if (vnode->hx < 800) {
    if (vnode->subtree_diff_font > 1 && divider->depth > 1
        && !vnode->struct_info->is_self_repeat
        && !vnode->struct_info->is_repeat_with_sibling) {
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return true;
    }
  }
  if (vnode->hx < 60 && vnode->wx > 600 && vnode->subtree_diff_font > 1) {
    debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> divide",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return true;
  }
//	if (vnode->struct_info->valid_child_num > 1 && vnode->hx > 30)
//	{
//		debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> divide", divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
//		return true;
//	}
//	if(divider->depth == 2 && vnode->hx > 600 && vnode->wx > 250)
//	{
//		debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> divide", divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
//		return true;
//	}

  //�����wap��ҳ��ֻҪ�߶ȹ��ߣ�����Ҫ��������(sue)
  if (divider->wap_page == true) {
    if (vnode->hx > 30) {
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> divide",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return true;
    }
  }

  debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> not divide by default",
            divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
  return false;
}

/**
 * @brief	�жϷֿ��Ƿ����ı��ڵ㹹��;false:�ÿ鲻���ı��飬���п��ͽڵ�

 * @date 2011/07/05
 **/
static bool is_divide_text_block(html_area_t *area) {
  html_vnode_t *vnode = area->begin;
  if (vnode->hpNode->html_tag.tag_type == TAG_BR) {
    vnode = vnode->nextNode;
  }
  //�����÷ֿ��еĿ��ӽڵ㣬һ�����ַ��ı��Ķ��Ҳ���BR�ڵ�ʱ���Ϳ����϶��÷ֿ鲻�����ı��ڵ㹹��
  for (; vnode; vnode = vnode->nextNode) {
    if (vnode->isValid) {
      if (!IS_TEXT_VNODE(vnode->property)
          && (vnode != area->end || vnode->hpNode->html_tag.tag_type != TAG_BR))
        return false;
    }
    if (vnode == area->end) {
      break;
    }
  }
  return true;
}

static bool is_divisible_by_config(html_vnode_t *vnode,
                                   const area_config_t *config) {
  if (config && config->indivisible_tag_name && vnode->hpNode->html_tag.tag_name
      && strcasecmp(vnode->hpNode->html_tag.tag_name,
                    config->indivisible_tag_name) == 0) {
    return false;
  }

  return true;
}

/**
 * @brief ���ݵ�ǰ�ڵ��жϽ���ȡ���ж�.

 * @date 2011/07/05
 **/
static observer_t check_area(html_vnode_t *vnode, dividing_t *divider) {
  observer_t obsv = { 0, 0 };
  html_area_t *pArea = divider->parent_area;
  bool is_divid_text = is_divide_text_block(pArea);
  //�жϸýڵ��Ƿ���Ա�����
  if (is_divisible_by_config(vnode, divider->config)
      && is_need_divide(vnode, pArea, is_divid_text, divider)) {
    obsv.fit_pos = NO_FIT;
    obsv.act = AREA_TOBE_DIVIDE;
    return obsv;
  }
  //�����root�ڵ�(sue)
  html_tag_type_t tag_type = vnode->hpNode->html_tag.tag_type;
  if (tag_type == TAG_ROOT || tag_type == TAG_HTML || tag_type == TAG_BODY
      || tag_type == TAG_WAP_CARD) {
    obsv.fit_pos = NO_FIT;
    obsv.act = AREA_TOBE_DIVIDE;
    return obsv;
  }
  //�жϵ�ǰ�ڵ��뵱ǰ�ֿ������̶�
  obsv = check_fit_prev(vnode, divider);
  if (obsv.act != 0) {
    return obsv;
  }
  obsv.act = 0;
  if (divider->last_valid) {
    //���������ڵ㣬���в�ͬ��classֵʱ�������ֵ�ͬһ���ֿ���
    char *class_value = get_attribute_value(&vnode->hpNode->html_tag,
                                            ATTR_CLASS);
    char *last_class_value = get_attribute_value(
        &divider->last_valid->hpNode->html_tag, ATTR_CLASS);
    if (class_value && last_class_value
        && strcmp(class_value, last_class_value) != 0) {
      obsv.act = AREA_NEW_BEGIN;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_NEW_BEGIN",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    }
  }
  if (vnode->hpNode->html_tag.tag_type == TAG_H1) {
    obsv.act = AREA_END_BOTH;
    debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_END_BOTH",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return obsv;
  }
  if (divider->depth == 1 && vnode->hpNode->html_tag.tag_type == TAG_TABLE) {
    obsv.act = AREA_NEW_BEGIN;
    debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_NEW_BEGIN",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return obsv;
  }
  unsigned int depth_limit = 1;
  if (vnode->hx > 600) {
    depth_limit = 2;
  }
  if (divider->depth > depth_limit && vnode->hpNode->html_tag.tag_type != TAG_P
      && vnode->struct_info->is_repeat_with_sibling && vnode->hx < 200) {
    if (!divider->last_valid
        || divider->last_valid->hpNode->html_tag.tag_type
            == vnode->hpNode->html_tag.tag_type) {
      if (divider->last_valid && vnode->hx >= 20
          && vnode->hx != divider->last_valid->hx) {
        obsv.act = AREA_NEW_BEGIN;
        debuginfo(DIVIDING_AREA,
                  "[depth:%d] node(id=%d)<%s> act AREA_NEW_BEGIN",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
        return obsv;
      }
      if (divider->last_valid
          && vnode->subtree_max_font_size
              != divider->last_valid->subtree_max_font_size) {
        obsv.act = AREA_NEW_BEGIN;
        debuginfo(DIVIDING_AREA,
                  "[depth:%d] node(id=%d)<%s> act AREA_NEW_BEGIN",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
        return obsv;
      }
      obsv.act = AREA_CONTINUE;
      debuginfo(
          DIVIDING_AREA,
          "[depth:%d] node(id=%d)<%s> act AREA_CONTINUE for it is repeat with sibling",
          divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    }
  }
  //�����ǰ�ֿ�ĸ��ֿ鲻�����ı��ڵ㹹�ɣ����ҵ�ǰ�ڵ㲻���ı��ڵ�
  if (!is_divid_text && IS_TEXT_VNODE(vnode->property)) {
    //��ǰ�ڵ����һ�����ӽڵ����ı��ڵ㣬��ǰ�ڵ����뵱ǰ�ֿ��У������ռ��ڵ�
    if (divider->last_valid) {
      if (vnode->hpNode->html_tag.tag_type == TAG_FONT) {
        if (divider->last_valid->depth == vnode->depth) {
          if (divider->last_valid->subtree_max_font_size
              != vnode->subtree_max_font_size) {
            obsv.act = AREA_NEW_BEGIN;
            debuginfo(DIVIDING_AREA,
                      "[depth:%d] node(id=%d)<%s> act AREA_NEW_BEGIN",
                      divider->depth, vnode->id,
                      vnode->hpNode->html_tag.tag_name);
            return obsv;
          }
        }
      }
      if (IS_TEXT_VNODE(divider->last_valid->property)) {
        obsv.act = AREA_CONTINUE;
        debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_CONTINUE",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
        return obsv;
      }
    } else {  //�����ǰ�ֿ黹û��һ�����ӽڵ㣬��ô��ǰ�ڵ���Ϊһ���·ֿ�Ŀ�ʼ
      obsv.act = AREA_NEW_BEGIN;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_NEW_BEGIN",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    }
  }
  if (divider->last_valid && is_divid_text
      && (IS_TEXT_VNODE(divider->last_valid->property)
          || divider->last_valid->hpNode->html_tag.tag_type == TAG_BR)
      && IS_TEXT_VNODE(vnode->property)) {
    obsv.act = AREA_CONTINUE;
    debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_CONTINUE",
              divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
    return obsv;
  }
  //�����������
  switch (obsv.fit_pos) {
    case NO_FIT:
      if (vnode->subtree_textSize == 0
          && vnode->hpNode->html_tag.tag_type != TAG_IMG
          && vnode->hpNode->html_tag.tag_type != TAG_DIV) {
        debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_CONTINUE",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
        obsv.act = AREA_CONTINUE;
        return obsv;
      }
      //��������䣬��ô�����ǰ�ڵ��ʺ���Ϊһ�������ֿ�Ļ����γɵ����ֿ飬������Ϊһ���·ֿ�Ŀ�ʼ
      if (vnode->nextNode
          && vnode->nextNode->hpNode->html_tag.tag_type == TAG_BR) {
        obsv.act = AREA_NEW_BEGIN;
        debuginfo(DIVIDING_AREA,
                  "[depth:%d] node(id=%d)<%s> act AREA_NEW_BEGIN",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      } else if (is_suite_size(vnode, pArea)
          && (divider->last_valid || IS_BLOCK_TAG(vnode->property))
          && vnode->hpNode->html_tag.tag_type != TAG_LI) {
        obsv.act = AREA_END_BOTH;
        debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_END_BOTH",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      } else {
        obsv.act = AREA_NEW_BEGIN;
        debuginfo(DIVIDING_AREA,
                  "[depth:%d] node(id=%d)<%s> act AREA_NEW_BEGIN",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      }
      return obsv;
    case FIT_LOWER:
      if (divider->last_valid) {
        //������������ͬ�Ŀ鲻�����ں�
        if (divider->last_valid->subtree_max_font_size
            != vnode->subtree_max_font_size) {
          debuginfo(DIVIDING_AREA,
                    "[depth:%d] node(id=%d)<%s> act AREA_CONTINUE",
                    divider->depth, vnode->id,
                    vnode->hpNode->html_tag.tag_name);
          obsv.act = AREA_NEW_BEGIN;
          return obsv;
        }
        //�������ֳ���Ϊ0�ļ����ں�
        if (vnode->subtree_textSize == 0
            && vnode->hpNode->html_tag.tag_type != TAG_IMG) {
          debuginfo(DIVIDING_AREA,
                    "[depth:%d] node(id=%d)<%s> act AREA_CONTINUE",
                    divider->depth, vnode->id,
                    vnode->hpNode->html_tag.tag_name);
          obsv.act = AREA_CONTINUE;
          return obsv;
        }
        //�Խ������Բ�ͬ�Ŀ鲻�����ں�
        bool last_inter = IS_INCLUDE_INTER(divider->last_valid->property);
        bool this_inter = IS_INCLUDE_INTER(vnode->property);
        if (last_inter != this_inter) {
          debuginfo(DIVIDING_AREA,
                    "[depth:%d] node(id=%d)<%s> act AREA_CONTINUE",
                    divider->depth, vnode->id,
                    vnode->hpNode->html_tag.tag_name);
          obsv.act = AREA_NEW_BEGIN;
          return obsv;
        }
        //�Ѷ������ӷֵ�һ��
        if (divider->last_validword) {
          if (vnode->hx < 200
              && vnode->hpNode->html_tag.tag_type
                  == divider->last_validword->hpNode->html_tag.tag_type) {
            if (vnode->firstChild && !vnode->firstChild->nextNode
                && divider->last_validword->firstChild
                && !divider->last_validword->firstChild->nextNode) {
              if (vnode->firstChild->hpNode->html_tag.tag_type
                  == divider->last_validword->firstChild->hpNode->html_tag
                      .tag_type) {
                debuginfo(DIVIDING_AREA,
                          "[depth:%d] node(id=%d)<%s> act AREA_CONTINUE",
                          divider->depth, vnode->id,
                          vnode->hpNode->html_tag.tag_name);
                obsv.act = AREA_CONTINUE;
                return obsv;
              }
            }
          }
        }
      }
      //�����ǰ�ڵ��ڵ�ǰ�ֿ���·�������߶����������Ļ���֮ǰ�ռ��Ľڵ���Ϊһ���ֿ飬��ǰ�ڵ���Ϊһ���ֿ飬����ǰ�ֿ�����ں�
      if (vnode->hx >= SMALL_HX_VAL
          && vnode->hpNode->html_tag.tag_type != TAG_BR
          && vnode->hpNode->html_tag.tag_type != TAG_PURETEXT) {
        obsv.act = AREA_END_BOTH;
        debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_END_BOTH",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
        return obsv;
      }
      obsv.act = AREA_CONTINUE;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_CONTINUE",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    case FIT_RIGHT:
      if (vnode->hpNode->html_tag.tag_type != TAG_LI) {
        if (vnode->wx >= SMALL_WX_VAL && (IS_BLOCK_TAG(vnode->property))) {
          obsv.act = AREA_END_BOTH;
          debuginfo(DIVIDING_AREA,
                    "[depth:%d] node(id=%d)<%s> act AREA_END_BOTH",
                    divider->depth, vnode->id,
                    vnode->hpNode->html_tag.tag_name);
          return obsv;
        }
        if (vnode->wx >= SMALL_WX_VAL
            && (vnode->hpNode->html_tag.tag_type == TAG_BR)) {
          obsv.act = AREA_END;
          debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_END",
                    divider->depth, vnode->id,
                    vnode->hpNode->html_tag.tag_name);
          return obsv;
        }
      }
      obsv.act = AREA_CONTINUE;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_CONTINUE",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    case FIT_LEFT:
      if (vnode->wx >= SMALL_WX_VAL && (IS_BLOCK_TAG(vnode->property))) {
        obsv.act = AREA_END_BOTH;
        debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_END_BOTH",
                  divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
        return obsv;
      }
      obsv.act = AREA_CONTINUE;
      debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> act AREA_CONTINUE",
                divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
      return obsv;
    default:
      assert(0);
      break;
  }
  return obsv;
}

/**
 * @brief ��ǰ�ڵ�������п���±�.��Ҫ�Ǹ��µ�ǰ���xpos��ypos

 * @date 2011/07/05
 **/
static void block_append_lower(dividing_t *divider, html_vnode_t *vnode) {
  divider->end = vnode;
  divider->hx = vnode->ypos + vnode->hx - divider->ypos;
  int right_xpos = divider->xpos + divider->wx;
  if (right_xpos < vnode->xpos + vnode->wx) {
    right_xpos = vnode->xpos + vnode->wx;
  }
  if (vnode->xpos < divider->xpos) {
    divider->xpos = vnode->xpos;
  }
  divider->wx = right_xpos - divider->xpos;
}

/**
 * @brief ��ǰ�ڵ�������п�����.��Ҫ�Ǹ��µ�ǰ���xpos��ypos

 * @date 2011/07/05
 **/
static void block_append_left(dividing_t *divider, html_vnode_t *vnode) {
  divider->end = vnode;
  divider->wx = divider->xpos + divider->wx - vnode->xpos;
  int bot_ypos = divider->ypos + divider->hx;
  if (bot_ypos < vnode->ypos + vnode->hx)
    bot_ypos = vnode->ypos + vnode->hx;
  if (vnode->ypos < divider->ypos)
    divider->ypos = vnode->ypos;
  divider->hx = bot_ypos - divider->ypos;
  divider->xpos = vnode->xpos;
}

/**
 * @brief	��ǰ�ڵ�������п���ұ�.��Ҫ�Ǹ��µ�ǰ���xpos��ypos

 * @date 2011/07/05
 **/
static void block_append_right(dividing_t *divider, html_vnode_t *vnode) {
  divider->end = vnode;
  divider->wx = vnode->xpos + vnode->wx - divider->xpos;
  int bot_ypos = divider->ypos + divider->hx;
  if (bot_ypos < vnode->ypos + vnode->hx)
    bot_ypos = vnode->ypos + vnode->hx;
  if (vnode->ypos < divider->ypos)
    divider->ypos = vnode->ypos;
  divider->hx = bot_ypos - divider->ypos;
}

/**
 * @brief ��ǰ�ڵ������п���λ���ϲ�����.

 * @date 2011/07/05
 **/
static void block_append_nofit(dividing_t *divider, html_vnode_t *vnode) {
  divider->end = vnode;
  int bot_ypos = divider->ypos + divider->hx;
  int right_xpos = divider->xpos + divider->wx;
  if (vnode->xpos < divider->xpos) {
    divider->xpos = vnode->xpos;
  }
  if (vnode->ypos < divider->ypos) {
    divider->ypos = vnode->ypos;
  }
  if (bot_ypos < vnode->ypos + vnode->hx) {
    bot_ypos = vnode->ypos + vnode->hx;
  }
  divider->hx = bot_ypos - divider->ypos;
  if (right_xpos < vnode->xpos + vnode->wx) {
    right_xpos = vnode->xpos + vnode->wx;
  }
  divider->wx = right_xpos - divider->xpos;
}

/**
 * @brief ��ǰ�ڵ�������зֿ���.

 * @date 2011/07/05
 **/
static void block_append(dividing_t *divider, html_vnode_t *vnode,
                         unsigned int fit_pos) {
  //�÷ֿ�ĿǰΪ�գ�����俪ʼ�ͽ����ڵ�
  if (divider->begin == NULL) {
    divider->begin = vnode;
    divider->end = vnode;
  }
  //���Ŀǰ��û�п��ӽڵ㣬�����ֿ�ĳ������Ϣ�����Ҳ�������ҵĸ��
  if (divider->last_valid == NULL) {
    divider->xpos = vnode->xpos;
    divider->ypos = vnode->ypos;
    divider->wx = vnode->wx;
    divider->hx = vnode->hx;
    divider->last_valid = vnode;
    if (vnode->subtree_textSize > 0) {
      divider->last_validword = vnode;
    }
    return;
  }
  divider->last_valid = vnode;
  if (vnode->subtree_textSize > 0) {
    divider->last_validword = vnode;
  }
  //���µ�ǰ���xpos��ypos
  switch (fit_pos) {
    case FIT_LOWER:
      block_append_lower(divider, vnode);
      break;
    case FIT_LEFT:
      block_append_left(divider, vnode);
      break;
    case FIT_RIGHT:
      block_append_right(divider, vnode);
      break;
    case NO_FIT:
      block_append_nofit(divider, vnode);
      break;
    default:
      ;
  }
}

/**
 * @brief �Ƿ�̫С�ķֿ�,������.

 * @date 2011/07/05
 **/
static bool is_little_block(dividing_t *divider) {
  return false;
}

/**
 * @brief �Ƿ�Ը��ֿ�ɶҲû�ֳ���.

 * @date 2011/07/05
 **/
static bool is_divide_nothing(html_vnode_t *tail_vnode,
                              html_area_t *parentArea) {
  if (parentArea->valid_subArea_num == 0 && parentArea->end == tail_vnode) {
    return true;
  }
  return false;
}

/**
 * @brief �������ռ��ķֿ鴴���ֿ�ڵ�.��������ռ��ķֿ�.

 * @date 2011/07/05
 **/
static int old_collect_pass_away(dividing_t *divider) {
  if (divider->begin != NULL && !is_little_block(divider)
      && !is_divide_nothing(divider->end, divider->parent_area)) {
    html_area_t *aNode = blockToAreaNode(divider);
    if (aNode == NULL) {
      return -1;
    }
  }
  dividing_collect_clr(divider);
  return 1;
}

/**
 * @brief �ռ���Ч�ڵ�.

 * @date 2011/07/05
 static void collect_invalid_vnode(dividing_t *divider, html_vnode_t *vnode)
 {
 if(divider->begin == NULL){
 divider->begin = vnode;
 }
 divider->end = vnode;
 }
 **/

/**
 * @brief	����VTREE�ڵ㲢�ֿ�.

 * @date 2011/07/05
 **/
static int visit_for_divide(html_vnode_t *vnode, void *result) {
  dividing_t *divider = (dividing_t *) result;
  if (!vnode->isValid) {
    return VISIT_SKIP_CHILD;
  }
  //ȷ���ֿ�ľ��ߣ��ò�ȷ����ǰ�ڵ��뵱ǰ�ֿ鵽�����γ�һ���ֿ黹����ô�Ρ�����
  observer_t obsv = check_area(vnode, divider);
  switch (obsv.act) {
    case AREA_CONTINUE: {
      //�����ں�
      block_append(divider, vnode, obsv.fit_pos);
      return VISIT_SKIP_CHILD;
    }
    case AREA_NEW_BEGIN: {
      //�²���һ���ֿ飬�����ɷֿ飬��ʼ�·ֿ�
      if (old_collect_pass_away(divider) == -1) {
        goto FAIL;
      }
      block_append(divider, vnode, obsv.fit_pos);
      return VISIT_SKIP_CHILD;
    }
    case AREA_END_BOTH: {
      //֮ǰ�ռ��Ľ����Ϊһ���ֿ�,��ǰ�ڵ��Ϊһ���ֿ�
      if (old_collect_pass_away(divider) == -1) {
        goto FAIL;
      }
      if (!is_divide_nothing(vnode, divider->parent_area)) {  //don't do worthless divide
        block_append(divider, vnode, obsv.fit_pos);
        html_area_t *aNode = blockToAreaNode(divider);
        if (aNode == NULL) {
          goto FAIL;
        }
      }
      dividing_collect_clr(divider);
      return VISIT_SKIP_CHILD;
    }
    case AREA_END: {
      //�Ե�ǰ�ڵ���Ϊ��ǰ�ֿ�Ľ���
      if (!is_divide_nothing(vnode, divider->parent_area)) {  //don't do worthless divide
        block_append(divider, vnode, obsv.fit_pos);
        html_area_t *aNode = blockToAreaNode(divider);
        if (aNode == NULL) {
          goto FAIL;
        }
      }
      dividing_collect_clr(divider);
      return VISIT_SKIP_CHILD;
    }
    case AREA_TOBE_DIVIDE: {
      //��ǰ�ڵ���Ҫ����
      if (old_collect_pass_away(divider) == -1) {
        goto FAIL;
      }
      return VISIT_NORMAL;
    }
    default:
      assert(0);
      break;
  }
  FAIL: return VISIT_ERROR;
}

/**
 * @brief	������һ������ʱ,���ռ��ķֿ�Ҫô����һ���µķֿ�ڵ�,Ҫô����.
 *

 * @date 2011/07/05
 **/
static int end_visit_for_divide(html_vnode_t *vnode, void *result) {
  if (!vnode->isValid) {
    return VISIT_NORMAL;
  }
  dividing_t *divider = (dividing_t *) result;
  if (divider->end == vnode) {/**���Ǵ���������*/
    return VISIT_NORMAL;
  }
  if (old_collect_pass_away(divider) == -1) {
    return VISIT_ERROR;
  }
  debuginfo(DIVIDING_AREA, "[depth:%d] node(id=%d)<%s> old_collect_pass_away",
            divider->depth, vnode->id, vnode->hpNode->html_tag.tag_name);
  return VISIT_NORMAL;
}

/**
 * @brief ����ֻ��һ����Ч�ֿ�ķֿ��.

 * @date 2011/07/05
 **/
static void cut_single_area_level(html_area_t *area, nodepool_t *np) {
  html_area_t *pArea = area->parentArea;
  html_area_t *validArea = area;
//	for(;!validArea->isValid;validArea=validArea->nextArea);
  html_area_t *prevArea = validArea->prevArea;
  html_area_t *nextArea = validArea->nextArea;

  if (validArea->subArea == NULL) {
    html_area_t *next = NULL;
    for (html_area_t *a = pArea->subArea; a; a = next) {
      next = a->nextArea;
      nodepool_put(np, a);
    }
    pArea->subArea = NULL;
    pArea->subArea_num = 0;
    pArea->valid_subArea_num = 0;
  } else {
    if (prevArea) {
      prevArea->nextArea = validArea->subArea;
      validArea->subArea->prevArea = prevArea;
    }
    html_area_t *lastSubArea = NULL;
    // hang subArea on its parent
    for (html_area_t *subArea = validArea->subArea; subArea;
        subArea = subArea->nextArea) {
      subArea->parentArea = pArea;
      lastSubArea = subArea;
    }
    if (nextArea) {
      lastSubArea->nextArea = nextArea;
      nextArea->prevArea = lastSubArea;
    }
    // update parent area
    pArea->subArea_num += validArea->subArea_num - 1;
    pArea->valid_subArea_num = validArea->valid_subArea_num;
    if (prevArea == NULL) {
      pArea->subArea = validArea->subArea;
    }
    nodepool_put(np, validArea);
  }
}

/**
 * @brief ������Ч�ֿ飬����˵�����ֿ�.

 * @date 2011/07/05
 **/
static void cut_invalid_level(html_area_t *area, nodepool_t *np) {
  html_area_t *pArea = area->parentArea;
  html_area_t *next = NULL;
  for (html_area_t *a = pArea->subArea; a; a = next) {
    next = a->nextArea;
    nodepool_put(np, a);
  }
  pArea->subArea = NULL;
  pArea->subArea_num = 0;
  pArea->valid_subArea_num = 0;
}

/**
 * @brief ��ǰ�ֿ��Ƿ��ڷֿ��������õķ�Χ��.

 * @date 2011/07/05
 **/
static inline bool is_in_limit(const html_area_t *area, unsigned int depth,
                               const area_config_t *cfg) {
  int awx = area->area_info.width;
  int ahx = area->area_info.height;
  if (awx < cfg->min_width) {
    return false;
  }
  if (ahx < cfg->min_height) {
    return false;
  }
  if (depth > cfg->max_depth) {
    return false;
  }
  if (awx < BIG_SHORT_INT && ahx < BIG_SHORT_INT && awx * ahx < cfg->min_size) {
    return false;
  }
  return true;
}

/**
 * @brief ��ǰ�ֿ��Ƿ�ֻ��һ����Ч�ӷֿ飬���Ҳ��ǵ�һ��ֿ飬����������֣�˵���ò�ֿ����û������

 * @date 2011/07/05
 **/
static bool is_to_cut_valid_level(html_area_t *area) {
  if (area->valid_subArea_num == 1 && area->depth >= 1) {
    return true;
  }
  return false;
}
/**
 * @brief У׼area��size.

 * @date 2011/07/05
 **/
static void adjust_area_size(html_area_t *area) {
  int most_left_x = area->area_info.xpos + area->area_info.width;
  int most_right_x = area->area_info.xpos;

  for (html_area_t *subarea = area->subArea; subarea;
      subarea = subarea->nextArea) {
    if (!subarea->isValid) {
      continue;
    }
    int leftsidex = subarea->area_info.xpos;
    int rightsidex = subarea->area_info.xpos + subarea->area_info.width;

    if (leftsidex < most_left_x) {
      most_left_x = leftsidex;
    }

    if (rightsidex > most_right_x) {
      most_right_x = rightsidex;
    }
  }
  int sub_xspan = most_right_x - most_left_x;
  if (sub_xspan > 0 && sub_xspan < area->area_info.width) {
    area->area_info.width = sub_xspan;
  }
}

/**
 * @brief ����һ���ֿ�.
 * @param [in] area   :  html_area_t*	�����ֵķֿ�.
 * @param [in] cfg   : const area_config_t*	�ֿ����������.
 * @param [in/out] np   : nodepool_t*	�ֿ�ڵ��.
 * @param [in] depth   : unsigned int	�����ֵķֿ�����.
 * @return  int 
 * @retval   -1:�ֿ����.1:�ɹ�.

 * @date 2011/07/05
 **/
int areaNode_divide(html_area_t *area, const area_config_t *cfg, nodepool_t *np,
                    unsigned int depth) {
  /*�����α꣬�����г�ʼ��*/
  dividing_t divider;
  dividing_clr(&divider);
  divider.atree = area->area_tree;
  divider.depth = depth;
  divider.parent_area = area;
  divider.cur_tail = &(area->subArea);
  divider.np = np;
  divider.config = cfg;
  int doc_type = area->begin->vtree->hpTree->doctype;
  if (doc_type == 1 || doc_type == 2)
    divider.wap_page = true;

  unsigned int new_depth = 0;
  //���ֶα�ʶĳ���ֲ��Ƿ���Ҫȥ��
  bool is_cut_vlevel = false;
  //�жϵ�ǰ�ֿ��Ƿ��б�Ҫ�ٷ֣���Ҫ�ǿ�ȡ��߶ȡ���ȼ�������
  if (cfg != NULL && !is_in_limit(area, depth, cfg)) {
    return 1;
  }
  if (area->begin != area->end && depth > 3) {
    debuginfo(DIVIDING_AREA, "[depth:%d] area(node id=%d) not divide",
              area->depth, area->begin->id);
    return 1;
  }

  //���ֻ��һ�������ֵ�Ҷ�ӽڵ㣬���ٻ���
  int for_partition_area_num = 0;
  html_vnode_t *for_partition_area = NULL;
  for (html_vnode_t *vnode = area->begin; vnode; vnode = vnode->nextNode) {
    if (vnode->textSize > 0) {
      for_partition_area_num++;
      for_partition_area = vnode;
    }
    if (vnode == area->end) {
      break;
    }
  }
  if (for_partition_area_num <= 1) {
    if (for_partition_area
        && for_partition_area->struct_info->valid_leaf_num == 1) {
      debuginfo(DIVIDING_AREA, "[depth:%d] area(node id=%d) not divide",
                area->depth, area->begin->id);
      return 1;
    }
  }

  if (area->begin == area->end) {
    html_vnode_t *vnode = area->begin;
    char *class_value = get_attribute_value(&vnode->hpNode->html_tag,
                                            ATTR_CLASS);
    char *id_value = get_attribute_value(&vnode->hpNode->html_tag, ATTR_ID);
    int hx = vnode->hx;
    int wx = vnode->wx;
    if (class_value
        && (strstr(class_value, "copyright") || strstr(class_value, "footer")
            || strstr(class_value, "discuss"))) {  //�����ǰ�Ȩ�ķֿ鲻��ϸ��
      debuginfo(DIVIDING_AREA, "[depth:%d] area(node id=%d) not divide",
                area->depth, area->begin->id);
      return 1;
    }
    if (depth > 2) {
      if (hx <= 50 && (class_value != NULL || id_value != NULL)) {  //�߶���һ����Χ�ڣ��Ҿ���class��id���ԵĲ��ټ�������
        debuginfo(DIVIDING_AREA, "[depth:%d] area(node id=%d) not divide",
                  area->depth, area->begin->id);
        return 1;
      }
    }
    if (depth > 2) {
      if (vnode->struct_info->is_self_repeat
          && vnode->struct_info->self_similar_value >= 96) {  //�ظ����ṹ���ټ�������
        debuginfo(
            DIVIDING_AREA,
            "[depth:%d] area(node id=%d) not divide for it is self repeat area",
            area->depth, area->begin->id);
        return 1;
      }
    }
    unsigned int subtree_node_num = 0;
    for (html_vnode_t *vnode = area->begin; vnode; vnode = vnode->nextNode) {
      subtree_node_num += vnode->struct_info->valid_node_num;
      if (vnode == area->end) {
        break;
      }
    }
    if (!(vnode->hpNode->subnodetype && (1 << 5))) {  //������form�ڵ�
      if (vnode->hpNode->html_tag.tag_type == TAG_TABLE && wx >= 700 && hx <= 40
          && subtree_node_num < 100) {  //������״��table�ڵ㣬�Ҳ�����form�ڵ�ģ�����ϸ�� (sue)
        debuginfo(
            DIVIDING_AREA,
            "[depth:%d] area(node id=%d) not divide for it is like navigation",
            area->depth, area->begin->id);
        return 1;
      }
    }
  }
  // ����vtree��ȡ�ֿ�
  for (html_vnode_t *vnode = area->begin; vnode; vnode = vnode->nextNode) {
    int ret = html_vnode_visit(vnode, visit_for_divide, end_visit_for_divide,
                               &divider);
    if (ret == VISIT_ERROR) {
      goto FAIL;
    }
    if (vnode == area->end) {
      if (area->valid_subArea_num > 0
          && old_collect_pass_away(&divider) == -1) {
        goto FAIL;
      }
      break;
    }
  }
  new_depth = depth + 1;
  is_cut_vlevel = is_to_cut_valid_level(area);
  //������Ч�ֿ�����1����������
  if (is_cut_vlevel
      || (area->valid_subArea_num == 0 && area->subArea_num > 0)) {
    new_depth--;
  }
  // ���������ӷֿ�
  for (html_area_t *subArea = area->subArea; subArea;
      subArea = subArea->nextArea) {
    if (!subArea->isValid) {
      continue;
    }
    int ret = areaNode_divide(subArea, cfg, np, new_depth);
    if (ret == VISIT_ERROR) {
      goto FAIL;
    }
  }
  if (is_cut_vlevel) {
    //����ֻ��һ����Ч�ֿ�Ĳ�
    //������ͼ�õ�һ��ֿ�չʾ��ҳ�ṹ,�Ե�һ��ֿ鲻���ж���.
    cut_single_area_level(area->subArea, np);
  } else if (area->valid_subArea_num == 0 && area->subArea_num > 0) {  //û����Ч�ֿ�Ĳ�
    cut_invalid_level(area->subArea, np);
  }
  if (area->depth == 0) {
    adjust_area_size(area);
  }
  return 1;

  FAIL: return -1;
}

/**
 * @brief ��ȡ���ֿ�,��ҳ��������Ŀ��.

 * @date 2011/07/05
 **/
static int real_page_width(const html_area_t *root) {
  if (root->subArea == NULL) {
    return root->area_info.width;
  }
  int wx = 0;
  for (html_area_t *a = root->subArea; a; a = a->nextArea) {
    int tmp = a->area_info.xpos + a->area_info.width;
    if (tmp > wx) {
      wx = tmp;
    }
  }
  return wx;
}

static int visit_for_add_no(html_area_t *area, void *data) {
  unsigned int *no = (unsigned int *) data;
  area->no = (*no)++;
  if (area->isValid
      && area->depth >= (unsigned int) area->area_tree->max_depth) {
    area->area_tree->max_depth = area->depth;
  }
  return AREA_VISIT_NORMAL;
}

static int visit_vnode_for_add_vnode2area(html_vnode_t *vnode, void *data) {
  /**
   * ��һ��area�ڲ���������vnode������Ӧ����С�ֿ顣
   * ���ѱ���䣬˵�����Ѷ�Ӧ����С�ķֿ飬����������������ı�����
   */
  html_area_t *area = (html_area_t *) data;
  if (vnode->hp_area == NULL) {
    vnode->hp_area = area;
    //SET_LEAF_AREA(area->areaattr);
  } else {
    return VISIT_SKIP_CHILD;
  }
  return AREA_VISIT_NORMAL;
}

static int finish_visit_for_add_vnode2area(html_area_t *area, void *data) {
  for (html_vnode_t *vnode = area->begin;; vnode = vnode->nextNode) {
    if (IS_DOMTREE_SUBTYPE(area->area_tree->hp_vtree->hpTree->treeAttr)) {
      switch (vnode->hpNode->html_tag.tag_type) {
        case TAG_P:
          MARK_DOMTREE_P_TAG(area->nodeTypeOfArea);
          break;
        case TAG_DIV:
          MARK_DOMTREE_DIV_TAG(area->nodeTypeOfArea);
          break;
        case TAG_TABLE:
          MARK_DOMTREE_TABLE_TAG(area->nodeTypeOfArea);
          break;
        case TAG_H1:
        case TAG_H2:
        case TAG_H3:
        case TAG_H4:
        case TAG_H5:
        case TAG_H6:
          MARK_DOMTREE_H_TAG(area->nodeTypeOfArea);
          break;
        case TAG_UL:
        case TAG_OL:
          MARK_DOMTREE_LIST_TAG(area->nodeTypeOfArea);
          break;
        case TAG_FORM:
          MARK_DOMTREE_FORM_TAG(area->nodeTypeOfArea);
          break;
        default:
          break;
      }
      area->nodeTypeOfArea = area->nodeTypeOfArea | vnode->hpNode->subnodetype;
    }
    html_vnode_visit(vnode, visit_vnode_for_add_vnode2area, NULL, area);
    if (vnode == area->end) {
      break;
    }
  }
  return AREA_VISIT_NORMAL;
}

int area_partition(area_tree_t *atree, html_vtree_t *vtree,
                   const char *base_url) {
  debuginfo_on (DIVIDING_AREA);
  timeinit();
  timestart();

  unsigned int no = 0;
  int ret, doc_type;
  area_tree_clean(atree);
  /** add repeat struct info */
  vhtml_struct_prof(vtree);

  //��ʼ�����ڵ㡢hp_vtree������
  atree->root = createAreaNode(&atree->np);
  if (atree->root == NULL) {
    goto FAIL;
  }

  //ȷ���ĵ�����
  determine_doctype(vtree->hpTree, base_url);
  //���ҳ���Ⱥ�խ��Ҳ����wapҳ�洦��(sue)
  doc_type = vtree->hpTree->doctype;
  if (doc_type != 1 && doc_type != 2 && vtree->root->wx < 250) {
    vtree->hpTree->doctype = doctype_xhtml_MP;
  }

  atree->hp_vtree = vtree;
  atree->root->area_tree = atree;
  atree->root->begin = vtree->root;
  atree->root->end = vtree->root;
  atree->root->area_info.xpos = vtree->root->xpos;
  atree->root->area_info.ypos = vtree->root->ypos;
  atree->root->area_info.width = vtree->root->wx;
  atree->root->area_info.height = vtree->root->hx;
  atree->root->depth = 0;

  ret = areaNode_divide(atree->root, &atree->config, &atree->np, 1);
  if (ret == -1) {
    goto FAIL;
  }
  // У׼ҳ����
  atree->root->area_info.width = real_page_width(atree->root);
  // Ϊÿ��area���uid�����Ҹ���vnodeָ��area��ָ��
  areatree_visit(atree, visit_for_add_no, finish_visit_for_add_vnode2area, &no);
  atree->area_num = no;

  timeend("area", "");
  dumpdebug(DIVIDING_AREA, DIVIDING_AREA);
  return 1;
  FAIL: timeend("area", "");
  dumpdebug(DIVIDING_AREA, DIVIDING_AREA);
  return -1;
}

/**
 * @brief �ֿ����ı���������ͬ html_tree_visit ����
 * @param [in/out] atree   :  area_tree_t*
 * @param [in/out] start   : FUNC_START_T
 * @param [in/out] finish   : FUNC_FINISH_T
 * @param [in/out] data   : void*
 * @return  bool 
 * @retval   �ɹ� true ��ʧ��false
 * @see 

 * @date 2011/07/05
 **/
bool areatree_visit(area_tree_t * atree, FUNC_START_T start,
                    FUNC_FINISH_T finish, void * data) {
  int ret = areatree_visit(atree->root, start, finish, data);
  if (ret == AREA_VISIT_ERR) {
    return false;
  }
  return true;
}

/**
 * @brief	�ֿ��������ķǵݹ�ʵ��.

 * @date 2011/07/05
 **/
int areatree_visit(html_area_t * area, FUNC_START_T start, FUNC_FINISH_T finish,
                   void * data) {
  int ret = AREA_VISIT_NORMAL;
  html_area_t *iter = area;

  while (iter) {
    int local_ret = AREA_VISIT_NORMAL;
    if (start != NULL) {
      local_ret = start(iter, data);
      if (local_ret == AREA_VISIT_ERR || local_ret == AREA_VISIT_FINISH) {
        ret = local_ret;
        goto FINISH;
      }
    }
    if (iter->subArea && local_ret != AREA_VISIT_SKIP) {
      iter = iter->subArea;
      continue;
    }

    /**
     * ����ýڵ�ΪҶ�ڵ��AREA_VISIT_SKIP��
     * �����������finish_visit()
     */
    if (finish != NULL) {
      local_ret = finish(iter, data);
      if (local_ret == AREA_VISIT_ERR || local_ret == AREA_VISIT_FINISH) {
        ret = local_ret;
        goto FINISH;
      }
      assert(local_ret == AREA_VISIT_NORMAL);
    }
    if (iter == area) {
      goto FINISH;
    }

    /**����
     */
    if (iter->nextArea) {
      iter = iter->nextArea;
      continue;
    }

    /**����
     */
    while (1) {
      iter = iter->parentArea;
      if (iter == NULL) {
        break;
      }
      if (finish != NULL) {
        local_ret = (*finish)(iter, data);
        if (local_ret == AREA_VISIT_ERR || local_ret == AREA_VISIT_FINISH) {
          ret = local_ret;
          goto FINISH;
        }
        assert(local_ret == AREA_VISIT_NORMAL);
      }
      if (iter == area) {
        goto FINISH;
      }

      /**����
       */
      if (iter->nextArea) {
        iter = iter->nextArea;
        break;
      }
    }
  }

  FINISH: return ret;
}
void printArea(html_area_t * area, int level) {
}
void printAtree(area_tree_t * atree) {
  if (atree) {
    printArea(atree->root, 0);
  }

}
void printSingleArea(html_area_t * area) {
}

