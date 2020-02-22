#include "handle_cJSON.h"

#define DBG_TAG "handle.cjson"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#define cJSON_GetString(json, item) cJSON_GetStringValue(cJSON_GetObjectItem(json, item))
/* -------------------------------     cjson handle    ------------------------------- */

struct cjson_handle *cjson_handle_list = NULL;

/*  
 *  cjson_handle_find
 *  功能: 从 handle list 中找到名字是 name 的那个handle
 *  handle  :   要处理的 handle list
 *  name    :   需要找到的 handle name
 *  return  :
 *      非 0 为找到对应 handle，反之为找不到
 */
struct cjson_handle *cjson_handle_find(struct cjson_handle *handle, char *name)
{
    struct cjson_handle *this_list = handle;
    while (this_list)
    {
        if (strcmp(this_list->name, name) == 0)
            return this_list;
        this_list = this_list->next;
    }
    return (struct cjson_handle *)0;
}
struct cjson_handle *cjson_handle_add(char *name)
{
    struct cjson_handle *handle = cjson_handle_find(cjson_handle_list, name);
    if (!handle)
    {
        handle = malloc(sizeof(struct cjson_handle));
        memset(handle, 0, sizeof(struct cjson_handle));
        handle->name = name;
        if (!cjson_handle_list)
            cjson_handle_list = handle;
        else
        {
            struct cjson_handle *endLast = cjson_handle_list;
            while (endLast->next)
            {
                if (strcmp(endLast->name, name) == 0)
                {
                    free(handle);
                    return (struct cjson_handle *)0;
                }
                endLast = endLast->next;
            }
            endLast->next = handle;
        }
    }
    return handle;
}

void cjson_handle_remove(char *name)
{
    struct cjson_handle *endLast = cjson_handle_list;
    if (strcmp(endLast->name, name) == 0)
    {
        cjson_handle_list = endLast->next;
        free(endLast);
				return ;
    }
    while (endLast && strcmp(endLast->next->name, name) != 0)
        endLast = endLast->next;
    if (endLast)
    {
        struct cjson_handle *free_item = endLast->next;
        endLast->next = free_item->next;
        free(free_item);
    }
}

/**
 *  cjson_handle_add_hook
 *  功能：添加钩子函数到 handle 中
 *  handle  需要添加钩子函数的 handle 指针
 *  cb      要挂载到 handle 的钩子函数
*/
void cjson_handle_add_hook(struct cjson_handle *handle, cjson_handle_cb cb)
{
    if (cb && handle)
    {
        struct cjson_handle_hook *hook = malloc(sizeof(struct cjson_handle_hook));
        hook->cb = cb;
        hook->next = 0;
        if (!handle->hook)
            handle->hook = hook;
        else
        {
            struct cjson_handle_hook *endLast = handle->hook;
            while (endLast->next)
                endLast = endLast->next;
            endLast->next = hook;
        }
    }
}
/**
 *  cjson_handle_add_to_parent
 *  功能:    给 parent 添加一个 子 handle
 *  parent  需要添加子handle的handle
 *  name    子 handle 的名字
 *  return  子 handle 的地址
*/
struct cjson_handle *cjson_handle_add_to_parent(struct cjson_handle *parent, char *name)
{
    struct cjson_handle *handle = malloc(sizeof(struct cjson_handle));
    memset(handle, 0, sizeof(struct cjson_handle));
    handle->name = name;
    if (!parent->list)
        parent->list = handle;
    else
    {
        struct cjson_handle *endLast = parent->list;
        while (endLast->next)
        {
            if (strcmp(endLast->name, name) == 0)
            {
                free(handle);
                return (struct cjson_handle *)0;
            }
            endLast = endLast->next;
        }
        endLast->next = handle;
    }
    return handle;
}
/**
 *  cjson_handle_add_hook_to_parent
 *  功能:    添加 钩子函数到 parent handle 的 name 子handle中
 *              如果 parent 中，没有 name 名字的子handle，则创建一个
 *  parent  父级 handle
 *  name    父级的 子handle 名称
 *  cb      子handle 的钩子函数
 *  return  子handle 地址
*/
struct cjson_handle *cjson_handle_add_hook_to_parent(struct cjson_handle *parent, char *name, cjson_handle_cb cb)
{
    struct cjson_handle *handle = cjson_handle_find(parent->list, name);
    if (!handle)
        handle = cjson_handle_add_to_parent(parent, name);
    cjson_handle_add_hook(handle, cb);
    return handle;
}

/*  
 *  cjson_handle_hook
 *  功能: 处理钩子，钩子必须返回 true / false ，用于判断是否继续执行 list 内容
 *  handle  :   要处理的 handle
 *  json    :   传给 hook 函数的 json 数据
 *  return  :
 *      1(true)
 *      0(false)
 */
uint8_t cjson_handle_hook(struct cjson_handle_hook *handle_hook, cJSON *json)
{
    struct cjson_handle_hook *hook = handle_hook;
    uint8_t re = 1;
    while (hook)
    {
        if (hook->cb)
            re = hook->cb(json) && re ? 1 : 0;
        hook = hook->next;
    }
    return re;
}

void cjson_handle_(struct cjson_handle *handle, char *name, cJSON *json)
{
    struct cjson_handle *this_handle = cjson_handle_find(handle, name);
    if (!this_handle && strcmp(handle->name, name) == 0)
        this_handle = handle;
    if (this_handle)
    {
        if (cjson_handle_hook(this_handle->hook, json) && this_handle->list)
        {
            // 可执行
            cJSON *sec = cJSON_GetObjectItem(json, name);
            if (sec)
            {
                //  回调
                cjson_handle_(this_handle->list, cJSON_GetStringValue(sec), json);
            }
        }
    }
}
/**
 *  cjson_handle
 *  功能:    对json进行数据解析，判断是否存在 type item，不存在则退出，反之则处理
*/
void cjson_handle(cJSON *json)
{
    if (cJSON_IsObject(json) && cjson_handle_list)
    {
        cJSON *type_item = cJSON_GetObjectItem(json, "type");
        if (type_item)
        {
            cjson_handle_(cjson_handle_list, cJSON_GetStringValue(type_item), json);
        }
    }
}
/**
 *  cjson_handle_from_item
 *  功能:    对json进行数据解析，判断是否存在 item name 为 item_name 的，不存在则退出，反之则处理
*/
void cjson_handle_from_item(cJSON *json, char *item_name)
{
    if (cJSON_IsObject(json) && cjson_handle_list)
    {
        cJSON *type_item = cJSON_GetObjectItem(json, item_name);
        if (type_item)
        {
            cjson_handle_(cjson_handle_list, cJSON_GetStringValue(type_item), json);
        }
    }
}
/* -------------------------------     cjson handle    ------------------------------- */
