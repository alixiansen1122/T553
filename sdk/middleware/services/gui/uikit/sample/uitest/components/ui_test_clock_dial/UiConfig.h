/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UI_CONFIG_H
#define UI_CONFIG_H

#include <cstdint>
#include "graphic_config.h"

namespace OHOS {
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

    constexpr int16_t SCREEN_HORIZONTAL_RESOLUTION = 454; // watch width
    constexpr int16_t SCREEN_VERTICAL_RESOLUTION = 454; // watch width

    constexpr const char *CLOCK_IAMGES = RES_PATH"/clock.bin";
    constexpr const char *PLAYER_IAMGES = RES_PATH"/player/player.bin";
    constexpr const char *COMPASS_IAMGES = RES_PATH"/compass.bin";
    constexpr const char *DROPDOWN_IAMGES = RES_PATH"/dropdown.bin";
    constexpr const char *HEART_IAMGES = RES_PATH"/heart.bin";
    constexpr const char *PNG_BLUETOOTH_HINTS_PATH = RES_PATH"/BLUETOOTH_HINTS.bin";
    constexpr const char *PNG_BLOODOXYGEN_IMAGE_PATH = RES_PATH"/BLOODOXYGEN_VIEW.bin";

    constexpr const char *PNG_BLOOD_ICON_ENTER = RES_PATH"/BLOOD_ICON_ENTER.bin";

    constexpr const char *PNG_AGREEING_IMAGE_PATH = RES_PATH"/AGREEING.bin";
    constexpr const char *PNG_DISSGREE_IMAGE_PATH = RES_PATH"/DISSGREE.bin";
    constexpr const char *PNG_MICROPHONE_OPEN_IMAGE_PATH = RES_PATH"/MICRO_PHONE_OPEN.bin";
    constexpr const char *PNG_MICROPHONE_CLOSE_IMAGE_PATH = RES_PATH"/MICRO_PHONE_CLOSE.bin";
    constexpr const char *PNG_HANDUP_IMAGE_PATH = RES_PATH"/HANDUP.bin";
    constexpr const char *PNG_ANSWER_TMAGE_PATH = RES_PATH"/ANSWER_PHONE.bin";
    constexpr const char *PNG_CALLING_TMAGE_PATH = RES_PATH"/CALLING_PHONE.bin";
    constexpr const char *PNG_CALLER_TMAGE_PATH = RES_PATH"/CALLER_LOG_IMAGE.bin";
    constexpr const char *PNG_NO_CALLER_LOG_IMAGE = RES_PATH"/NO_CALLER_LOG_IMAGE.bin";
    constexpr const char *PNG_APPLIST_CALLER_IMAGE = RES_PATH"/APPLIST_CALLER_IMAGE.bin";
    constexpr const char *PNG_APPLIST_COMPASS_IMAGE = RES_PATH"/APPLIST_COMPASS_IMAGE.bin";

    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_0 = RES_PATH"/COUNTDOWN_IMAGE_01.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_1 = RES_PATH"/COUNTDOWN_IMAGE_02.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_2 = RES_PATH"/COUNTDOWN_IMAGE_03.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_3 = RES_PATH"/COUNTDOWN_IMAGE_04.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_4 = RES_PATH"/COUNTDOWN_IMAGE_05.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_5 = RES_PATH"/COUNTDOWN_IMAGE_06.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_6 = RES_PATH"/COUNTDOWN_IMAGE_07.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_7 = RES_PATH"/COUNTDOWN_IMAGE_08.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_8 = RES_PATH"/COUNTDOWN_IMAGE_09.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_9 = RES_PATH"/COUNTDOWN_IMAGE_10.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_10 = RES_PATH"/COUNTDOWN_IMAGE_11.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_11 = RES_PATH"/COUNTDOWN_IMAGE_12.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_12 = RES_PATH"/COUNTDOWN_IMAGE_13.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_13 = RES_PATH"/COUNTDOWN_IMAGE_14.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_14 = RES_PATH"/COUNTDOWN_IMAGE_15.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_15 = RES_PATH"/COUNTDOWN_IMAGE_16.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_16 = RES_PATH"/COUNTDOWN_IMAGE_17.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_17 = RES_PATH"/COUNTDOWN_IMAGE_18.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_18 = RES_PATH"/COUNTDOWN_IMAGE_19.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_19 = RES_PATH"/COUNTDOWN_IMAGE_20.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_20 = RES_PATH"/COUNTDOWN_IMAGE_21.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_21 = RES_PATH"/COUNTDOWN_IMAGE_22.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_22 = RES_PATH"/COUNTDOWN_IMAGE_23.bin";
    constexpr const char *PNG_COUNTDOWN_TIME_IMAGE_23 = RES_PATH"/COUNTDOWN_IMAGE_24.bin";

    constexpr const char *PNG_PROGRESS_BAR_TMAGE = RES_PATH"/PROGRESS_BAR_TMAGE.bin";
    constexpr const char *TEMP_WAITING = RES_PATH"/TEMP_WAITING.bin";
    constexpr const char *TEMP_REPORT = RES_PATH"/TEMP_REPORT.bin";
    constexpr const char *TEMP_POINT = RES_PATH"/TEMP_POINT.bin";
    constexpr const char *TEMP_REV_POINTER = RES_PATH"/TEMP_REV_POINTER.bin";

    constexpr const char *PNG_BLOOD_FLOW_IMAGE_0 = RES_PATH"/BLOOD_FLOW_IMAGE_02.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_1 = RES_PATH"/BLOOD_FLOW_IMAGE_03.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_2 = RES_PATH"/BLOOD_FLOW_IMAGE_04.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_3 = RES_PATH"/BLOOD_FLOW_IMAGE_05.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_4 = RES_PATH"/BLOOD_FLOW_IMAGE_06.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_5 = RES_PATH"/BLOOD_FLOW_IMAGE_07.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_6 = RES_PATH"/BLOOD_FLOW_IMAGE_08.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_7 = RES_PATH"/BLOOD_FLOW_IMAGE_09.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_8 = RES_PATH"/BLOOD_FLOW_IMAGE_10.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_9 = RES_PATH"/BLOOD_FLOW_IMAGE_11.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_10 = RES_PATH"/BLOOD_FLOW_IMAGE_12.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_11 = RES_PATH"/BLOOD_FLOW_IMAGE_13.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_12 = RES_PATH"/BLOOD_FLOW_IMAGE_14.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_13 = RES_PATH"/BLOOD_FLOW_IMAGE_15.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_14 = RES_PATH"/BLOOD_FLOW_IMAGE_16.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_15 = RES_PATH"/BLOOD_FLOW_IMAGE_17.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_16 = RES_PATH"/BLOOD_FLOW_IMAGE_18.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_17 = RES_PATH"/BLOOD_FLOW_IMAGE_19.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_18 = RES_PATH"/BLOOD_FLOW_IMAGE_20.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_19 = RES_PATH"/BLOOD_FLOW_IMAGE_21.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_20 = RES_PATH"/BLOOD_FLOW_IMAGE_22.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_21 = RES_PATH"/BLOOD_FLOW_IMAGE_23.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_22 = RES_PATH"/BLOOD_FLOW_IMAGE_24.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_23 = RES_PATH"/BLOOD_FLOW_IMAGE_25.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_24 = RES_PATH"/BLOOD_FLOW_IMAGE_26.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_25 = RES_PATH"/BLOOD_FLOW_IMAGE_27.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_26 = RES_PATH"/BLOOD_FLOW_IMAGE_28.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_27 = RES_PATH"/BLOOD_FLOW_IMAGE_29.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_28 = RES_PATH"/BLOOD_FLOW_IMAGE_30.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_29 = RES_PATH"/BLOOD_FLOW_IMAGE_31.bin";
    constexpr const char *PNG_BLOOD_FLOW_IMAGE_30 = RES_PATH"/BLOOD_FLOW_IMAGE_32.bin";

    constexpr const char *PNG_BK_FACE_IMAGE = RES_PATH"/BACK_GROUND.bin";
    constexpr const char *PNG_PLAYER_FACE_IMAGE = RES_PATH"/PLAYER.bin";
    constexpr const char *PNG_PLAYER_HL_FACE_IMAGE = RES_PATH"/PLAYERHIGHLIGHT.bin";
    constexpr const char *PNG_NEXTBUTTON_FACE_IMAGE = RES_PATH"/NEXT_BUTTON.bin";
    constexpr const char *PNG_PREBUTTON_FACE_IMAGE = RES_PATH"/PRE_BUTTOW.bin";
    constexpr const char *PNG_PLAYERS_MODE_1 = RES_PATH"/PLAYERSMODEL_LOOP.bin";
    constexpr const char *PNG_PLAYERS_MODE_2 = RES_PATH"/PLAYERSMODEL_REPEATS.bin";
    constexpr const char *PNG_PLAYERS_MODE_3 = RES_PATH"/PLAYERSMODEL_RANDOM.bin";
    constexpr const char *PNG_PLAYERS_MODE_4 = RES_PATH"/PLAYERSMODEL_SEQUENCE.bin";
    constexpr const char *PNG_SOUNDBUTTON2 = RES_PATH"/SOUND_IMAGE.bin";
    constexpr const char *PNG_OPTIONSBUTTON_FACE_IMAGE = RES_PATH"/OPTIONS_IMAGE.bin";
    constexpr const char *PNG_PULLBUTTON_FACE_IMAGE = RES_PATH"/BLOOD_FLOW_IMAGE_01.bin";
    constexpr const char *PNG_MUTE_BUTTON = RES_PATH"/MUTE_BUTTON.bin";
    constexpr const char *PNG_DECREASE_BUTTON = RES_PATH"/DECREASE_BUTTON.bin";
    constexpr const char *PNG_INCREASE_BUTTON = RES_PATH"/INCREASE_BUTTON.bin";

    constexpr const char *PNG_SETTTING_BUTTON_IMAGE = RES_PATH"/SETTTING_BUTTON.bin";
    constexpr const char *PNG_BLUETOOTH_IMAGE = RES_PATH"/BLUETOOTH_IMAGE.bin";
    constexpr const char *PNG_WATCH_IMAGE = RES_PATH"/WATCH_IMAGE.bin";
    constexpr const char *PNG_PHONE_IMAGE = RES_PATH"/PHONE_IMAGE.bin";
    constexpr const char *PNG_REBOOT_BUTTON_IMAGE = RES_PATH"/REBOOT_BUTTON.bin";
    constexpr const char *PNG_SHUTDOWN_BUTTON_IMAGE = RES_PATH"/SHUTDOWN_BUTTON.bin";
    constexpr const char *PNG_SETTTING_REBOOT_IMAGE = RES_PATH"/SETTTING_REBOOT.bin";
    constexpr const char *PNG_SETTTING_SHUTDOWN_IMAGE = RES_PATH"/SETTTING_SHUTDOWN.bin";

    constexpr const char *PNG_CONNECT_PHONE_IMAGE = RES_PATH"/CONNECT_PHONE.bin";
    constexpr const char *PNG_BLUETOOTH_STITLE = RES_PATH"/BLUETOOTH_TITLE.bin";

    constexpr const char *PNG_NEW_DEVICE = RES_PATH"/NEW_DEVICE.bin";
    constexpr const char *PNG_BT_LIST = RES_PATH"/BT_LIST.bin";
    constexpr const char *PNG_SELECTION_BOX = RES_PATH"/SELECTION_BOX.bin";
    constexpr const char *PNG_GANTAN = RES_PATH"/GANTAN.bin";
    constexpr const char *PNG_RIGHT = RES_PATH"/RIGHT.bin";

    constexpr const char *PNG_NEW_WLAN_WIFI = RES_PATH"/FULL_SIGNAL_OPEN.bin";
    constexpr const char *FULL_SIGNAL_ENCRYPTION = RES_PATH"/FULL_SIGNAL_ENCRYPTION.bin";

    #define PNG_MAIN_FACE_IMAGE RES_PATH"/MAIN_FACE_IMAGE.bin"
    #define B024_009_IMAGE_PATH RES_PATH"/HOURHAND.bin"
    #define B024_010_IMAGE_PATH RES_PATH"/SECONDHAND.bin"
    #define B024_011_IMAGE_PATH RES_PATH"/MINUTEHAND.bin"
    #define ALARM_ADD_BUTTON_PATH RES_PATH"/BLUE_ADD.bin"
    #define ALARM_CLOCK_CLOSE_ADD_PATH RES_PATH"/CLOSE_RING_CLOCK.bin"
    #define ALARM_CLOCK_ADD_PATH RES_PATH"/POP_UP_CLOCK.bin"
    #define ALARM_CLOCK_POP_ADD_PATH RES_PATH"/RING_CLOCK.bin"
    #define ALARM_DELETE_ADD_PATH RES_PATH"/DELETE.bin"
    #define ALARM_TRUE_ADD_PATH RES_PATH"/TRUE_ADD.bin"
    #define TEMP_MEASURE RES_PATH"/TEMP_MEASURE.bin"
    #define TEMP_UP_DROP RES_PATH"/TEMP_UP_DROP.bin"
    #define TEMP_EXIT RES_PATH"/TEMP_EXIT.bin"
    #define TEMP_ROTATE RES_PATH"/TEMP_ROTATE.bin"
    #define TEMP_ICON_ENTER RES_PATH"/TEMP_ICON_ENTER.bin"
    #define TEMP_BACKGROUND RES_PATH"/TEMP_BACKGROUND.bin"
    #define TEMP_WARN RES_PATH"/TEMP_WARN.bin"
    #define TEMP_LITTLE_MEASURE RES_PATH"/TEMP_LITTLE_MEASURE.bin"
    #define TEMP_HOT RES_PATH"/TEMP_HOT.bin"
    #define TEMP_COLD RES_PATH"/TEMP_COLD.bin"
    #define TEMP_LIT_TIP RES_PATH"/TEMP_LIT_TIP.bin"
    #define TEMP_ABOUT_STUDY RES_PATH"/TEMP_ABOUT_STUDY.bin"
    #define TEMP_TRUE RES_PATH"/TEMP_TRUE.bin"
    #define TEMP_DELETE RES_PATH"/TEMP_DELETE.bin"

    #define PNG_COMPASS_FACE_IMAGE RES_PATH"/COMPASS.bin"
    #define PNG_COMPASS_DEGREE_IMAGE RES_PATH"/CORRESPONDENCE_DEGREE.bin"
    #define PNG_COMPASS_CENTER_IMAGE RES_PATH"/ROTATE_CENTER_IMAGE.bin"
    #define COMPASS_CALIBRATE_IMAGE RES_PATH"/COMPASS_CALIBRATE_IMAGE.bin"

    #define PNG_HEARTRATE_IMAGE_PATH RES_PATH"/HEARTRATE_IMAGE_PATH.bin"
    #define PNG_HAND24HOUR_IMAGE_PATH RES_PATH"/HAND24HOUR_IMAGE_PATH.bin"
    #define PNG_PROGRESS_IMAGE_PATH RES_PATH"/PROGRESS_IMAGE_PATH.bin"
    #define PNG_BIGCIRCLE_IMAGE_PATH RES_PATH"/BIGCIRCLE_IMAGE_PATH.bin"
    #define PNG_MEDCIRCLE_IMAGE_PATH RES_PATH"/MEDCIRCLE_IMAGE_PATH.bin"
    #define PNG_SMALLCIRCLE_IMAGE_PATH RES_PATH"/SMALLCIRCLE_IMAGE_PATH.bin"
    #define PNG_MONDAY_IMAGE_PATH RES_PATH"/MONDAY_IMAGE_PATH.bin"
    #define PNG_TUESDAY_IMAGE_PATH RES_PATH"/TUESDAY_IMAGE_PATH.bin"
    #define PNG_WEDNESDAY_IMAGE_PATH RES_PATH"/WEDNESDAY_IMAGE_PATH.bin"
    #define PNG_THURSDAY_IMAGE_PATH RES_PATH"/THURSDAY_IMAGE_PATH.bin"
    #define PNG_FIRDAY_IMAGE_PATH RES_PATH"/FIRDAY_IMAGE_PATH.bin"
    #define PNG_SATURDAY_IMAGE_PATH RES_PATH"/SATURDAY_IMAGE_PATH.bin"
    #define PNG_SUNDAY_IMAGE_PATH RES_PATH"/SUNDAY_IMAGE_PATH.bin"
    #define PNG_CIRCLE_IMAGE_PATH RES_PATH"/CIRCLE_IMAGE_PATH.bin"

    #define PNG_SETTING_NEXT_IMAGE RES_PATH"/SETTING_NEXT.bin"

    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE01 = RES_PATH"/VOICE_ASSISTANT_01.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE02 = RES_PATH"/VOICE_ASSISTANT_02.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE03 = RES_PATH"/VOICE_ASSISTANT_03.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE04 = RES_PATH"/VOICE_ASSISTANT_04.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE05 = RES_PATH"/VOICE_ASSISTANT_05.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE06 = RES_PATH"/VOICE_ASSISTANT_06.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE07 = RES_PATH"/VOICE_ASSISTANT_07.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE08 = RES_PATH"/VOICE_ASSISTANT_08.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE09 = RES_PATH"/VOICE_ASSISTANT_09.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE10 = RES_PATH"/VOICE_ASSISTANT_10.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE11 = RES_PATH"/VOICE_ASSISTANT_11.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE12 = RES_PATH"/VOICE_ASSISTANT_12.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE13 = RES_PATH"/VOICE_ASSISTANT_13.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE14 = RES_PATH"/VOICE_ASSISTANT_14.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE15 = RES_PATH"/VOICE_ASSISTANT_15.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE16 = RES_PATH"/VOICE_ASSISTANT_16.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE17 = RES_PATH"/VOICE_ASSISTANT_17.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE18 = RES_PATH"/VOICE_ASSISTANT_18.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE19 = RES_PATH"/VOICE_ASSISTANT_19.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE20 = RES_PATH"/VOICE_ASSISTANT_20.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE21 = RES_PATH"/VOICE_ASSISTANT_21.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE22 = RES_PATH"/VOICE_ASSISTANT_22.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE23 = RES_PATH"/VOICE_ASSISTANT_23.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE24 = RES_PATH"/VOICE_ASSISTANT_24.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE25 = RES_PATH"/VOICE_ASSISTANT_25.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE26 = RES_PATH"/VOICE_ASSISTANT_26.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE27 = RES_PATH"/VOICE_ASSISTANT_27.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE28 = RES_PATH"/VOICE_ASSISTANT_28.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE29 = RES_PATH"/VOICE_ASSISTANT_29.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE30 = RES_PATH"/VOICE_ASSISTANT_30.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE31 = RES_PATH"/VOICE_ASSISTANT_31.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE32 = RES_PATH"/VOICE_ASSISTANT_32.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE33 = RES_PATH"/VOICE_ASSISTANT_33.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE34 = RES_PATH"/VOICE_ASSISTANT_34.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE35 = RES_PATH"/VOICE_ASSISTANT_35.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE36 = RES_PATH"/VOICE_ASSISTANT_36.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE37 = RES_PATH"/VOICE_ASSISTANT_37.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE38 = RES_PATH"/VOICE_ASSISTANT_38.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE39 = RES_PATH"/VOICE_ASSISTANT_39.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE40 = RES_PATH"/VOICE_ASSISTANT_40.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE41 = RES_PATH"/VOICE_ASSISTANT_41.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE42 = RES_PATH"/VOICE_ASSISTANT_42.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE43 = RES_PATH"/VOICE_ASSISTANT_43.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE44 = RES_PATH"/VOICE_ASSISTANT_44.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE45 = RES_PATH"/VOICE_ASSISTANT_45.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE46 = RES_PATH"/VOICE_ASSISTANT_46.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE47 = RES_PATH"/VOICE_ASSISTANT_47.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE48 = RES_PATH"/VOICE_ASSISTANT_48.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE49 = RES_PATH"/VOICE_ASSISTANT_49.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE50 = RES_PATH"/VOICE_ASSISTANT_50.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE51 = RES_PATH"/VOICE_ASSISTANT_51.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE52 = RES_PATH"/VOICE_ASSISTANT_52.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE53 = RES_PATH"/VOICE_ASSISTANT_53.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE54 = RES_PATH"/VOICE_ASSISTANT_54.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE55 = RES_PATH"/VOICE_ASSISTANT_55.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE56 = RES_PATH"/VOICE_ASSISTANT_56.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE57 = RES_PATH"/VOICE_ASSISTANT_57.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE58 = RES_PATH"/VOICE_ASSISTANT_58.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE59 = RES_PATH"/VOICE_ASSISTANT_59.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE60 = RES_PATH"/VOICE_ASSISTANT_60.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE61 = RES_PATH"/VOICE_ASSISTANT_61.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE62 = RES_PATH"/VOICE_ASSISTANT_62.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE63 = RES_PATH"/VOICE_ASSISTANT_63.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE64 = RES_PATH"/VOICE_ASSISTANT_64.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE65 = RES_PATH"/VOICE_ASSISTANT_65.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE66 = RES_PATH"/VOICE_ASSISTANT_66.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE67 = RES_PATH"/VOICE_ASSISTANT_67.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE68 = RES_PATH"/VOICE_ASSISTANT_68.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE69 = RES_PATH"/VOICE_ASSISTANT_69.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE70 = RES_PATH"/VOICE_ASSISTANT_70.bin";
    constexpr const char *PNG_VOICE_ASSISTANT_IMAGE71 = RES_PATH"/VOICE_ASSISTANT_71.bin";
    constexpr const char *PNG_APPLIST_VOCASSIST_IMAGE = RES_PATH"/APPLIST_VOCASSIST_IMAGE.bin";

    constexpr const char *PNG_STEP_SHOE_IMAGE_PATH = RES_PATH"/SHOE_IMAGE.bin";

    #define PNG_HEARTRATECOOR_IMAGE_PATH RES_PATH"/HEARTRATECOOR.bin"
    #define PNG_A004_009_IMAGE_PATH RES_PATH"/HEARTRATE_TOP.bin"
    #define PNG_A004_006_IMAGE_PATH RES_PATH"/HEARTRATE_BOTTOM.bin"
    #define PNG_A004_070_IMAGE_PATH RES_PATH"/HEARTBEAT_IMAGE_01.bin"
    #define PNG_A004_071_IMAGE_PATH RES_PATH"/HEARTBEAT_IMAGE_02.bin"

    constexpr const char *PNG_A019_080_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_01.bin";
    constexpr const char *PNG_A019_081_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_02.bin";
    constexpr const char *PNG_A019_082_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_03.bin";
    constexpr const char *PNG_A019_083_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_04.bin";
    constexpr const char *PNG_A019_084_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_05.bin";
    constexpr const char *PNG_A019_085_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_06.bin";
    constexpr const char *PNG_A019_086_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_07.bin";
    constexpr const char *PNG_A019_087_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_08.bin";

    constexpr const char *PNG_A019_088_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_09.bin";
    constexpr const char *PNG_A019_089_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_10.bin";
    constexpr const char *PNG_A019_090_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_11.bin";
    constexpr const char *PNG_A019_091_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_12.bin";
    constexpr const char *PNG_A019_092_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_13.bin";
    constexpr const char *PNG_A019_093_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_14.bin";
    constexpr const char *PNG_A019_094_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_15.bin";
    constexpr const char *PNG_A019_095_IMAGE_PATH = RES_PATH"/BLUETOOTH_IMAGE_16.bin";
    constexpr const char *PNG_BLUETOOTHEAR = RES_PATH"/BLUETOOTHEAR.bin";
    constexpr const char *PNG_STANDBY_DIAL_ANTLERS = RES_PATH"/STANDBY_DIAL_ANTLERS.bin";
    // AppList

    constexpr const char *PNG_APPLIST_ALARM_IMAGE = RES_PATH"/APPLIST_ALARM.bin"; // 闹钟
    constexpr const char *PNG_APPLIST_SPO2_IMAGE = RES_PATH"/APPLIST_SPO2.bin";  // 血氧
    constexpr const char *PNG_APPLIST_SETTING_IMAGE = RES_PATH"/APPLIST_SETTING.bin";  // 设置
    constexpr const char *PNG_MAIN_DROP_DOWN_DNDISTURB = RES_PATH"/DROPDOWN_DN_DISTURB.bin"; // 勿扰
    constexpr const char *PNG_MAIN_DROP_DOWN_BRISCREEN = RES_PATH"/DROPDOWN_BRI_SCREEN.bin"; // 亮屏
    constexpr const char *PNG_MAIN_DROP_DOWN_FINPHONE = RES_PATH"/DROPDOWN_FIND_PHONE.bin"; // 找手机
    constexpr const char *PNG_MAIN_DROP_DOWN_ALARM = RES_PATH"/DROPDOWN_ALARM.bin"; // 闹钟
    constexpr const char *PNG_MAIN_DROP_DOWN_SETTING = RES_PATH"/DROPDOWN_SETTING.bin"; // 设置
    constexpr const char *PNG_MAIN_DROP_DOWN_BLUETOOTH = RES_PATH"/DROPDOWN_BLUETOOTH.bin"; // 蓝牙状态
    constexpr const char *PNG_MAIN_DROP_DOWN_BATTERY = RES_PATH"/DROPDOWN_BATTERY.bin"; // 电量显示
    constexpr const char *PNG_APPLIST_TEMP_IMAGE = RES_PATH"/TEMP_IMAGE.bin"; // 体温

    constexpr const char *PNG_MAIN_DROP_DOWN_DNDISTURB_B = RES_PATH"/DROPDOWN_DN_DISTURB_B.bin";
    constexpr const char *PNG_MAIN_DROP_DOWN_BRISCREEN_B = RES_PATH"/DROPDOWN_BRI_SCREEN_B.bin";
    constexpr const char *PNG_MAIN_DROP_DOWN_FINPHONE_B = RES_PATH"/DROPDOWN_FIND_PHONE_B.bin";
    constexpr const char *PNG_MAIN_DROP_DOWN_ALARM_B = RES_PATH"/DROPDOWN_ALARM_B.bin";
    constexpr const char *PNG_MAIN_DROP_DOWN_SETTING_B = RES_PATH"/DROPDOWN_SETTING_B.bin";
    constexpr const char *PNG_MAIN_DROP_DOWN_BLUETOOTH_B = RES_PATH"/DROPDOWN_BLUETOOTH_B.bin";
}

#endif // UI_CONFIG_H
