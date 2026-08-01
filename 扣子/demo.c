#include <stdio.h>
#include <string.h>

int main()
{
    char command[4096];
    FILE *fp;
    char buffer[4096];
    
    char question[1024] = "你好";  
    printf("请输入问题：\n");
    scanf("%s", question);
    
    // 构建curl命令，分多行编写
    snprintf(command, sizeof(command), 
             "curl -X POST 'https://api.coze.cn/v3/chat?' \
             -H \"Authorization: Bearer cztei_qqOgkecyrVgxzveHG1CrcrtgTdEnZcVIQr0bW6U5BNz90kFnVUgJQk3wY8xRCXcC2\" \
             -H \"Content-Type: application/json\" \
             -d '{\"bot_id\": \"7632527845897895951\", \"user_id\": \"123456789\", \"stream\": true, \"additional_messages\":  \
              [{\"content\": \"%s\", \"content_type\": \"text\",  \
              \"role\": \"user\", \"type\": \"question\"}], \"parameters\": {}}'"
    ,question);
    
    // 使用popen执行命令
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("无法执行命令\n");
        return 1;
    }
    
    // 读取命令输出
    printf("执行结果：\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
      // printf("%s", buffer);  //输出所有信息

   //判断是否包含是 content 关键字
    char *completed =  strstr(buffer, "type");
    char *text =  strstr(buffer, "content");
    char *answer = strstr(buffer, "\"answer\"");

    if (completed != NULL && text != NULL  && answer != NULL) 
    {
        char *m = strstr(text, ":");
        //去掉content 后面的内容  
        char *end = strstr(m, ",");
        if (end != NULL) {
            *end = '\0';  //补充结束符
        }
        printf("%s\n", m+1);  //跳过 :
    }

 }
  /*
event:conversation.message.completed
data:{"id":"7632591619372040228",
"conversation_id":"7632591616696320036",
"bot_id":"7632527845897895951",
"role":"assistant",
"type":"answer",
"content":"我能为你介绍嵌入式细分领域及对应岗位，明确岗位核心技能与适配人群；规划学习路径，从基础到进阶推荐内容并提炼关键资源；分析主流技术栈，如MCU、FPGA等，还能提示职业晋升方向和技能拓展重点。 ","content_type":"text","chat_id":"7632591616696369188",
"section_id":"7632591616696320036",
"created_at":1777101222,
"time_cost":{"total_duration_ms":3050}}
  */
 
  //获取content字段的内容  
  



    // 关闭文件指针
    pclose(fp);
    
    return 0;
}