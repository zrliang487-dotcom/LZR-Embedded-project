#include "../../lv_examples.h"
#if LV_USE_LABEL && LV_USE_TEXTAREA && LV_FONT_SIMSUN_16_CJK && LV_USE_IME_PINYIN && LV_BUILD_EXAMPLES

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);

    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_active()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
        }
    }
    else if(code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta);   /*To forget the last clicked object to make it focusable again*/
    }
}

void lv_example_ime_pinyin_1(void)
{
    // 创建拼音输入法对象，将其添加到当前活动屏幕上
    lv_obj_t * pinyin_ime = lv_ime_pinyin_create(lv_screen_active());
    // 设置拼音输入法的文本字体为16号中文字体（支持中日韩字符）
    lv_obj_set_style_text_font(pinyin_ime, &lv_font_simsun_16_cjk, 0);
    //lv_ime_pinyin_set_dict(pinyin_ime, your_dict); // 使用自定义词典，如果不设置则使用内置词典

    /* ta1 - 创建一个单行文本区域，用于输入中文 */
    lv_obj_t * ta1 = lv_textarea_create(lv_screen_active());
    lv_textarea_set_one_line(ta1, true);  // 设置为单行模式
    lv_obj_set_style_text_font(ta1, &lv_font_simsun_16_cjk, 0);  // 设置字体为16号中文字体
    lv_obj_align(ta1, LV_ALIGN_TOP_LEFT, 0, 0);  // 左上角对齐

    /*创建键盘并将其添加到拼音输入法中*/
    lv_obj_t * kb = lv_keyboard_create(lv_screen_active());  // 在当前活动屏幕上创建键盘对象
    lv_ime_pinyin_set_keyboard(pinyin_ime, kb);              // 将键盘设置到拼音输入法，使输入法可以控制键盘
    lv_keyboard_set_textarea(kb, ta1);                       // 将键盘关联到文本区域ta1，使键盘输入能显示在ta1中

    lv_obj_add_event_cb(ta1, ta_event_cb, LV_EVENT_ALL, kb);

    /*Get the cand_panel, and adjust its size and position*/
    lv_obj_t * cand_panel = lv_ime_pinyin_get_cand_panel(pinyin_ime);
    lv_obj_set_size(cand_panel, LV_PCT(100), LV_PCT(10));
    lv_obj_align_to(cand_panel, kb, LV_ALIGN_OUT_TOP_MID, 0, 0);

    /*Try using ime_pinyin to output the Chinese below in the ta1 above*/
    lv_obj_t * cz_label = lv_label_create(lv_screen_active());
    lv_label_set_text(cz_label,
                      "嵌入式系统（Embedded System），\n是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。");
    lv_obj_set_style_text_font(cz_label, &lv_font_simsun_16_cjk, 0);
    lv_obj_set_width(cz_label, 310);
    lv_obj_align_to(cz_label, ta1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
}

#endif
