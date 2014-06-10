/**
 * @file utils/simplehash.h
 **/

#ifndef  __SIMPLEHASH_H_
#define  __SIMPLEHASH_H_

#include <unistd.h>
#include "nodepool.h"

struct hashmap_t;
/**< ��hashmap */

#define DEFAULT_HASHMAP_SIZE 521		  /**< better to be prime number       */

/**
 * @brief ����hashmap.
 * @return  hashmap_t* �ɹ�����hashmapָ��,ʧ�ܷ���NULL.
 **/
hashmap_t *hashmap_create(int size);
hashmap_t *hashmap_create();

/**
 * @brief �ݻ�hashmap.
 **/
void hashmap_destroy(hashmap_t *hm);

/**
 * @brief hashmap���.
 **/
void hashmap_clean(hashmap_t *hm);

/**
 * @brief ��hashmap�д�һ��Ԫ��.
 *  ������keyֵ�ظ�����.
 **/
void hashmap_put(hashmap_t *hm, const char *key, void *data);

void hashmap_mod(hashmap_t *hm, const char *key, void *data);

/**
 * @brief ����ֵ��hashmap�л�ȡԪ��.
 **/
void *hashmap_get(hashmap_t *hm, const char *key);

void *hashmap_get(hashmap_t *hm, const char *key, int key_len);

/**
 * @brief ׼������hashmap�е�Ԫ��.
 **/
void hashmap_iter_begin(hashmap_t *hm);

/**
 * @brief hashmap������ȡ��һ��Ԫ��.
 * XXX: ���������̲߳���ȫ.
 * @retval   ����Ԫ��ָ��. ������NULL,����Ԫ���ѵ�����.
 **/
void *hashmap_iter_next(hashmap_t *hm);

/**
 * @brief ��ȡhashmap���Ѳ����Ԫ������.
 **/
int hashmap_get_element_num(hashmap_t *hm);

/**
 * @brief ���ݼ�ֵ��������hashͰ�е����.
 **/
int key_to_hash_bucket_index(const char *key, hashmap_t *hm);

int key_to_hash_bucket_index(const char *key, int key_len, hashmap_t *hm);

/**
 * @brief �����������hashͰ��ŵ�ǰ����, �����ӦԪ��.
 *  XXX: for advanced using.
 **/
void hashmap_put(hashmap_t *hm, int i_bucket, const char *key, void *value);

void hashmap_put(hashmap_t *hm, int i_bucket, const char *key, int key_len, void *value);

/**
 * @brief �޸�hash���е�data�ֶ�
 * @author sue
 * @date 2013/04/09
 */
void hashmap_mod(hashmap_t *hm, int i_bucket, const char *key, void *value);

void hashmap_mod(hashmap_t *hm, int i_bucket, const char *key, int key_len, void *value);

/**
 * @brief �����������hashͰλ�õ�ǰ����, ��ȡ��ӦԪ��.
 *  XXX: for advanced using.
 **/
void *hashmap_get(hashmap_t *hm, int i_bucket, const char *key);

void *hashmap_get(hashmap_t *hm, int i_bucket, const char *key, int key_len);

/**
 * @brief ���key�Ƿ���hashmap��, ���������.
 * @return  bool �Ƿ���hashmap��.
 **/
bool check_in_hashmap(hashmap_t *hm, const char *key, void *value);

#endif  //__TOOLS/SIMPLEHASH_H_
