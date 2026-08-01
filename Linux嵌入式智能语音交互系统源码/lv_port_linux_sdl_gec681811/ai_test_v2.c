#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern void RecordingRecognition(char *text);

// 处理转义字符
static void unescape_string(char *dest, const char *src)
{
    while (*src) {
        if (*src == '\\' && *(src + 1)) {
            src++;
            switch (*src) {
                case 'n':  *dest++ = '\n'; break;
                case 't':  *dest++ = '\t'; break;
                case 'r':  *dest++ = '\r'; break;
                case '"':  *dest++ = '"';  break;
                case '\\': *dest++ = '\\'; break;
                default:   *dest++ = '\\'; *dest++ = *src; break;
            }
        } else {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';
}

// 清理Markdown格式字符
static void clean_markdown(char *str)
{
    char *src = str;
    char *dst = str;
    
    while (*src) {
        // 跳过 ## 标题标记
        if (*src == '#' && *(src + 1) == '#') {
            src += 2;
            // 跳过空格
            while (*src == ' ') src++;
            continue;
        }
        // 跳过单个 # 
        if (*src == '#' && *(src + 1) == ' ') {
            src += 2;
            continue;
        }
        // 跳过 ** 加粗标记
        if (*src == '*' && *(src + 1) == '*') {
            src += 2;
            continue;
        }
        // 跳过 --- 分隔线
        if (*src == '-' && *(src + 1) == '-' && *(src + 2) == '-') {
            src += 3;
            while (*src == '-') src++;
            while (*src == '\n') src++;
            continue;
        }
        // 跳过列表标记 - （行首的）
        if (*src == '-' && *(src + 1) == ' ' && (src == str || *(src - 1) == '\n')) {
            src += 2;
            continue;
        }
        // 跳过数字列表 1. 2. 等
        if (*src >= '0' && *src <= '9' && *(src + 1) == '.' && *(src + 2) == ' ') {
            src += 3;
            continue;
        }
        // 处理连续多个换行，合并为最多两个换行
        if (*src == '\n') {
            *dst++ = '\n';
            src++;
            // 跳过一个换行
            if (*src == '\n') {
                *dst++ = '\n';
                src++;
            }
            // 跳过更多连续的换行
            while (*src == '\n') src++;
            continue;
        }
        
        *dst++ = *src++;
    }
    *dst = '\0';
}

static char* ai_request(const char *input_text)
{
    char command[8192];
    FILE *fp;
    char buffer[4096];
    char *result = NULL;
    
    // 拼接提示语
    char wrapped_input[2048];
    snprintf(wrapped_input, sizeof(wrapped_input), "%s，请简短回答，不要使用任何markdown格式，不要使用emoji", input_text);
    
    snprintf(command, sizeof(command), 
        "curl -k -s -X POST \"https://api.coze.cn/v3/chat?\" "
        "-H \"Authorization: Bearer cztei_lrOMF5aRDZkSVZIeChhgKtLEmneVgjUlrjByo4ijdJJ3QaRyZmzu3LkPHmERIUQrI\" "
        "-H \"Content-Type: application/json\" "
        "-d \"{\\\"bot_id\\\":\\\"7642911541591425030\\\",\\\"user_id\\\":\\\"123456789\\\",\\\"stream\\\":true,"
        "\\\"additional_messages\\\":[{\\\"content\\\":\\\"%s\\\",\\\"content_type\\\":\\\"text\\\","
        "\\\"role\\\":\\\"user\\\",\\\"type\\\":\\\"question\\\"}],\\\"parameters\\\":{}}\"",
        wrapped_input);
    
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("无法执行命令\n");
        return NULL;
    }
    
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        char *ack = strstr(buffer, "time_cost");
        char *content = strstr(buffer, "content");
        char *answer = strstr(buffer, "\"answer\"");
        
        if (ack != NULL && content != NULL && answer != NULL) {
            char *start = strstr(content, "\":\"");
            if (start != NULL) {
                start += 3;
                char *end = strchr(start, '"');
                if (end != NULL) {
                    size_t len = end - start;
                    char *temp = (char*)malloc(len + 1);
                    if (temp != NULL) {
                        strncpy(temp, start, len);
                        temp[len] = '\0';
                        
                        result = (char*)malloc(len + 1);
                        if (result != NULL) {
                            unescape_string(result, temp);
                            clean_markdown(result);  // 清理Markdown格式
                        }
                        free(temp);
                    }
                }
            }
        }
    }
    
    pclose(fp);
    return result;
}

// 语音输入
char* ai_reply(void)
{
    char text[1024] = {0};
    RecordingRecognition(text);
    return ai_request(text);
}

// 文本输入
char* ai_reply_with_text(const char *input_text)
{
    return ai_request(input_text);
}