#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 录制并识别音频文件
void RecordingRecognition(char *text)
{
     printf("正在录制音频文件......\n");
     // 1.加载音频录制命令
     system("arecord -f S16_LE -r 16000 -c 1 -d 10 voice.wav");

     printf("音频文件录制完毕,开始识别.....\n");

     // 2.调用popen 加载
     // curl -i -X POST -H "Content-Type: audio/wav;rate=16000" "http://vop.baidu.com/pro_api?dev_pid=80001&cuid=gec6818jack&token=24.97b736bbcc23dff81ceccb377902c719.2592000.1781597946.282335-123334510" --data-binary "@./voice.wav"

     // 1. 定义curl命令（和你提供的完全一致，注意转义双引号）
     // popen执行的命令需要用双引号包裹，内部双引号前加\转义
     const char *curl_cmd = "curl -i -X POST -H \"Content-Type: audio/wav;rate=16000\" \"http://vop.baidu.com/pro_api?dev_pid=80001&cuid=gec6818jack&token=24.97b736bbcc23dff81ceccb377902c719.2592000.1781597946.282335-123334510\" --data-binary \"@./voice.wav\"";

     // 2. 调用popen执行命令，模式为"r"（只读，读取命令输出）
     FILE *fp = popen(curl_cmd, "r");
     if (fp == NULL)
     {
          perror("popen调用失败"); // 打印错误原因
          return;
     }

     // 3. 读取命令输出
     char buffer[1024];
     while (fgets(buffer, sizeof(buffer), fp) != NULL)
     {
        //  printf("%s", buffer); // 打印命令输出

          //"result":["我是第三组同学。"]

          // 过滤信息
          if (strstr(buffer, "result") != NULL) // strstr 查找字符串
          {
               char *p = strstr(buffer, "result");
               p += 10; // 跳过 "result":

               char *q = strstr(p, "\"");
               *q = '\0';
               printf("识别结果：%s\n", p);

               strcpy(text, p); // 保存识别结果

               break;
          }
     }

     // 4. 关闭文件指针
     pclose(fp);
}

int main()
{

     char text[1024] = {0};
     RecordingRecognition(text);

     printf("text:%s\n", text);

     return 0;
}