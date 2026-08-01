#include "lvgl/lvgl.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "lvgl/src/libs/freetype/lv_freetype.h"

#include "./ui/ui.h"

extern const lv_pinyin_dict_t my_lv_ime_pinyin_dict[];

static lv_font_t *chinese_font = NULL;

static void lv_linux_disp_init(void)
{
    lv_display_t *disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, "/dev/fb0");
}

void init_pinyin(lv_obj_t *keyboard)
{
    lv_obj_t *pinyin_ime = lv_ime_pinyin_create(lv_screen_active());
    lv_ime_pinyin_set_dict(pinyin_ime, (lv_pinyin_dict_t *)my_lv_ime_pinyin_dict);
    lv_ime_pinyin_set_keyboard(pinyin_ime, keyboard);
    
    lv_obj_t *cand_panel = lv_ime_pinyin_get_cand_panel(pinyin_ime);
    lv_obj_set_size(cand_panel, LV_PCT(100), LV_PCT(10));
    lv_obj_align_to(cand_panel, keyboard, LV_ALIGN_OUT_TOP_MID, 0, 0);
}

int main(void)
{
    /* 1. 初始化 LVGL */
    lv_init();
    lv_linux_disp_init();
    
    lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event0");
    lv_evdev_set_calibration(touch, 0, 0, 800, 480);
    
    /* 2. 初始化 FreeType（必须先于 UI 初始化）*/
    lv_freetype_init(4096, 8, 8);
    
    /* 3. 提前加载字体 */
    printf("Loading font...\n");
    chinese_font = lv_freetype_font_create(
        "/simhei.ttf",
        LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
        36,
        LV_FREETYPE_FONT_STYLE_NORMAL
    );
    
    if (chinese_font == NULL) {
        printf("Trying OUTLINE mode...\n");
        chinese_font = lv_freetype_font_create(
            "/simhei.ttf",
            LV_FREETYPE_FONT_RENDER_MODE_OUTLINE,
            36,
            LV_FREETYPE_FONT_STYLE_NORMAL
        );
    }
    
    if (chinese_font == NULL) {
        printf("ERROR: Cannot load font!\n");
        return -1;
    }
    
    /* 4. 测试字形查找 */
    lv_font_glyph_dsc_t glyph_dsc;
    printf("U+4F60 (你): %s\n", lv_font_get_glyph_dsc(chinese_font, &glyph_dsc, 0x4F60, 0) ? "OK" : "FAIL");
    printf("U+597D (好): %s\n", lv_font_get_glyph_dsc(chinese_font, &glyph_dsc, 0x597D, 0) ? "OK" : "FAIL");
    printf("U+0041 (A):  %s\n", lv_font_get_glyph_dsc(chinese_font, &glyph_dsc, 0x0041, 0) ? "OK" : "FAIL");
    
    /* 5. 设置全局默认字体（推荐）*/
    lv_theme_t *th = lv_theme_default_init(lv_display_get_default(), 
                                          lv_palette_main(LV_PALETTE_BLUE),
                                          lv_palette_main(LV_PALETTE_RED),
                                          LV_THEME_DEFAULT_DARK,
                                          chinese_font);
    lv_display_set_theme(lv_display_get_default(), th);
    
    /* 6. 现在才初始化 UI */
    ui_init();
    
    /* 7. 为特定标签设置字体（如果 UI 有多个控件需要中文字体）*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, chinese_font);
    lv_obj_add_style(ui_Label1, &style, 0);
    
    lv_label_set_text(ui_Label1, "你好世界");
    printf("Text set to: 你好世界\n");
    
    /* 8. 主循环 */
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }
    
    return 0;
}