#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "./ui/ui.h"

// 生命外部定义的字典
extern const lv_pinyin_dict_t my_lv_ime_pinyin_dict[];

static void lv_linux_disp_init(void)
{
    lv_display_t *disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, "/dev/fb0");
}

// 初始化中文字库
lv_style_t *init_chinese_font(lv_obj_t *obj, int font_size)
{
    // 1.创建中文字体
    lv_font_t *font = lv_freetype_font_create("/simkai.ttf", // 👍加载字库文件
                                              LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                              font_size,
                                              LV_FREETYPE_FONT_STYLE_NORMAL);

    // 2.创建中文字体样式
    static lv_style_t style;
    lv_style_init(&style);                                 // 初始化样式
    lv_style_set_text_font(&style, font);                  // 设置中文字体样式
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER); // 字体居中显示

    lv_obj_add_style(ui_Screen1, &style, 0);

    return &style;
}

// 初始化拼音输入法
void init_pinyin(lv_obj_t *keyboard)
{
    // 创建拼音输入法对象，将其添加到当前活动屏幕上
    lv_obj_t *pinyin_ime = lv_ime_pinyin_create(lv_screen_active());

    lv_ime_pinyin_set_dict(pinyin_ime, &my_lv_ime_pinyin_dict); // 使用自定义词典，如果不设置则使用内置词典

    lv_ime_pinyin_set_keyboard(pinyin_ime, keyboard); // 将键盘设置到拼音输入法，使输入法可以控制键盘

    /*Get the cand_panel, and adjust its size and position 设置输入栏的大小和位置 */
    lv_obj_t *cand_panel = lv_ime_pinyin_get_cand_panel(pinyin_ime);
    lv_obj_set_size(cand_panel, LV_PCT(100), LV_PCT(10));
    lv_obj_align_to(cand_panel, keyboard, LV_ALIGN_OUT_TOP_MID, 0, 0);
}

int main(void)
{
    // 初始化LVGL库，设置内部状态、内存管理和任务系统
    lv_init();
    /*Linux display device init*/
    // 调用Linux显示设备初始化函数，根据配置创建帧缓冲或SDL窗口显示
    lv_linux_disp_init();

    // 创建输入设备
    lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event0");
    // 校准输入设备屏幕坐标
    lv_evdev_set_calibration(touch, 0, 0, 1024, 600); // 黑色边框的屏幕
    //lv_evdev_set_calibration(touch, 0, 0, 800, 480);  // 蓝色边框的屏幕

    ui_init();

    // 加载中文字体
    lv_font_t *chinese_font = lv_freetype_font_create("/simkai.ttf",
                                                        LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                        20,
                                                        LV_FREETYPE_FONT_STYLE_NORMAL);
    // 创建样式
    static lv_style_t style_cn;
    lv_style_init(&style_cn);
    lv_style_set_text_font(&style_cn, chinese_font);

    // 应用到控件
    lv_obj_add_style(ui_Text1, &style_cn, 0);
    lv_obj_add_style(ui_Text2, &style_cn, 0);

    /*Handle LVGL tasks*/
    // 主循环：持续处理LVGL定时器任务，驱动UI更新和动画
    while (1)
    {
        lv_timer_handler(); // 执行LVGL定时器处理，更新屏幕显示和处理事件
        usleep(5000);       // 休眠5毫秒，避免CPU占用过高，同时保证UI流畅性
    }
    return 0;
}
