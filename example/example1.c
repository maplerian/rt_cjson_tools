#include "rtthread.h"
#include "rt_cjson_tools.h"

#define DBG_TAG "app.cjson.tools.example1"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#ifndef PKG_RT_CJSON_TOOLS_EXAMPLE_STACK_SZIE
#define PKG_RT_CJSON_TOOLS_EXAMPLE_STACK_SZIE 1024
#endif
#ifndef PKG_RT_CJSON_TOOLS_EXAMPLE_PRIORITY
#define PKG_RT_CJSON_TOOLS_EXAMPLE_PRIORITY 15
#endif

#define HELLO_JSON_DATA "{\"type\":\"api\",\"api\":\"hello\"}"
#define HELLO_JSON_DATA_ITEM "{\"item\":\"api\",\"api\":\"hello\"}"

uint8_t cjson_handle_hello(cJSON *json)
{
    printf("[I/%s] hello\n",DBG_TAG);
    return RT_EOK;
}

void rt_cjson_tools_example1_entry(void *p)
{
    //  Add API list
    rt_cjson_handle_t api = rt_cjson_handle_add("api");
    //  Add Hello function to API
    rt_cjson_handle_add_hook_to_parent(api,"hello",cjson_handle_hello);

    //  Parsing JSON data with cjson
    cJSON * json = cJSON_Parse(HELLO_JSON_DATA);
    //  Start with type keyword
    rt_cjson_handle(json);
    cJSON_Delete(json); //  free

    //  Parsing JSON data with cjson
    json = cJSON_Parse(HELLO_JSON_DATA_ITEM);
    //  Start with item keyword
    rt_cjson_handle_from_item(json,"item");
    cJSON_Delete(json); //  free
}

//  create thread
int rt_cjson_tools_example1_init(void)
{
    rt_thread_t thread = RT_NULL;
    thread = rt_thread_create(
        "cjson_t_e1",
        rt_cjson_tools_example1_entry,
        RT_NULL,
        PKG_RT_CJSON_TOOLS_EXAMPLE_STACK_SZIE,
        PKG_RT_CJSON_TOOLS_EXAMPLE_PRIORITY,
        10);
    if (thread == RT_NULL)
    {
        LOG_E("Failed to create an example");
        return -RT_ERROR;
    }
    if (rt_thread_startup(thread) != RT_EOK)
    {
        LOG_E("Unable to start sample thread");
        return -RT_ERROR;
    }
    return RT_EOK;
}
INIT_APP_EXPORT(rt_cjson_tools_example1_init);
