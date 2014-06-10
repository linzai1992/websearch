#ifndef __EASOU_VHTML_UTILS_H_
#define __EASOU_VHTML_UTILS_H_

/**
 * @brief print vtree
 * @return д�뵽buf�ĳ���
 */
int print_vtree_html(html_vtree_t *html_vtree, char* buf, int size, const char* base);

/**
 * @brief �ݹ��ӡ��ǰ�����Ľڵ���Ϣ
 * @return д�뵽buf�ĳ���
 */
int print_vnode_html(html_vnode_t *html_vnode, char* buf, int size, int& avail, const char* base);

/**
 * @brief print vtree
 * @return д�뵽buf�ĳ���
 */
int print_vtree(html_vtree_t *html_vtree, char* buf, int size);

/**
 * @brief �ݹ��ӡ��ǰ�����Ľڵ���Ϣ
 * @return д�뵽buf�ĳ���
 */
int print_vnode(html_vnode_t *html_vnode, char* buf, int size, int& avail);

/**
 * @brief ��ӡ����V���ڵ���Ϣ
 * @param [in] vnode,
 * @param [in/out] p, ����
 * @param [in] bufLen, p�ĳ���
 * @param [in] space_len, ��ӡ�ո���
 * @param [in] type, 2������3����
 * @return д�뵽p�ĳ���
 */
int print_vnode_info(html_vnode_t *vnode, char *p, int bufLen, int space_len, int type);

#endif
