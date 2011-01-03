/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2010-2011 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Shuhei Tanuma <stanuma@zynga.co.jp>                          |
  +----------------------------------------------------------------------+
*/

#include "php_git.h"
#include <spl/spl_array.h>
#include <zend_interfaces.h>
#include <string.h>
#include <time.h>

git_tree *php_get_git_tree(zval *obj TSRMLS_DC) {
    zval **tmp = NULL;
    git_tree *tree = NULL;
    int id = 0, type = 0;
    if (zend_hash_find(Z_OBJPROP_P(obj), "tree", strlen("tree") + 1,(void **)&tmp) == FAILURE) {
        return NULL;
    }

    id = Z_LVAL_PP(tmp);
    tree = (git_tree *) zend_list_find(id, &type);
    return tree;
}



PHP_METHOD(git_tree, count)
{
	long cnt = 0;
    zval *entries;

    entries = zend_read_property(git_tree_entry_class_entry,getThis(),"entries",7,0 TSRMLS_DC);
	cnt = zend_hash_num_elements(Z_ARRVAL_P(entries));

    RETURN_LONG(cnt);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_git_tree_path, 0, 0, 1)
    ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

PHP_METHOD(git_tree, path)
{
    zval *object = getThis();
    char *path;
    int path_len= 0;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "s", &path, &path_len) == FAILURE){
        return;
    }
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_git_tree_add, 0, 0, 1)
    ZEND_ARG_INFO(0, entry)
ZEND_END_ARG_INFO()
PHP_METHOD(git_tree, add)
{
    zval *object = getThis();
    zval *entry;
    char *filename;
    git_oid oid;
    git_tree *tree;
    int attr;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "z", &entry) == FAILURE){
        return;
    }
    
    tree = php_get_git_tree(object TSRMLS_CC);
    git_oid_mkstr(&oid, Z_STRVAL_P(zend_read_property(git_tree_entry_class_entry,entry,"oid",3,0 TSRMLS_CC)));
    filename = Z_STRVAL_P(zend_read_property(git_tree_entry_class_entry,entry,"name",4,0 TSRMLS_CC));
    attr = Z_LVAL_P(zend_read_property(git_tree_entry_class_entry,entry,"attr",4,0 TSRMLS_CC));
    //git_tree_add_entry (git_tree *tree, const git_oid *id, const char *filename, int attributes)

    char uhi[40];
    git_oid *mo;
    git_oid_to_string(uhi,41,&oid);
    printf("oid: %s\n",uhi);
    printf("path: %s\n",filename);
    mo = git_tree_id(tree);
    git_oid_to_string(&uhi,41,mo);
    printf("tree-oid: %s\n",uhi);
    
    
    git_tree_add_entry(tree, &oid, filename, attr);

    int ret = git_object_write((git_object *)tree);
    if(ret != GIT_SUCCESS){
        php_printf("can't write object");
    }
    
    char out[40];
    git_oid *om;
    om = git_object_id((git_object *)tree);
    git_oid_to_string(&out,41,om);
    
    RETVAL_STRING(&out, 1);
}


// GitTree
PHPAPI function_entry php_git_tree_methods[] = {
    PHP_ME(git_tree, count, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(git_tree, path, arginfo_git_tree_path, ZEND_ACC_PUBLIC)
    PHP_ME(git_tree, add, arginfo_git_tree_add, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void git_init_tree(TSRMLS_D)
{
    zend_class_entry git_tree_ce;
    INIT_CLASS_ENTRY(git_tree_ce, "GitTree", php_git_tree_methods);
    git_tree_class_entry = zend_register_internal_class(&git_tree_ce TSRMLS_CC);
    zend_class_implements(git_tree_class_entry TSRMLS_CC, 1, spl_ce_Countable);
}