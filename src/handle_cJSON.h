#ifndef __HANDLE_CJSON_H__
#define __HANDLE_CJSON_H__
#include "cJSON.h"
#include "string.h"
#include "stdint.h"
#include "stdlib.h"

typedef uint8_t (*cjson_handle_cb)(cJSON *json);

struct cjson_handle_hook
{
    cjson_handle_cb cb;
    struct cjson_handle_hook *next;
};
struct cjson_handle
{
    char *name;
    struct cjson_handle_hook *hook;
    struct cjson_handle *list;
    struct cjson_handle *next;
};

//  处理类 接口
void cjson_handle(cJSON *json);
void cjson_handle_from_item(cJSON *json,char *item_name);

//  功能类 接口(添加，查找，删除)
struct cjson_handle *cjson_handle_add(char *name);
struct cjson_handle *cjson_handle_add_to_parent(struct cjson_handle *parent, char *name);
struct cjson_handle *cjson_handle_find(struct cjson_handle *handle, char *name);
void cjson_handle_remove(char *name);

//  功能类 接口(添加钩子函数)
void cjson_handle_add_hook(struct cjson_handle *handle, cjson_handle_cb cb);
struct cjson_handle *cjson_handle_add_hook_to_parent(struct cjson_handle *parent, char *name, cjson_handle_cb cb);

#endif
