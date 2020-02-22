#include "parse_cJSON.h"

#define DBG_TAG "parse.cjson"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

parse_json_t *parse_json_create(uint16_t buf_len)
{
    parse_json_t *parse_json_obj = (parse_json_t *)malloc(sizeof(parse_json_t));
    memset(parse_json_obj, 0, sizeof(parse_json_t));
    parse_json_obj->buf = (char *)malloc(sizeof(char) * buf_len);
    parse_json_obj->len = buf_len;
    parse_json_obj->p = parse_json_obj->buf;
    LOG_W("malloc memory size:%d", sizeof(parse_json_t) + sizeof(char) * buf_len);
    return parse_json_obj;
}
void parse_json_free(parse_json_t *parse_json_obj)
{
    LOG_W("free memory size:%d", sizeof(parse_json_t) + sizeof(char) * parse_json_obj->len);
    free(parse_json_obj->buf);
    free(parse_json_obj);
}

void parse_json_start(parse_json_t *parse_json_obj)
{
    parse_json_obj->receive_state = 1; //  置为正在接收数据
    char *buf = parse_json_obj->buf;
    uint16_t len = parse_json_obj->len;
    memset(buf, 0, sizeof(char) * parse_json_obj->len);
    memset(parse_json_obj, 0, sizeof(parse_json_t));
    parse_json_obj->p = buf;
    parse_json_obj->buf = buf;
    parse_json_obj->len = len;
}

cJSON *parse_json_end(parse_json_t *parse_json_obj, uint8_t error)
{
    cJSON *json = 0;
    if (!error)
    {
        parse_json_obj->receive_state = 0;
        if (parse_json_obj->count > 2)
            json = cJSON_Parse(parse_json_obj->buf);
    }
    parse_json_start(parse_json_obj);
    return json;
}

#define RECEIVE_IS_NOT_JSON(parse_json_obj, if_on)                    \
    do                                                                \
    {                                                                 \
        if (!(if_on))                                                 \
        {                                                             \
            *parse_json_obj->p = '\0';                                \
            LOG_E("This is not JSON data:\n%s", parse_json_obj->buf); \
            return parse_json_end(parse_json_obj, 1);                 \
        }                                                             \
    } while (0)

cJSON *parse_json_ch(parse_json_t *parse_json_obj, char ch)
{
    //  可视字符区
    if (32 <= ch && ch <= 126)
    {
        //  json数据开始标志
        if (ch == '{')
        {
            //  开始接收数据
            if (parse_json_obj->Angle_brackets_left_flag == 0)
                parse_json_start(parse_json_obj);
            parse_json_obj->Angle_brackets_left_flag++;
        }
        else if (ch == '[')
        {
            //  开始接收数据
            if (parse_json_obj->middle_brackets_left_flag == 0 && !parse_json_obj->Angle_brackets_left_flag)
                parse_json_start(parse_json_obj);
            parse_json_obj->middle_brackets_left_flag++;
        }

        //  两个开始标志
        if (parse_json_obj->Angle_brackets_left_flag || parse_json_obj->middle_brackets_left_flag)
        {
            //  缓存字符
            *parse_json_obj->p++ = ch;
            //  若已接收到一个{ 就意味着开始记录数据了
            parse_json_obj->count++;

            /*  --------------------------   ↓ 错误规则 ↓   --------------------------    */
            //  不在双引号中
            if (parse_json_obj->double_quotation_marks_left_flag == parse_json_obj->double_quotation_marks_right_flag)
            {
                switch (parse_json_obj->last_flag)
                {
                case '{':
                {
                    RECEIVE_IS_NOT_JSON(parse_json_obj, ch == '"');
                    break;
                }
                case '[':
                case ',':
                case ':':
                {
                    RECEIVE_IS_NOT_JSON(parse_json_obj, ch == '"' || ch == '{' || ch == '[' || ('0' <= ch && ch <= '9'));
                    break;
                }
                case '}':
                case ']':
                {
                    RECEIVE_IS_NOT_JSON(parse_json_obj, ch == ',' || ch == '}' || ch == ']');
                    break;
                }
                case '"':
                {
                    RECEIVE_IS_NOT_JSON(parse_json_obj, ch == ':' || ch == ',' || ch == '}' || ch == ']');
                    break;
                }
                }
            }
            // //  处于 [ .... 中括号内容中
            // if (parse_json_obj->middle_brackets_left_flag > parse_json_obj->middle_brackets_right_flag)
            // {

            // }
            /*  --------------------------   ↑ 错误规则 ↑   --------------------------    */

            //  缓冲区溢出
            if (parse_json_obj->count >= parse_json_obj->len - 1)
            {
                *parse_json_obj->p = '\0';
                LOG_E("Overflow parsing JSON:\n%s", parse_json_obj->buf);
                return parse_json_end(parse_json_obj, 1);
            }

            //  记录标志
            switch (ch)
            {
            case '{':
            {
                break;
            }
            case '[':
            {
                break;
            }
            case '}':
            {
                parse_json_obj->Angle_brackets_right_flag++;
                break;
            }
            case ']':
            {
                parse_json_obj->middle_brackets_right_flag++;
                break;
            }
            case '"':
            {
                if (parse_json_obj->double_quotation_marks_left_flag == parse_json_obj->double_quotation_marks_right_flag)
                    parse_json_obj->double_quotation_marks_left_flag++;
                else
                    parse_json_obj->double_quotation_marks_right_flag++;
                break;
            }
            case ',':
            {
                break;
            }
            case ':':
            {
                break;
            }
            }
            parse_json_obj->last_flag = ch;

            //  总标志判断，全部相等就结束
            if (
                (parse_json_obj->Angle_brackets_right_flag == parse_json_obj->Angle_brackets_left_flag) &&
                (parse_json_obj->middle_brackets_right_flag == parse_json_obj->middle_brackets_left_flag) &&
                (parse_json_obj->double_quotation_marks_left_flag == parse_json_obj->double_quotation_marks_right_flag))
            {
                return parse_json_end(parse_json_obj, 0);
            }
        }
    }
    return (cJSON *)1;
}

cJSON *parse_json_str(parse_json_t *parse_json_obj, char *str)
{
    uint16_t count = strlen(str);
    if (count)
    {
        cJSON *json;
        do
        {
            json = parse_json_ch(parse_json_obj, *str++);
            if (json == 0)
                return (cJSON *)0;
            else if (json > (cJSON *)1)
                return json;
        } while (count--);
    }
    return (cJSON *)0;
}
