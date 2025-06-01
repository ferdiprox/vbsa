//
// Created by ferdinand on 5/31/25.
//

#include <bits/std_function.h>

#include "vvm/VangersVirtualMachine.h"
#include "vvm/IscreenVirtualUnit.h"

void VangersVirtualMachine::oftrInstructionStep() {
    switch (readInstruction()) {
        default:
            if (instructionReg > OftrInstr::MAX_OPTION) {
                error("Unknown oftr instruction: " + verboseInstruction(instructionReg));
                break;
            }
            VVM_NO_SUPPORT("such instruction: " + verboseInstruction(instructionReg));
            break;
        case EC_SCALE_OBJ:
        case EC_SMOOTH_OBJ:
        case EC_NEXT_TEXT:
        case EC_PREV_TEXT:
        case EC_INPUT_STRING:
        case EC_SET_TRIGGER:
            mainIvu.assertCurrentObject(BI_COMMAND);
            readString();
            readNumber();
            break;
        case EC_EXIT:
        case EC_START_TIME:
        case EC_LINE_ID:
        case EC_SET_LINE_ID:
        case EC_SEND2ACTINT:
        case EC_MOVE_SCREEN:
            mainIvu.assertCurrentObject(BI_COMMAND);
            readNumber();
            break;
        case EC_MOVE2SCREEN: // TODO: here vvm can track used screens and after EOF flag check if all required screens are exists
        case EC_RISE_OBJ:
        case EC_LOW_OBJ:
        case EC_HIDE_OBJ:
        case EC_JUMP_OBJ:
        case EC_TRIGGER_CHANGE:
        case EC_SCROLLER_CHANGE:
        case EC_PREV_TRIGGER:
            mainIvu.assertCurrentObject(BI_COMMAND);
            readString();
            break;
        case EC_EXT_EVENT:
            mainIvu.assertCurrentObject(BI_COMMAND);
            readNumber();
            readNumber();
            break;
        case EC_RESET_TEXTS:
        case EC_END_EVENT:
        case EC_CHECK_CREDITS:
        case EC_EXIT_EVENT:
            mainIvu.assertCurrentObject(BI_COMMAND);
            break;
        case I_SET_NUM_FONTS:
            readNumber();
            break;
        case SD_SET_DEFAULT_SCR:
            readString();
            break;
        case I_NEW_COMMAND:
            mainIvu.openNewObject(I_COMMAND);
            break;
        case I_NEW_ELEMENT:
            mainIvu.openNewObject(I_ELEMENT);
            oftrProcessElement();
            break;
        case EL_TERRAIN:
            mainIvu.openNewObject(I_TERR_ELEMENT);
            break;
        case I_NEW_EVENT:
            mainIvu.openNewObject(I_EVENT);
            break;
        case I_NEW_SCREEN:
            mainIvu.openNewObject(I_SCREEN);
            readString();
            break;
        case I_NEW_TRIGGER_OBJECT:
            mainIvu.openNewObject(I_TRIGGER);
            readString();
            break;
        case I_NEW_OBJECT:
            mainIvu.openNewObject(I_OBJECT);
            readString();
            break;
        case I_END_BLOCK:
            mainIvu.closeObject();
            break;
        case EL_SET_KEY_FLAG:
        case EL_SET_NUMBER_FLAG:
        case EL_SET_JOYSTICK_FLAG:
        case EL_SET_SCALE_NULL:
        case EL_SET_HIDDEN_FON:
        case EL_SET_NO_SCALE:
            mainIvu.assertCurrentObject(BI_ELEMENT);
            break;
        case EL_SET_TERRAIN:
        case EL_INIT_BMP_LEVEL:
        case EL_INIT_DIR:
        case EL_INIT_VALUE:
        case EL_INIT_MAX_VALUE:
            mainIvu.assertCurrentObject(BI_ELEMENT);
            readNumber();
            break;
        case EL_SET_ID:
            mainIvu.assertCurrentObject(BI_ELEMENT | BI_TERR_ELEMENT);
            readString();
            break;
        case EL_INIT_SCALE:
            mainIvu.assertCurrentObject(BI_TERR_ELEMENT);
            readNumber();
            break;
        case EL_INIT_FNAME:
        case EL_INIT_AVINAME:
        case EL_AVI_BORDER_NAME:
        case EL_AVI_IBS_NAME:
        case EL_INIT_STRING:
            // TODO: need to track current element's type too
            mainIvu.assertCurrentObject(BI_ELEMENT);
            readString();
            break;
        case EL_AVI_BORDER_TYPE:
            mainIvu.assertCurrentObject(BI_ELEMENT);
            readInstruction();
            break;
        case EL_PICTURE_INIT:
            mainIvu.assertCurrentObject(BI_ELEMENT);
            break;
        case EL_AVI_BORDER_NULL_LEVEL:
        case EL_AVI_BORDER_LEVEL:
        case EL_AVI_BORDER_SIZE:
            mainIvu.assertCurrentObject(BI_ELEMENT);
            readNumber();
            break;
        case EL_INIT_PALNAME:
            mainIvu.assertCurrentObject(BI_ELEMENT | BI_SCREEN);
            readString();
            break;
        case OBJ_INIT_X:
        case OBJ_INIT_Y:
        case OBJ_INIT_SX:
        case OBJ_INIT_SY:
        case EL_SET_ALIGN_OFFS_X:
        case EL_SET_ALIGN_OFFS_Y:
            mainIvu.assertCurrentObject(BI_OBJECT | BI_ELEMENT | BI_TERR_ELEMENT);
            readNumber();
            break;
        case EL_SET_ALIGN_X:
        case EL_SET_ALIGN_Y:
            mainIvu.assertCurrentObject(BI_OBJECT | BI_ELEMENT | BI_TERR_ELEMENT);
            readInstruction();
            break;
        case EL_INIT_FONT:
        case EL_INIT_SPACE:
        case EL_INIT_LEVEL:
            readNumber();
            break;
        case EL_INIT_TEXT:
        case EL_INIT_S_TEXT:
        case OBJ_SET_PROMPT_TEXT:
            mainIvu.assertCurrentObject(BI_OBJECT);
            readString();
            break;
        case OBJ_SET_PROMPT_ID:
        case OBJ_SET_SHADOW_SIZE:
            mainIvu.assertCurrentObject(BI_OBJECT);
            readNumber();
            break;
        case OBJ_SET_NO_FON:
        case OBJ_SET_UNLOCK:
        case OBJ_SET_SELECT:
            mainIvu.assertCurrentObject(BI_OBJECT);
            break;
        case SD_NEW_TEXT:
            readNumber();
            readString();
            readString();
            break;
        case EV_KEY:
        case EV_CODE:
            mainIvu.assertCurrentObject(BI_EVENT);
            readNumber();
            break;
        case EV_SELECTED:
        case EV_NOT_SELECTED:
        case EV_LOCKED:
        case EV_SET_MULTILINE:
            mainIvu.assertCurrentObject(BI_EVENT);
            break;
        case E_TIME:
            mainIvu.assertCurrentObject(BI_EVENT | BI_COMMAND);
            readNumber();
            break;
        case SCR_SET_BLOCK_FLAG:
            mainIvu.assertCurrentObject(BI_SCREEN);
            break;
        case SCR_SET_DEFAULT_OBJ:
            mainIvu.assertCurrentObject(BI_SCREEN);
            readString();
            break;
        case SCR_SET_OFFS:
            mainIvu.assertCurrentObject(BI_SCREEN);
            readNumber();
            break;
        case TRIG_NEW_ELEMENT:
            mainIvu.assertCurrentObject(BI_OBJECT);
            readNumber();
            mainIvu.openNewObject(I_ELEMENT);
            oftrProcessElement();
            break;
        case TRIG_NUM_STATE:
            mainIvu.assertCurrentObject(BI_OBJECT);
            readNumber();
            break;
        case EOF_SIGN:
            inputOffsetReg = inputSizeReg;
            inputStream.close();
            mainLogger.info() << "Script tested successful!\n";
            break;
    }
}

void VangersVirtualMachine::oftrProcessElement() {
    switch (readInstruction()) {
        default:
            break;
        case EL_SCROLLER:
        case EL_AVI:
            readString();
            break;
    }
}