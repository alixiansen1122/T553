/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: UiConfig for application resource
 * Author:
 * Create: 2021-12-03
 */

#ifndef UI_CONFIG_H
#define UI_CONFIG_H

#include <cstdint>
#include "ohos_types.h"
#include "graphic_config.h"

namespace OHOS {
    typedef enum {
        WATCH_FACE = 0, /* 主表盘枚举值不能修改 */
        COMPASS,
        ACTIVITY,
        MUSIC_PLAYER,
        VIDEO_CARD1,
        VIDEO_CARD2,
        VIDEO_CARD3,
        MAX_CARD,
    } CardId;
    static const int CARD_SETTING_COUNT_MAX = MAX_CARD;

    typedef enum {
        DIAL_CLOCK = 0,
        GRADIENT_CLOCK,
        DIAL_MAX,
    } DialId;

    typedef enum {
        DIAL_DISPLAY_MODE_NORMAL = 0,
        DIAL_DISPLAY_MODE_AOD,
        DIAL_DISPLAY_MODE_MAX,
    } DialDisplayMode;

    typedef enum {
        DIAL_DISPLAY_SUPPORT_NORMAL = 0,
        DIAL_DISPLAY_SUPPORT_AOD,
        DIAL_DISPLAY_SUPPORT_NORMAL_AOD,
        DIAL_DISPLAY_SUPPORT_MAX,
    } DialDisplayAbility;

    #ifndef UNUSED
    #define UNUSED(x) ((void)(x))
    #endif

    #define LA_HORIZONTAL_RESOLUTION 400 // watch width
    #define LA_VERTICAL_RESOLUTION 400   // watch height
    #define NORMAL_BUTTON_WIDTH 100 // normal button width
    #define NORMAL_BUTTON_HEIGHT 50 // normal button height
    #define NORMAL_MARGIN 5 // normal margin
    #define DIAL_SQUARE 250   // watch plate sauqre
    #define SETTING_ITEM_HEIGHT 100   // setting item height
    #define UN_OPACITY 255   // UN_OPACITY

    #define MILLESECOND 1
    #define SECOND 1000         // 1s 1000ms
    #define MINUTE (60 * SECOND)  // 1min 60s
    #define HOUR (60 * MINUTE)    // 1hour 60min

    #define FONT_COMMON 20 // 20px
    #define X_DRAG_OFFSET 20
    #define DOUBLES 2

#if !defined (_WIN32)
    #define APP_RES_PATH "/user/res"
    #define APP_MUSIC_PATH "/user/music"
    #define APP_VIDEO_PATH "/user/video"
    #define APP_DIAL_PATH "/user/dial"
    #define APP_RECENT_PATH "/system/recent/"
#else
#ifdef RELEASE_VERSION
    #define APP_RES_PATH "res/res"
    #define APP_MUSIC_PATH "res/music"
    #define APP_VIDEO_PATH "res/video"
    #define APP_DIAL_PATH "res/dial"
    #define APP_RECENT_PATH "/res/recent/"
#else
    #define APP_RES_PATH "../../../wearable/res/res"
    #define APP_MUSIC_PATH "../../../wearable/res/music"
    #define APP_VIDEO_PATH "../../../wearable/res/video"
    #define APP_DIAL_PATH "../../../wearable/res/dial"
    #define APP_RECENT_PATH "../../../wearable/res/recent/"
#endif
#endif

    static constexpr const char *CARD_CONFIG_PATH = "/user/cardconfig.bin";

    static constexpr const char *CLOCK_IAMGES = APP_RES_PATH"/clock/clock.bin";
    static constexpr const char *PLAYER_IAMGES = APP_RES_PATH"/player/player.bin";
    static constexpr const char *COMPASS_IAMGES = APP_RES_PATH"/compass/compass.bin";
    static constexpr const char *DROPDOWN_IAMGES = APP_RES_PATH"/dropdown/dropdown.bin";
    static constexpr const char *HEART_IAMGES = APP_RES_PATH"/heart/heart.bin";
    static constexpr const char *PNG_BLUETOOTH_HINTS_PATH = APP_RES_PATH"/BLUETOOTH_HINTS.bin";
    static constexpr const char *PNG_APPLIST_COMPASS_IMAGE = APP_RES_PATH"/APPLIST_COMPASS_IMAGE.bin";

    static constexpr const char *PNG_PROGRESS_BAR_TMAGE = APP_RES_PATH"/PROGRESS_BAR_TMAGE.bin";

    static constexpr const char *PNG_SOUNDBUTTON2 = APP_RES_PATH"/SOUND_IMAGE.bin";
    static constexpr const char *PNG_PULLUP_BUTTON = APP_RES_PATH"/PNG_PULLUP_BUTTON.bin";
    static constexpr const char *MUSIC_2 = APP_RES_PATH"/mp3_48k_caiqing.mp3";
    static constexpr const char *MUSIC_1 = APP_RES_PATH"/one.mp3";

    static constexpr const char *PNG_SETTTING_BUTTON_IMAGE = APP_RES_PATH"/SETTTING_BUTTON.bin";
    static constexpr const char *PNG_BLUETOOTH_IMAGE = APP_RES_PATH"/BLUETOOTH_IMAGE.bin";
    static constexpr const char *PNG_WATCH_IMAGE = APP_RES_PATH"/WATCH_IMAGE.bin";
    static constexpr const char *PNG_PHONE_IMAGE = APP_RES_PATH"/PHONE_IMAGE.bin";

    static constexpr const char *PNG_NEW_DEVICE = APP_RES_PATH"/NEW_DEVICE.bin";
    static constexpr const char *PNG_BT_LIST = APP_RES_PATH"/BT_LIST.bin";
    static constexpr const char *PNG_SELECTION_BOX = APP_RES_PATH"/SELECTION_BOX.bin";
    static constexpr const char *PNG_GANTAN = APP_RES_PATH"/GANTAN.bin";
    static constexpr const char *PNG_RIGHT = APP_RES_PATH"/RIGHT.bin";

    static constexpr const char *PNG_NEW_WLAN_WIFI = APP_RES_PATH"/FULL_SIGNAL_OPEN.bin";
    static constexpr const char *FULL_SIGNAL_ENCRYPTION = APP_RES_PATH"/FULL_SIGNAL_ENCRYPTION.bin";

    static constexpr const char *PNG_MAIN_FACE_IMAGE = APP_RES_PATH"/MAIN_FACE_IMAGE.bin";
    static constexpr const char *B024_009_IMAGE_PATH = APP_RES_PATH"/HOURHAND.bin";
    static constexpr const char *B024_010_IMAGE_PATH = APP_RES_PATH"/SECONDHAND.bin";
    static constexpr const char *B024_011_IMAGE_PATH = APP_RES_PATH"/MINUTEHAND.bin";
    static constexpr const char *ALARM_ADD_BUTTON_PATH = APP_RES_PATH"/BLUE_ADD.bin";
    static constexpr const char *ALARM_CLOCK_CLOSE_ADD_PATH = APP_RES_PATH"/CLOSE_RING_CLOCK.bin";
    static constexpr const char *ALARM_CLOCK_ADD_PATH = APP_RES_PATH"/POP_UP_CLOCK.bin";
    static constexpr const char *ALARM_CLOCK_POP_ADD_PATH = APP_RES_PATH"/RING_CLOCK.bin";
    static constexpr const char *ALARM_DELETE_ADD_PATH = APP_RES_PATH"/DELETE.bin";
    static constexpr const char *ALARM_TRUE_ADD_PATH = APP_RES_PATH"/TRUE_ADD.bin";

    static constexpr const char *COMPASS_CALIBRATE_IMAGE = APP_RES_PATH"/COMPASS_CALIBRATE_IMAGE.bin";
    static constexpr const char *PNG_SETTING_NEXT_IMAGE = APP_RES_PATH"/SETTING_NEXT.bin";

    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE01 = APP_RES_PATH"/VOICE_ASSISTANT_01.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE02 = APP_RES_PATH"/VOICE_ASSISTANT_02.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE03 = APP_RES_PATH"/VOICE_ASSISTANT_03.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE04 = APP_RES_PATH"/VOICE_ASSISTANT_04.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE05 = APP_RES_PATH"/VOICE_ASSISTANT_05.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE06 = APP_RES_PATH"/VOICE_ASSISTANT_06.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE07 = APP_RES_PATH"/VOICE_ASSISTANT_07.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE08 = APP_RES_PATH"/VOICE_ASSISTANT_08.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE09 = APP_RES_PATH"/VOICE_ASSISTANT_09.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE10 = APP_RES_PATH"/VOICE_ASSISTANT_10.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE11 = APP_RES_PATH"/VOICE_ASSISTANT_11.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE12 = APP_RES_PATH"/VOICE_ASSISTANT_12.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE13 = APP_RES_PATH"/VOICE_ASSISTANT_13.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE14 = APP_RES_PATH"/VOICE_ASSISTANT_14.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE15 = APP_RES_PATH"/VOICE_ASSISTANT_15.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE16 = APP_RES_PATH"/VOICE_ASSISTANT_16.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE17 = APP_RES_PATH"/VOICE_ASSISTANT_17.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE18 = APP_RES_PATH"/VOICE_ASSISTANT_18.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE19 = APP_RES_PATH"/VOICE_ASSISTANT_19.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE20 = APP_RES_PATH"/VOICE_ASSISTANT_20.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE21 = APP_RES_PATH"/VOICE_ASSISTANT_21.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE22 = APP_RES_PATH"/VOICE_ASSISTANT_22.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE23 = APP_RES_PATH"/VOICE_ASSISTANT_23.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE24 = APP_RES_PATH"/VOICE_ASSISTANT_24.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE25 = APP_RES_PATH"/VOICE_ASSISTANT_25.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE26 = APP_RES_PATH"/VOICE_ASSISTANT_26.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE27 = APP_RES_PATH"/VOICE_ASSISTANT_27.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE28 = APP_RES_PATH"/VOICE_ASSISTANT_28.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE29 = APP_RES_PATH"/VOICE_ASSISTANT_29.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE30 = APP_RES_PATH"/VOICE_ASSISTANT_30.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE31 = APP_RES_PATH"/VOICE_ASSISTANT_31.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE32 = APP_RES_PATH"/VOICE_ASSISTANT_32.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE33 = APP_RES_PATH"/VOICE_ASSISTANT_33.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE34 = APP_RES_PATH"/VOICE_ASSISTANT_34.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE35 = APP_RES_PATH"/VOICE_ASSISTANT_35.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE36 = APP_RES_PATH"/VOICE_ASSISTANT_36.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE37 = APP_RES_PATH"/VOICE_ASSISTANT_37.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE38 = APP_RES_PATH"/VOICE_ASSISTANT_38.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE39 = APP_RES_PATH"/VOICE_ASSISTANT_39.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE40 = APP_RES_PATH"/VOICE_ASSISTANT_40.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE41 = APP_RES_PATH"/VOICE_ASSISTANT_41.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE42 = APP_RES_PATH"/VOICE_ASSISTANT_42.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE43 = APP_RES_PATH"/VOICE_ASSISTANT_43.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE44 = APP_RES_PATH"/VOICE_ASSISTANT_44.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE45 = APP_RES_PATH"/VOICE_ASSISTANT_45.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE46 = APP_RES_PATH"/VOICE_ASSISTANT_46.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE47 = APP_RES_PATH"/VOICE_ASSISTANT_47.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE48 = APP_RES_PATH"/VOICE_ASSISTANT_48.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE49 = APP_RES_PATH"/VOICE_ASSISTANT_49.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE50 = APP_RES_PATH"/VOICE_ASSISTANT_50.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE51 = APP_RES_PATH"/VOICE_ASSISTANT_51.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE52 = APP_RES_PATH"/VOICE_ASSISTANT_52.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE53 = APP_RES_PATH"/VOICE_ASSISTANT_53.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE54 = APP_RES_PATH"/VOICE_ASSISTANT_54.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE55 = APP_RES_PATH"/VOICE_ASSISTANT_55.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE56 = APP_RES_PATH"/VOICE_ASSISTANT_56.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE57 = APP_RES_PATH"/VOICE_ASSISTANT_57.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE58 = APP_RES_PATH"/VOICE_ASSISTANT_58.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE59 = APP_RES_PATH"/VOICE_ASSISTANT_59.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE60 = APP_RES_PATH"/VOICE_ASSISTANT_60.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE61 = APP_RES_PATH"/VOICE_ASSISTANT_61.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE62 = APP_RES_PATH"/VOICE_ASSISTANT_62.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE63 = APP_RES_PATH"/VOICE_ASSISTANT_63.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE64 = APP_RES_PATH"/VOICE_ASSISTANT_64.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE65 = APP_RES_PATH"/VOICE_ASSISTANT_65.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE66 = APP_RES_PATH"/VOICE_ASSISTANT_66.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE67 = APP_RES_PATH"/VOICE_ASSISTANT_67.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE68 = APP_RES_PATH"/VOICE_ASSISTANT_68.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE69 = APP_RES_PATH"/VOICE_ASSISTANT_69.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE70 = APP_RES_PATH"/VOICE_ASSISTANT_70.bin";
    static constexpr const char *PNG_VOICE_ASSISTANT_IMAGE71 = APP_RES_PATH"/VOICE_ASSISTANT_71.bin";
    static constexpr const char *PNG_APPLIST_VOCASSIST_IMAGE = APP_RES_PATH"/APPLIST_VOCASSIST_IMAGE.bin";

    static constexpr const char *PNG_STEP_SHOE_IMAGE_PATH = APP_RES_PATH"/SHOE_IMAGE.bin";

    static constexpr const char *PNG_A019_080_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_01.bin";
    static constexpr const char *PNG_A019_081_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_02.bin";
    static constexpr const char *PNG_A019_082_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_03.bin";
    static constexpr const char *PNG_A019_083_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_04.bin";
    static constexpr const char *PNG_A019_084_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_05.bin";
    static constexpr const char *PNG_A019_085_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_06.bin";
    static constexpr const char *PNG_A019_086_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_07.bin";
    static constexpr const char *PNG_A019_087_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_08.bin";

    static constexpr const char *PNG_A019_088_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_09.bin";
    static constexpr const char *PNG_A019_089_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_10.bin";
    static constexpr const char *PNG_A019_090_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_11.bin";
    static constexpr const char *PNG_A019_091_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_12.bin";
    static constexpr const char *PNG_A019_092_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_13.bin";
    static constexpr const char *PNG_A019_093_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_14.bin";
    static constexpr const char *PNG_A019_094_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_15.bin";
    static constexpr const char *PNG_A019_095_IMAGE_PATH = APP_RES_PATH"/BLUETOOTH_IMAGE_16.bin";
    static constexpr const char *PNG_BLUETOOTHEAR = APP_RES_PATH"/BLUETOOTHEAR.bin";
    // AppList

    static constexpr const char *PNG_APPLIST_ALARM_IMAGE = APP_RES_PATH"/APPLIST_ALARM.bin"; // 闹钟
    static constexpr const char *PNG_APPLIST_SETTING_IMAGE = APP_RES_PATH"/APPLIST_SETTING.bin";  // 设置

    static constexpr const char *PNG_APPLIST_ALIPAY_IMAGE = APP_RES_PATH"/alipay_logo.bin"; // 支付宝
    static constexpr const char *PNG_ALIPAY_BIND_FAILURE = APP_RES_PATH"/ALIPAY_BIND_FAILURE.bin";
    static constexpr const char *PNG_ALIPAY_BIND_SUCCESS = APP_RES_PATH"/ALIPAY_BIND_SUCCESS.bin";
    static constexpr const char *PNG_ALIPAY_LIST_PAY = APP_RES_PATH"/ALIPAY_LIST_PAY.bin";
    static constexpr const char *PNG_ALIPAY_LIST_TRANS = APP_RES_PATH"/ALIPAY_LIST_TRANS.bin";
    static constexpr const char *PNG_ALIPAY_LIST_SETTING = APP_RES_PATH"/ALIPAY_LIST_SETTING.bin";
    static constexpr const char *PNG_ALIPAY_LIST_HELP = APP_RES_PATH"/ALIPAY_LIST_HELP.bin";
    static constexpr const char *PNG_ALIPAY_PAY_ICON_WHITE = APP_RES_PATH"/ALIPAY_PAY_ICON_WHITE.bin";
    static constexpr const char *PNG_ALIPAY_PAY_ICON_GREY = APP_RES_PATH"/ALIPAY_PAY_ICON_GREY.bin";

    // AppList hexagon icon
    static constexpr const char *PNG_APPLIST_DEFAULT_IMG = APP_RES_PATH"/regular_hexagon.bin";
    static constexpr const char *PNG_APPLIST_NOTSETUSE_IMG = APP_RES_PATH"/regular_hexagon_inverse.bin";

    static constexpr const char *PNG_APPLIST_UI_SAMPLE_IMAGE = APP_RES_PATH"/APPLIST_UI_SAMPLE.bin"; // Applist图标

    static constexpr const char *PNG_SET_CARD_COMPASS = APP_RES_PATH"/CARD_COMPASS.bin";
    static constexpr const char *PNG_SET_CARD_ACTIVITY = APP_RES_PATH"/CARD_ACTIVITY.bin";
    static constexpr const char *PNG_SET_CARD_PLAYER = APP_RES_PATH"/CARD_PLAYER.bin";
    static constexpr const char *PNG_SET_CARD_VIDEO = APP_RES_PATH"/noaudio_5_set.bin";
    static constexpr const char *PNG_PREVIEW_CARD_VIDEO = APP_RES_PATH"/noaudio_05.bin";

    static constexpr const char *PNG_SET_DIAL_CLOCK = APP_RES_PATH"/DIAL_CLOCK.bin";

    static constexpr const char *SPORT_ICON = APP_RES_PATH"/SPORT_ICON.bin";
    static constexpr const char *SPORT_IMAGE = APP_RES_PATH"/SPORT.bin";
    static constexpr const char *STOPWATCH_ICON = APP_RES_PATH"/STOPWATCH_ICON.bin";
    static constexpr const char *STOPWATCH_IMAGE = APP_RES_PATH"/STOPWATCH.bin";
    static constexpr const char *WEATHER_IMAGE = APP_RES_PATH"/WEATHER.bin";
    static constexpr const char *WEATHER_ICON = APP_RES_PATH"/WEATHER_ICON.bin";
    static constexpr const char *TIMER_IMAGE = APP_RES_PATH"/TIMER.bin";
    static constexpr const char *TIMER_ICON = APP_RES_PATH"/TIMER_ICON.bin";
    static constexpr const char *DATE_IMAGE = APP_RES_PATH"/DATE.bin";
    static constexpr const char *DATE_ICON = APP_RES_PATH"/DATE_ICON.bin";
    static constexpr const char *FLASHLIGHT_IMAGE = APP_RES_PATH"/FLASHLIGHT.bin";
    static constexpr const char *FLASHLIGHT_ICON = APP_RES_PATH"/FLASHLIGHT_ICON.bin";
    static constexpr const char *SLEEP_IMAGE = APP_RES_PATH"/SLEEP.bin";
    static constexpr const char *SLEEP_ICON = APP_RES_PATH"/SLEEP_ICON.bin";
    static constexpr const char *BREATH_IMAGE = APP_RES_PATH"/BREATH.bin";
    static constexpr const char *BREATH_ICON = APP_RES_PATH"/BREATH_ICON.bin";
    static constexpr const char *FINDPHONE_IMAGE = APP_RES_PATH"/FINDPHONE.bin";
    static constexpr const char *FINDPHONE_ICON = APP_RES_PATH"/FINDPHONE_ICON.bin";
    static constexpr const char *CAMERA_IMAGE = APP_RES_PATH"/CAMERA.bin";
    static constexpr const char *CAMERA_ICON = APP_RES_PATH"/CAMERA_ICON.bin";
    static constexpr const char *PHONEMENU_IMAGE = APP_RES_PATH"/PHONEMENU.bin";
    static constexpr const char *PHONEMENU_ICON = APP_RES_PATH"/PHONEMENU_ICON.bin";
    static constexpr const char *HEART_RATE_IMAGE = APP_RES_PATH"/HEART_RATE.bin";
    static constexpr const char *HEART_RATE_ICON = APP_RES_PATH"/HEART_RATE_ICON.bin";
    static constexpr const char *MESSAGE_IMAGE = APP_RES_PATH"/MESSAGE.bin";
    static constexpr const char *BLOOD_IMAGE = APP_RES_PATH"/BLOOD.bin";
    static constexpr const char *BLOOD_ICON = APP_RES_PATH"/BLOOD_ICON.bin";
    static constexpr const char *SETTING_IMAGE = APP_RES_PATH"/SETTING.bin";
    static constexpr const char *SETTING_ICON = APP_RES_PATH"/SETTING_ICON.bin";
    static constexpr const char *EMPTY_IMAGE = APP_RES_PATH"/EMPTY.bin";
    static constexpr const char *EMPTY_ICON = APP_RES_PATH"/EMPTY_ICON.bin";
    static constexpr const char *MONKEY_ICON = APP_RES_PATH"/MONKEY_ICON.bin";

    static constexpr const char *PNG_SET_GRADIENT_CLOCK = APP_RES_PATH"/GRADIENT_CLOCK.bin";
    static constexpr const char *PNG_TRANSITION_SLICE_SAMPLE = APP_RES_PATH"/TRANSITION_SLICE_SAMPLE.bin";
}

#endif // UI_CONFIG_H
