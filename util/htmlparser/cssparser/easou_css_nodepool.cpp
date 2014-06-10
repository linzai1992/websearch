/**
 * @file easou_css_nodepool.cpp
 * @author xunwu
 * @date 2011/06/21
 * @version 1.0
 * @brief	CSS�ڵ�������.
 *
 **/
#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "easou_css_nodepool.h"

using namespace EA_COMMON;

/**
 * @brief �����µ��ڴ����ڵ�.
 * @param [in] size   : size_t	���ڴ���С.
 * @return  css_mem_node_t*	�µ��ڴ����ڵ�.
 * @retval	ʧ�ܷ���NULL.
 * @author xunwu
 * @date 2011/06/20
 **/
static easou_css_mem_node_t *css_new_mem_node(size_t size)
{
	/* alloc mem node */
	easou_css_mem_node_t *pnode = (easou_css_mem_node_t *) calloc(sizeof(easou_css_mem_node_t), 1);
	if (NULL == pnode)
	{
		Fatal((char*) "%s:%s:malloc error!", __FILE__, __FUNCTION__);
		goto FAIL_CNMN;
	}
	/* alloc mem */
	pnode->p_mem = calloc(size, 1);
	if (NULL == pnode->p_mem)
	{
		Fatal((char*) "%s:%s:malloc error!", __FILE__, __FUNCTION__);
		goto FAIL_CNMN;
	}
	pnode->mem_size = size;
	pnode->next = NULL;
	return pnode;

	FAIL_CNMN: if (pnode != NULL)
	{
		if (pnode->p_mem != NULL)
		{
			free(pnode->p_mem);
			pnode->p_mem = NULL;
		}
		free(pnode);
		pnode = NULL;
	}
	return NULL;
}

/**
 * @brief give required size of mem to nodepool.
 * @param [in] pool   : css_nodepool_t*	�ڵ�����.
 * @param [in] size   : size_t	���ڴ��С.
 * @return  int
 * @retval  -1:ʧ��;1:�ɹ�.
 * @author xunwu
 * @date 2011/06/20
 **/
static int css_nodepool_request(easou_css_nodepool_t *pool, size_t size)
{
	/* alloc mem node */
	easou_css_mem_node_t *pnode = css_new_mem_node(size);
	if (NULL == pnode)
	{
		return -1;
	}
	/* set nodepool */
	pnode->next = pool->mem_node_list;
	pool->mem_node_list = pnode;
	pool->p_curr_mem = pnode->p_mem;
	pool->p_curr_mem_size = pnode->mem_size;
	pool->p_pool_avail = pnode->p_mem;
	return 1;
}

/**
 * @brief	init nodepool
 * @retval   success: return 1; fail: return -1.
 * @author xunwu
 * @date 2011/11/10
 **/
int css_nodepool_init(easou_css_nodepool_t *pool, size_t size)
{
	pool->mem_node_list = NULL;
	return css_nodepool_request(pool, size);
}

/**
 * @brief reset the nodepool to have only one memery node and reset the pool as never used
 * @author xunwu
 * @date 2011/06/21
 **/
void css_nodepool_reset(easou_css_nodepool_t *pool)
{
	easou_css_mem_node_t *keep_node = NULL;
	easou_css_mem_node_t *node = NULL;
	keep_node = pool->mem_node_list;
	node = keep_node;
	/* free extra mem and only keep one mem node*/
	while (node->next != NULL)
	{
		keep_node = node->next;
		if (node->p_mem != NULL)
		{
			free(node->p_mem);
			node->p_mem = NULL;
		}
		free(node);
		node = keep_node;
	}
	keep_node->next = NULL;
	/* reset the nodepool */
	pool->mem_node_list = keep_node;
	pool->p_curr_mem = keep_node->p_mem;
	pool->p_curr_mem_size = keep_node->mem_size;
	pool->p_pool_avail = keep_node->p_mem;
}

/**
 * @brief destroy the nodepool
 * @author xunwu
 * @date 2011/11/10
 **/
void css_nodepool_destroy(easou_css_nodepool_t *pool)
{
	easou_css_mem_node_t *node = pool->mem_node_list;
	while (node != NULL)
	{
		easou_css_mem_node_t *next = node->next;
		if (node->p_mem != NULL)
		{
			free(node->p_mem);
			node->p_mem = NULL;
		}
		free(node);
		node = next;
	}
	pool->mem_node_list = NULL;
	pool->p_curr_mem = NULL;
	pool->p_curr_mem_size = 0;
	pool->p_pool_avail = NULL;
}

/**
 * @brief	��nodepool��ȡsize��С���ڴ�.
 * @retval   �ɹ������ڴ��ַ,ʧ�ܷ���NULL.
 * @author xunwu
 * @date 2011/11/10
 **/
void *css_get_from_nodepool(easou_css_nodepool_t *pool, size_t size)
{
	void *ret_mem = NULL;
	if (size > pool->p_curr_mem_size)
	{
		Fatal((char*) "%s:%s:required size > current mem pool size!", __FILE__, __FUNCTION__);
		return NULL;
	}
	if ((char *) (pool->p_pool_avail) + size <= (char *) (pool->p_curr_mem) + pool->p_curr_mem_size)
	{
		ret_mem = pool->p_pool_avail;
		pool->p_pool_avail = (char *) (pool->p_pool_avail) + size;
	}
	else
	{
		if (css_nodepool_request(pool, pool->p_curr_mem_size) < 0)
			return NULL;
		if ((char *) (pool->p_pool_avail) + size <= (char *) (pool->p_curr_mem) + pool->p_curr_mem_size)
		{
			ret_mem = pool->p_pool_avail;
			pool->p_pool_avail = (char *) pool->p_pool_avail + size;
		}
		else
			return NULL;
	}

	return ret_mem;
}
