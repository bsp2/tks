   case 12: // ---- void, 1 args . (0)
                                 ((void(YAC_CALL*)(sUI))command->y_adr)(call_values[0].ui); break;
   case 13: // ---- int, 1 args . (0)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI))command->y_adr)(call_values[0].ui); _env->cret.type=1; break;
   case 14: // ---- float, 1 args . (0)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI))command->y_adr)(call_values[0].ui); _env->cret.type=2; break;
   case 15: // ---- Object, 1 args . (0)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI))command->y_adr)(call_values[0].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 16: // ---- void, 1 args * (1)
                                 ((void(YAC_CALL*)(void*))command->y_adr)(call_values[0].any); break;
   case 17: // ---- int, 1 args * (1)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*))command->y_adr)(call_values[0].any); _env->cret.type=1; break;
   case 18: // ---- float, 1 args * (1)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*))command->y_adr)(call_values[0].any); _env->cret.type=2; break;
   case 19: // ---- Object, 1 args * (1)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*))command->y_adr)(call_values[0].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 20: // ---- void, 2 args .. (0)
                                 ((void(YAC_CALL*)(sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui); break;
   case 21: // ---- int, 2 args .. (0)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui); _env->cret.type=1; break;
   case 22: // ---- float, 2 args .. (0)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui); _env->cret.type=2; break;
   case 23: // ---- Object, 2 args .. (0)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 24: // ---- void, 2 args *. (1)
                                 ((void(YAC_CALL*)(void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui); break;
   case 25: // ---- int, 2 args *. (1)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui); _env->cret.type=1; break;
   case 26: // ---- float, 2 args *. (1)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui); _env->cret.type=2; break;
   case 27: // ---- Object, 2 args *. (1)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 28: // ---- void, 2 args .* (2)
                                 ((void(YAC_CALL*)(sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any); break;
   case 29: // ---- int, 2 args .* (2)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any); _env->cret.type=1; break;
   case 30: // ---- float, 2 args .* (2)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any); _env->cret.type=2; break;
   case 31: // ---- Object, 2 args .* (2)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 32: // ---- void, 2 args ** (3)
                                 ((void(YAC_CALL*)(void*, void*))command->y_adr)(call_values[0].any, call_values[1].any); break;
   case 33: // ---- int, 2 args ** (3)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*))command->y_adr)(call_values[0].any, call_values[1].any); _env->cret.type=1; break;
   case 34: // ---- float, 2 args ** (3)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*))command->y_adr)(call_values[0].any, call_values[1].any); _env->cret.type=2; break;
   case 35: // ---- Object, 2 args ** (3)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*))command->y_adr)(call_values[0].any, call_values[1].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 36: // ---- void, 3 args ... (0)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui); break;
   case 37: // ---- int, 3 args ... (0)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui); _env->cret.type=1; break;
   case 38: // ---- float, 3 args ... (0)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui); _env->cret.type=2; break;
   case 39: // ---- Object, 3 args ... (0)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 40: // ---- void, 3 args *.. (1)
                                 ((void(YAC_CALL*)(void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui); break;
   case 41: // ---- int, 3 args *.. (1)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui); _env->cret.type=1; break;
   case 42: // ---- float, 3 args *.. (1)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui); _env->cret.type=2; break;
   case 43: // ---- Object, 3 args *.. (1)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 44: // ---- void, 3 args .*. (2)
                                 ((void(YAC_CALL*)(sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui); break;
   case 45: // ---- int, 3 args .*. (2)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui); _env->cret.type=1; break;
   case 46: // ---- float, 3 args .*. (2)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui); _env->cret.type=2; break;
   case 47: // ---- Object, 3 args .*. (2)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 48: // ---- void, 3 args **. (3)
                                 ((void(YAC_CALL*)(void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui); break;
   case 49: // ---- int, 3 args **. (3)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui); _env->cret.type=1; break;
   case 50: // ---- float, 3 args **. (3)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui); _env->cret.type=2; break;
   case 51: // ---- Object, 3 args **. (3)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 52: // ---- void, 3 args ..* (4)
                                 ((void(YAC_CALL*)(sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any); break;
   case 53: // ---- int, 3 args ..* (4)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any); _env->cret.type=1; break;
   case 54: // ---- float, 3 args ..* (4)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any); _env->cret.type=2; break;
   case 55: // ---- Object, 3 args ..* (4)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 56: // ---- void, 3 args *.* (5)
                                 ((void(YAC_CALL*)(void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any); break;
   case 57: // ---- int, 3 args *.* (5)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any); _env->cret.type=1; break;
   case 58: // ---- float, 3 args *.* (5)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any); _env->cret.type=2; break;
   case 59: // ---- Object, 3 args *.* (5)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 60: // ---- void, 3 args .** (6)
                                 ((void(YAC_CALL*)(sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any); break;
   case 61: // ---- int, 3 args .** (6)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any); _env->cret.type=1; break;
   case 62: // ---- float, 3 args .** (6)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any); _env->cret.type=2; break;
   case 63: // ---- Object, 3 args .** (6)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 64: // ---- void, 3 args *** (7)
                                 ((void(YAC_CALL*)(void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any); break;
   case 65: // ---- int, 3 args *** (7)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any); _env->cret.type=1; break;
   case 66: // ---- float, 3 args *** (7)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any); _env->cret.type=2; break;
   case 67: // ---- Object, 3 args *** (7)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 68: // ---- void, 4 args .... (0)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui); break;
   case 69: // ---- int, 4 args .... (0)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui); _env->cret.type=1; break;
   case 70: // ---- float, 4 args .... (0)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui); _env->cret.type=2; break;
   case 71: // ---- Object, 4 args .... (0)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 72: // ---- void, 4 args *... (1)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui); break;
   case 73: // ---- int, 4 args *... (1)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui); _env->cret.type=1; break;
   case 74: // ---- float, 4 args *... (1)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui); _env->cret.type=2; break;
   case 75: // ---- Object, 4 args *... (1)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 76: // ---- void, 4 args .*.. (2)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui); break;
   case 77: // ---- int, 4 args .*.. (2)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui); _env->cret.type=1; break;
   case 78: // ---- float, 4 args .*.. (2)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui); _env->cret.type=2; break;
   case 79: // ---- Object, 4 args .*.. (2)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 80: // ---- void, 4 args **.. (3)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui); break;
   case 81: // ---- int, 4 args **.. (3)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui); _env->cret.type=1; break;
   case 82: // ---- float, 4 args **.. (3)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui); _env->cret.type=2; break;
   case 83: // ---- Object, 4 args **.. (3)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 84: // ---- void, 4 args ..*. (4)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui); break;
   case 85: // ---- int, 4 args ..*. (4)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui); _env->cret.type=1; break;
   case 86: // ---- float, 4 args ..*. (4)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui); _env->cret.type=2; break;
   case 87: // ---- Object, 4 args ..*. (4)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 88: // ---- void, 4 args *.*. (5)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui); break;
   case 89: // ---- int, 4 args *.*. (5)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui); _env->cret.type=1; break;
   case 90: // ---- float, 4 args *.*. (5)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui); _env->cret.type=2; break;
   case 91: // ---- Object, 4 args *.*. (5)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 92: // ---- void, 4 args .**. (6)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui); break;
   case 93: // ---- int, 4 args .**. (6)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui); _env->cret.type=1; break;
   case 94: // ---- float, 4 args .**. (6)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui); _env->cret.type=2; break;
   case 95: // ---- Object, 4 args .**. (6)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 96: // ---- void, 4 args ***. (7)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui); break;
   case 97: // ---- int, 4 args ***. (7)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui); _env->cret.type=1; break;
   case 98: // ---- float, 4 args ***. (7)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui); _env->cret.type=2; break;
   case 99: // ---- Object, 4 args ***. (7)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 100: // ---- void, 4 args ...* (8)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any); break;
   case 101: // ---- int, 4 args ...* (8)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any); _env->cret.type=1; break;
   case 102: // ---- float, 4 args ...* (8)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any); _env->cret.type=2; break;
   case 103: // ---- Object, 4 args ...* (8)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 104: // ---- void, 4 args *..* (9)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any); break;
   case 105: // ---- int, 4 args *..* (9)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any); _env->cret.type=1; break;
   case 106: // ---- float, 4 args *..* (9)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any); _env->cret.type=2; break;
   case 107: // ---- Object, 4 args *..* (9)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 108: // ---- void, 4 args .*.* (10)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any); break;
   case 109: // ---- int, 4 args .*.* (10)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any); _env->cret.type=1; break;
   case 110: // ---- float, 4 args .*.* (10)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any); _env->cret.type=2; break;
   case 111: // ---- Object, 4 args .*.* (10)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 112: // ---- void, 4 args **.* (11)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any); break;
   case 113: // ---- int, 4 args **.* (11)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any); _env->cret.type=1; break;
   case 114: // ---- float, 4 args **.* (11)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any); _env->cret.type=2; break;
   case 115: // ---- Object, 4 args **.* (11)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 116: // ---- void, 4 args ..** (12)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any); break;
   case 117: // ---- int, 4 args ..** (12)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any); _env->cret.type=1; break;
   case 118: // ---- float, 4 args ..** (12)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any); _env->cret.type=2; break;
   case 119: // ---- Object, 4 args ..** (12)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 120: // ---- void, 4 args *.** (13)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any); break;
   case 121: // ---- int, 4 args *.** (13)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any); _env->cret.type=1; break;
   case 122: // ---- float, 4 args *.** (13)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any); _env->cret.type=2; break;
   case 123: // ---- Object, 4 args *.** (13)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 124: // ---- void, 4 args .*** (14)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any); break;
   case 125: // ---- int, 4 args .*** (14)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any); _env->cret.type=1; break;
   case 126: // ---- float, 4 args .*** (14)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any); _env->cret.type=2; break;
   case 127: // ---- Object, 4 args .*** (14)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 128: // ---- void, 4 args **** (15)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any); break;
   case 129: // ---- int, 4 args **** (15)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any); _env->cret.type=1; break;
   case 130: // ---- float, 4 args **** (15)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any); _env->cret.type=2; break;
   case 131: // ---- Object, 4 args **** (15)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 132: // ---- void, 5 args ..... (0)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui); break;
   case 133: // ---- int, 5 args ..... (0)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui); _env->cret.type=1; break;
   case 134: // ---- float, 5 args ..... (0)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui); _env->cret.type=2; break;
   case 135: // ---- Object, 5 args ..... (0)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 136: // ---- void, 5 args *.... (1)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui); break;
   case 137: // ---- int, 5 args *.... (1)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui); _env->cret.type=1; break;
   case 138: // ---- float, 5 args *.... (1)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui); _env->cret.type=2; break;
   case 139: // ---- Object, 5 args *.... (1)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 140: // ---- void, 5 args .*... (2)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui); break;
   case 141: // ---- int, 5 args .*... (2)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui); _env->cret.type=1; break;
   case 142: // ---- float, 5 args .*... (2)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui); _env->cret.type=2; break;
   case 143: // ---- Object, 5 args .*... (2)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 144: // ---- void, 5 args **... (3)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui); break;
   case 145: // ---- int, 5 args **... (3)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui); _env->cret.type=1; break;
   case 146: // ---- float, 5 args **... (3)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui); _env->cret.type=2; break;
   case 147: // ---- Object, 5 args **... (3)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 148: // ---- void, 5 args ..*.. (4)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui); break;
   case 149: // ---- int, 5 args ..*.. (4)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui); _env->cret.type=1; break;
   case 150: // ---- float, 5 args ..*.. (4)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui); _env->cret.type=2; break;
   case 151: // ---- Object, 5 args ..*.. (4)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 152: // ---- void, 5 args *.*.. (5)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui); break;
   case 153: // ---- int, 5 args *.*.. (5)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui); _env->cret.type=1; break;
   case 154: // ---- float, 5 args *.*.. (5)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui); _env->cret.type=2; break;
   case 155: // ---- Object, 5 args *.*.. (5)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 156: // ---- void, 5 args .**.. (6)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui); break;
   case 157: // ---- int, 5 args .**.. (6)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui); _env->cret.type=1; break;
   case 158: // ---- float, 5 args .**.. (6)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui); _env->cret.type=2; break;
   case 159: // ---- Object, 5 args .**.. (6)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 160: // ---- void, 5 args ***.. (7)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui); break;
   case 161: // ---- int, 5 args ***.. (7)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui); _env->cret.type=1; break;
   case 162: // ---- float, 5 args ***.. (7)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui); _env->cret.type=2; break;
   case 163: // ---- Object, 5 args ***.. (7)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 164: // ---- void, 5 args ...*. (8)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui); break;
   case 165: // ---- int, 5 args ...*. (8)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui); _env->cret.type=1; break;
   case 166: // ---- float, 5 args ...*. (8)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui); _env->cret.type=2; break;
   case 167: // ---- Object, 5 args ...*. (8)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 168: // ---- void, 5 args *..*. (9)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui); break;
   case 169: // ---- int, 5 args *..*. (9)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui); _env->cret.type=1; break;
   case 170: // ---- float, 5 args *..*. (9)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui); _env->cret.type=2; break;
   case 171: // ---- Object, 5 args *..*. (9)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 172: // ---- void, 5 args .*.*. (10)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui); break;
   case 173: // ---- int, 5 args .*.*. (10)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui); _env->cret.type=1; break;
   case 174: // ---- float, 5 args .*.*. (10)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui); _env->cret.type=2; break;
   case 175: // ---- Object, 5 args .*.*. (10)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 176: // ---- void, 5 args **.*. (11)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui); break;
   case 177: // ---- int, 5 args **.*. (11)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui); _env->cret.type=1; break;
   case 178: // ---- float, 5 args **.*. (11)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui); _env->cret.type=2; break;
   case 179: // ---- Object, 5 args **.*. (11)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 180: // ---- void, 5 args ..**. (12)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui); break;
   case 181: // ---- int, 5 args ..**. (12)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui); _env->cret.type=1; break;
   case 182: // ---- float, 5 args ..**. (12)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui); _env->cret.type=2; break;
   case 183: // ---- Object, 5 args ..**. (12)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 184: // ---- void, 5 args *.**. (13)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui); break;
   case 185: // ---- int, 5 args *.**. (13)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui); _env->cret.type=1; break;
   case 186: // ---- float, 5 args *.**. (13)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui); _env->cret.type=2; break;
   case 187: // ---- Object, 5 args *.**. (13)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 188: // ---- void, 5 args .***. (14)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui); break;
   case 189: // ---- int, 5 args .***. (14)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui); _env->cret.type=1; break;
   case 190: // ---- float, 5 args .***. (14)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui); _env->cret.type=2; break;
   case 191: // ---- Object, 5 args .***. (14)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 192: // ---- void, 5 args ****. (15)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui); break;
   case 193: // ---- int, 5 args ****. (15)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui); _env->cret.type=1; break;
   case 194: // ---- float, 5 args ****. (15)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui); _env->cret.type=2; break;
   case 195: // ---- Object, 5 args ****. (15)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 196: // ---- void, 5 args ....* (16)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any); break;
   case 197: // ---- int, 5 args ....* (16)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any); _env->cret.type=1; break;
   case 198: // ---- float, 5 args ....* (16)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any); _env->cret.type=2; break;
   case 199: // ---- Object, 5 args ....* (16)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 200: // ---- void, 5 args *...* (17)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any); break;
   case 201: // ---- int, 5 args *...* (17)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any); _env->cret.type=1; break;
   case 202: // ---- float, 5 args *...* (17)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any); _env->cret.type=2; break;
   case 203: // ---- Object, 5 args *...* (17)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 204: // ---- void, 5 args .*..* (18)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any); break;
   case 205: // ---- int, 5 args .*..* (18)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any); _env->cret.type=1; break;
   case 206: // ---- float, 5 args .*..* (18)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any); _env->cret.type=2; break;
   case 207: // ---- Object, 5 args .*..* (18)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 208: // ---- void, 5 args **..* (19)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any); break;
   case 209: // ---- int, 5 args **..* (19)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any); _env->cret.type=1; break;
   case 210: // ---- float, 5 args **..* (19)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any); _env->cret.type=2; break;
   case 211: // ---- Object, 5 args **..* (19)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 212: // ---- void, 5 args ..*.* (20)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any); break;
   case 213: // ---- int, 5 args ..*.* (20)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any); _env->cret.type=1; break;
   case 214: // ---- float, 5 args ..*.* (20)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any); _env->cret.type=2; break;
   case 215: // ---- Object, 5 args ..*.* (20)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 216: // ---- void, 5 args *.*.* (21)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any); break;
   case 217: // ---- int, 5 args *.*.* (21)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any); _env->cret.type=1; break;
   case 218: // ---- float, 5 args *.*.* (21)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any); _env->cret.type=2; break;
   case 219: // ---- Object, 5 args *.*.* (21)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 220: // ---- void, 5 args .**.* (22)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any); break;
   case 221: // ---- int, 5 args .**.* (22)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any); _env->cret.type=1; break;
   case 222: // ---- float, 5 args .**.* (22)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any); _env->cret.type=2; break;
   case 223: // ---- Object, 5 args .**.* (22)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 224: // ---- void, 5 args ***.* (23)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any); break;
   case 225: // ---- int, 5 args ***.* (23)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any); _env->cret.type=1; break;
   case 226: // ---- float, 5 args ***.* (23)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any); _env->cret.type=2; break;
   case 227: // ---- Object, 5 args ***.* (23)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 228: // ---- void, 5 args ...** (24)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any); break;
   case 229: // ---- int, 5 args ...** (24)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any); _env->cret.type=1; break;
   case 230: // ---- float, 5 args ...** (24)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any); _env->cret.type=2; break;
   case 231: // ---- Object, 5 args ...** (24)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 232: // ---- void, 5 args *..** (25)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any); break;
   case 233: // ---- int, 5 args *..** (25)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any); _env->cret.type=1; break;
   case 234: // ---- float, 5 args *..** (25)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any); _env->cret.type=2; break;
   case 235: // ---- Object, 5 args *..** (25)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 236: // ---- void, 5 args .*.** (26)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any); break;
   case 237: // ---- int, 5 args .*.** (26)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any); _env->cret.type=1; break;
   case 238: // ---- float, 5 args .*.** (26)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any); _env->cret.type=2; break;
   case 239: // ---- Object, 5 args .*.** (26)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 240: // ---- void, 5 args **.** (27)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any); break;
   case 241: // ---- int, 5 args **.** (27)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any); _env->cret.type=1; break;
   case 242: // ---- float, 5 args **.** (27)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any); _env->cret.type=2; break;
   case 243: // ---- Object, 5 args **.** (27)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 244: // ---- void, 5 args ..*** (28)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any); break;
   case 245: // ---- int, 5 args ..*** (28)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any); _env->cret.type=1; break;
   case 246: // ---- float, 5 args ..*** (28)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any); _env->cret.type=2; break;
   case 247: // ---- Object, 5 args ..*** (28)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 248: // ---- void, 5 args *.*** (29)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any); break;
   case 249: // ---- int, 5 args *.*** (29)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any); _env->cret.type=1; break;
   case 250: // ---- float, 5 args *.*** (29)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any); _env->cret.type=2; break;
   case 251: // ---- Object, 5 args *.*** (29)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 252: // ---- void, 5 args .**** (30)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any); break;
   case 253: // ---- int, 5 args .**** (30)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any); _env->cret.type=1; break;
   case 254: // ---- float, 5 args .**** (30)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any); _env->cret.type=2; break;
   case 255: // ---- Object, 5 args .**** (30)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 256: // ---- void, 5 args ***** (31)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any); break;
   case 257: // ---- int, 5 args ***** (31)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any); _env->cret.type=1; break;
   case 258: // ---- float, 5 args ***** (31)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any); _env->cret.type=2; break;
   case 259: // ---- Object, 5 args ***** (31)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 260: // ---- void, 6 args ...... (0)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); break;
   case 261: // ---- int, 6 args ...... (0)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 262: // ---- float, 6 args ...... (0)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 263: // ---- Object, 6 args ...... (0)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 264: // ---- void, 6 args *..... (1)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); break;
   case 265: // ---- int, 6 args *..... (1)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 266: // ---- float, 6 args *..... (1)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 267: // ---- Object, 6 args *..... (1)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 268: // ---- void, 6 args .*.... (2)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); break;
   case 269: // ---- int, 6 args .*.... (2)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 270: // ---- float, 6 args .*.... (2)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 271: // ---- Object, 6 args .*.... (2)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 272: // ---- void, 6 args **.... (3)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); break;
   case 273: // ---- int, 6 args **.... (3)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 274: // ---- float, 6 args **.... (3)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 275: // ---- Object, 6 args **.... (3)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 276: // ---- void, 6 args ..*... (4)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); break;
   case 277: // ---- int, 6 args ..*... (4)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 278: // ---- float, 6 args ..*... (4)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 279: // ---- Object, 6 args ..*... (4)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 280: // ---- void, 6 args *.*... (5)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); break;
   case 281: // ---- int, 6 args *.*... (5)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 282: // ---- float, 6 args *.*... (5)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 283: // ---- Object, 6 args *.*... (5)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 284: // ---- void, 6 args .**... (6)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); break;
   case 285: // ---- int, 6 args .**... (6)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 286: // ---- float, 6 args .**... (6)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 287: // ---- Object, 6 args .**... (6)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 288: // ---- void, 6 args ***... (7)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); break;
   case 289: // ---- int, 6 args ***... (7)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 290: // ---- float, 6 args ***... (7)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 291: // ---- Object, 6 args ***... (7)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 292: // ---- void, 6 args ...*.. (8)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); break;
   case 293: // ---- int, 6 args ...*.. (8)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 294: // ---- float, 6 args ...*.. (8)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 295: // ---- Object, 6 args ...*.. (8)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 296: // ---- void, 6 args *..*.. (9)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); break;
   case 297: // ---- int, 6 args *..*.. (9)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 298: // ---- float, 6 args *..*.. (9)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 299: // ---- Object, 6 args *..*.. (9)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 300: // ---- void, 6 args .*.*.. (10)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); break;
   case 301: // ---- int, 6 args .*.*.. (10)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 302: // ---- float, 6 args .*.*.. (10)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 303: // ---- Object, 6 args .*.*.. (10)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 304: // ---- void, 6 args **.*.. (11)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); break;
   case 305: // ---- int, 6 args **.*.. (11)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 306: // ---- float, 6 args **.*.. (11)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 307: // ---- Object, 6 args **.*.. (11)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 308: // ---- void, 6 args ..**.. (12)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); break;
   case 309: // ---- int, 6 args ..**.. (12)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 310: // ---- float, 6 args ..**.. (12)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 311: // ---- Object, 6 args ..**.. (12)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 312: // ---- void, 6 args *.**.. (13)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); break;
   case 313: // ---- int, 6 args *.**.. (13)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 314: // ---- float, 6 args *.**.. (13)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 315: // ---- Object, 6 args *.**.. (13)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 316: // ---- void, 6 args .***.. (14)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); break;
   case 317: // ---- int, 6 args .***.. (14)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 318: // ---- float, 6 args .***.. (14)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 319: // ---- Object, 6 args .***.. (14)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 320: // ---- void, 6 args ****.. (15)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); break;
   case 321: // ---- int, 6 args ****.. (15)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=1; break;
   case 322: // ---- float, 6 args ****.. (15)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=2; break;
   case 323: // ---- Object, 6 args ****.. (15)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 324: // ---- void, 6 args ....*. (16)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); break;
   case 325: // ---- int, 6 args ....*. (16)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 326: // ---- float, 6 args ....*. (16)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 327: // ---- Object, 6 args ....*. (16)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 328: // ---- void, 6 args *...*. (17)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); break;
   case 329: // ---- int, 6 args *...*. (17)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 330: // ---- float, 6 args *...*. (17)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 331: // ---- Object, 6 args *...*. (17)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 332: // ---- void, 6 args .*..*. (18)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); break;
   case 333: // ---- int, 6 args .*..*. (18)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 334: // ---- float, 6 args .*..*. (18)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 335: // ---- Object, 6 args .*..*. (18)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 336: // ---- void, 6 args **..*. (19)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); break;
   case 337: // ---- int, 6 args **..*. (19)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 338: // ---- float, 6 args **..*. (19)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 339: // ---- Object, 6 args **..*. (19)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 340: // ---- void, 6 args ..*.*. (20)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); break;
   case 341: // ---- int, 6 args ..*.*. (20)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 342: // ---- float, 6 args ..*.*. (20)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 343: // ---- Object, 6 args ..*.*. (20)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 344: // ---- void, 6 args *.*.*. (21)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); break;
   case 345: // ---- int, 6 args *.*.*. (21)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 346: // ---- float, 6 args *.*.*. (21)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 347: // ---- Object, 6 args *.*.*. (21)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 348: // ---- void, 6 args .**.*. (22)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); break;
   case 349: // ---- int, 6 args .**.*. (22)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 350: // ---- float, 6 args .**.*. (22)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 351: // ---- Object, 6 args .**.*. (22)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 352: // ---- void, 6 args ***.*. (23)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); break;
   case 353: // ---- int, 6 args ***.*. (23)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 354: // ---- float, 6 args ***.*. (23)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 355: // ---- Object, 6 args ***.*. (23)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 356: // ---- void, 6 args ...**. (24)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); break;
   case 357: // ---- int, 6 args ...**. (24)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 358: // ---- float, 6 args ...**. (24)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 359: // ---- Object, 6 args ...**. (24)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 360: // ---- void, 6 args *..**. (25)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); break;
   case 361: // ---- int, 6 args *..**. (25)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 362: // ---- float, 6 args *..**. (25)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 363: // ---- Object, 6 args *..**. (25)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 364: // ---- void, 6 args .*.**. (26)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); break;
   case 365: // ---- int, 6 args .*.**. (26)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 366: // ---- float, 6 args .*.**. (26)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 367: // ---- Object, 6 args .*.**. (26)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 368: // ---- void, 6 args **.**. (27)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); break;
   case 369: // ---- int, 6 args **.**. (27)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 370: // ---- float, 6 args **.**. (27)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 371: // ---- Object, 6 args **.**. (27)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 372: // ---- void, 6 args ..***. (28)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); break;
   case 373: // ---- int, 6 args ..***. (28)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 374: // ---- float, 6 args ..***. (28)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 375: // ---- Object, 6 args ..***. (28)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 376: // ---- void, 6 args *.***. (29)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); break;
   case 377: // ---- int, 6 args *.***. (29)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 378: // ---- float, 6 args *.***. (29)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 379: // ---- Object, 6 args *.***. (29)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 380: // ---- void, 6 args .****. (30)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); break;
   case 381: // ---- int, 6 args .****. (30)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 382: // ---- float, 6 args .****. (30)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 383: // ---- Object, 6 args .****. (30)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 384: // ---- void, 6 args *****. (31)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); break;
   case 385: // ---- int, 6 args *****. (31)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=1; break;
   case 386: // ---- float, 6 args *****. (31)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=2; break;
   case 387: // ---- Object, 6 args *****. (31)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 388: // ---- void, 6 args .....* (32)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); break;
   case 389: // ---- int, 6 args .....* (32)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 390: // ---- float, 6 args .....* (32)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 391: // ---- Object, 6 args .....* (32)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 392: // ---- void, 6 args *....* (33)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); break;
   case 393: // ---- int, 6 args *....* (33)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 394: // ---- float, 6 args *....* (33)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 395: // ---- Object, 6 args *....* (33)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 396: // ---- void, 6 args .*...* (34)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); break;
   case 397: // ---- int, 6 args .*...* (34)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 398: // ---- float, 6 args .*...* (34)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 399: // ---- Object, 6 args .*...* (34)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 400: // ---- void, 6 args **...* (35)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); break;
   case 401: // ---- int, 6 args **...* (35)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 402: // ---- float, 6 args **...* (35)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 403: // ---- Object, 6 args **...* (35)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 404: // ---- void, 6 args ..*..* (36)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); break;
   case 405: // ---- int, 6 args ..*..* (36)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 406: // ---- float, 6 args ..*..* (36)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 407: // ---- Object, 6 args ..*..* (36)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 408: // ---- void, 6 args *.*..* (37)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); break;
   case 409: // ---- int, 6 args *.*..* (37)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 410: // ---- float, 6 args *.*..* (37)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 411: // ---- Object, 6 args *.*..* (37)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 412: // ---- void, 6 args .**..* (38)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); break;
   case 413: // ---- int, 6 args .**..* (38)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 414: // ---- float, 6 args .**..* (38)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 415: // ---- Object, 6 args .**..* (38)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 416: // ---- void, 6 args ***..* (39)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); break;
   case 417: // ---- int, 6 args ***..* (39)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 418: // ---- float, 6 args ***..* (39)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 419: // ---- Object, 6 args ***..* (39)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 420: // ---- void, 6 args ...*.* (40)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); break;
   case 421: // ---- int, 6 args ...*.* (40)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 422: // ---- float, 6 args ...*.* (40)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 423: // ---- Object, 6 args ...*.* (40)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 424: // ---- void, 6 args *..*.* (41)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); break;
   case 425: // ---- int, 6 args *..*.* (41)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 426: // ---- float, 6 args *..*.* (41)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 427: // ---- Object, 6 args *..*.* (41)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 428: // ---- void, 6 args .*.*.* (42)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); break;
   case 429: // ---- int, 6 args .*.*.* (42)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 430: // ---- float, 6 args .*.*.* (42)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 431: // ---- Object, 6 args .*.*.* (42)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 432: // ---- void, 6 args **.*.* (43)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); break;
   case 433: // ---- int, 6 args **.*.* (43)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 434: // ---- float, 6 args **.*.* (43)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 435: // ---- Object, 6 args **.*.* (43)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 436: // ---- void, 6 args ..**.* (44)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); break;
   case 437: // ---- int, 6 args ..**.* (44)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 438: // ---- float, 6 args ..**.* (44)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 439: // ---- Object, 6 args ..**.* (44)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 440: // ---- void, 6 args *.**.* (45)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); break;
   case 441: // ---- int, 6 args *.**.* (45)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 442: // ---- float, 6 args *.**.* (45)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 443: // ---- Object, 6 args *.**.* (45)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 444: // ---- void, 6 args .***.* (46)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); break;
   case 445: // ---- int, 6 args .***.* (46)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 446: // ---- float, 6 args .***.* (46)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 447: // ---- Object, 6 args .***.* (46)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 448: // ---- void, 6 args ****.* (47)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); break;
   case 449: // ---- int, 6 args ****.* (47)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=1; break;
   case 450: // ---- float, 6 args ****.* (47)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=2; break;
   case 451: // ---- Object, 6 args ****.* (47)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 452: // ---- void, 6 args ....** (48)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); break;
   case 453: // ---- int, 6 args ....** (48)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 454: // ---- float, 6 args ....** (48)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 455: // ---- Object, 6 args ....** (48)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 456: // ---- void, 6 args *...** (49)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); break;
   case 457: // ---- int, 6 args *...** (49)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 458: // ---- float, 6 args *...** (49)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 459: // ---- Object, 6 args *...** (49)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 460: // ---- void, 6 args .*..** (50)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); break;
   case 461: // ---- int, 6 args .*..** (50)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 462: // ---- float, 6 args .*..** (50)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 463: // ---- Object, 6 args .*..** (50)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 464: // ---- void, 6 args **..** (51)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); break;
   case 465: // ---- int, 6 args **..** (51)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 466: // ---- float, 6 args **..** (51)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 467: // ---- Object, 6 args **..** (51)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 468: // ---- void, 6 args ..*.** (52)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); break;
   case 469: // ---- int, 6 args ..*.** (52)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 470: // ---- float, 6 args ..*.** (52)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 471: // ---- Object, 6 args ..*.** (52)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 472: // ---- void, 6 args *.*.** (53)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); break;
   case 473: // ---- int, 6 args *.*.** (53)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 474: // ---- float, 6 args *.*.** (53)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 475: // ---- Object, 6 args *.*.** (53)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 476: // ---- void, 6 args .**.** (54)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); break;
   case 477: // ---- int, 6 args .**.** (54)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 478: // ---- float, 6 args .**.** (54)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 479: // ---- Object, 6 args .**.** (54)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 480: // ---- void, 6 args ***.** (55)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); break;
   case 481: // ---- int, 6 args ***.** (55)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 482: // ---- float, 6 args ***.** (55)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 483: // ---- Object, 6 args ***.** (55)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 484: // ---- void, 6 args ...*** (56)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); break;
   case 485: // ---- int, 6 args ...*** (56)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 486: // ---- float, 6 args ...*** (56)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 487: // ---- Object, 6 args ...*** (56)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 488: // ---- void, 6 args *..*** (57)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); break;
   case 489: // ---- int, 6 args *..*** (57)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 490: // ---- float, 6 args *..*** (57)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 491: // ---- Object, 6 args *..*** (57)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 492: // ---- void, 6 args .*.*** (58)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); break;
   case 493: // ---- int, 6 args .*.*** (58)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 494: // ---- float, 6 args .*.*** (58)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 495: // ---- Object, 6 args .*.*** (58)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 496: // ---- void, 6 args **.*** (59)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); break;
   case 497: // ---- int, 6 args **.*** (59)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 498: // ---- float, 6 args **.*** (59)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 499: // ---- Object, 6 args **.*** (59)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 500: // ---- void, 6 args ..**** (60)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); break;
   case 501: // ---- int, 6 args ..**** (60)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 502: // ---- float, 6 args ..**** (60)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 503: // ---- Object, 6 args ..**** (60)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 504: // ---- void, 6 args *.**** (61)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); break;
   case 505: // ---- int, 6 args *.**** (61)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 506: // ---- float, 6 args *.**** (61)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 507: // ---- Object, 6 args *.**** (61)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 508: // ---- void, 6 args .***** (62)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); break;
   case 509: // ---- int, 6 args .***** (62)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 510: // ---- float, 6 args .***** (62)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 511: // ---- Object, 6 args .***** (62)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 512: // ---- void, 6 args ****** (63)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); break;
   case 513: // ---- int, 6 args ****** (63)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=1; break;
   case 514: // ---- float, 6 args ****** (63)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=2; break;
   case 515: // ---- Object, 6 args ****** (63)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 516: // ---- void, 7 args ....... (0)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 517: // ---- int, 7 args ....... (0)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 518: // ---- float, 7 args ....... (0)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 519: // ---- Object, 7 args ....... (0)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 520: // ---- void, 7 args *...... (1)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 521: // ---- int, 7 args *...... (1)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 522: // ---- float, 7 args *...... (1)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 523: // ---- Object, 7 args *...... (1)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 524: // ---- void, 7 args .*..... (2)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 525: // ---- int, 7 args .*..... (2)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 526: // ---- float, 7 args .*..... (2)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 527: // ---- Object, 7 args .*..... (2)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 528: // ---- void, 7 args **..... (3)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 529: // ---- int, 7 args **..... (3)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 530: // ---- float, 7 args **..... (3)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 531: // ---- Object, 7 args **..... (3)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 532: // ---- void, 7 args ..*.... (4)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 533: // ---- int, 7 args ..*.... (4)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 534: // ---- float, 7 args ..*.... (4)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 535: // ---- Object, 7 args ..*.... (4)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 536: // ---- void, 7 args *.*.... (5)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 537: // ---- int, 7 args *.*.... (5)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 538: // ---- float, 7 args *.*.... (5)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 539: // ---- Object, 7 args *.*.... (5)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 540: // ---- void, 7 args .**.... (6)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 541: // ---- int, 7 args .**.... (6)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 542: // ---- float, 7 args .**.... (6)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 543: // ---- Object, 7 args .**.... (6)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 544: // ---- void, 7 args ***.... (7)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 545: // ---- int, 7 args ***.... (7)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 546: // ---- float, 7 args ***.... (7)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 547: // ---- Object, 7 args ***.... (7)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 548: // ---- void, 7 args ...*... (8)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 549: // ---- int, 7 args ...*... (8)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 550: // ---- float, 7 args ...*... (8)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 551: // ---- Object, 7 args ...*... (8)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 552: // ---- void, 7 args *..*... (9)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 553: // ---- int, 7 args *..*... (9)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 554: // ---- float, 7 args *..*... (9)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 555: // ---- Object, 7 args *..*... (9)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 556: // ---- void, 7 args .*.*... (10)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 557: // ---- int, 7 args .*.*... (10)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 558: // ---- float, 7 args .*.*... (10)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 559: // ---- Object, 7 args .*.*... (10)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 560: // ---- void, 7 args **.*... (11)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 561: // ---- int, 7 args **.*... (11)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 562: // ---- float, 7 args **.*... (11)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 563: // ---- Object, 7 args **.*... (11)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 564: // ---- void, 7 args ..**... (12)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 565: // ---- int, 7 args ..**... (12)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 566: // ---- float, 7 args ..**... (12)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 567: // ---- Object, 7 args ..**... (12)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 568: // ---- void, 7 args *.**... (13)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 569: // ---- int, 7 args *.**... (13)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 570: // ---- float, 7 args *.**... (13)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 571: // ---- Object, 7 args *.**... (13)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 572: // ---- void, 7 args .***... (14)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 573: // ---- int, 7 args .***... (14)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 574: // ---- float, 7 args .***... (14)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 575: // ---- Object, 7 args .***... (14)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 576: // ---- void, 7 args ****... (15)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); break;
   case 577: // ---- int, 7 args ****... (15)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 578: // ---- float, 7 args ****... (15)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 579: // ---- Object, 7 args ****... (15)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 580: // ---- void, 7 args ....*.. (16)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 581: // ---- int, 7 args ....*.. (16)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 582: // ---- float, 7 args ....*.. (16)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 583: // ---- Object, 7 args ....*.. (16)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 584: // ---- void, 7 args *...*.. (17)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 585: // ---- int, 7 args *...*.. (17)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 586: // ---- float, 7 args *...*.. (17)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 587: // ---- Object, 7 args *...*.. (17)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 588: // ---- void, 7 args .*..*.. (18)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 589: // ---- int, 7 args .*..*.. (18)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 590: // ---- float, 7 args .*..*.. (18)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 591: // ---- Object, 7 args .*..*.. (18)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 592: // ---- void, 7 args **..*.. (19)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 593: // ---- int, 7 args **..*.. (19)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 594: // ---- float, 7 args **..*.. (19)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 595: // ---- Object, 7 args **..*.. (19)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 596: // ---- void, 7 args ..*.*.. (20)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 597: // ---- int, 7 args ..*.*.. (20)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 598: // ---- float, 7 args ..*.*.. (20)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 599: // ---- Object, 7 args ..*.*.. (20)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 600: // ---- void, 7 args *.*.*.. (21)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 601: // ---- int, 7 args *.*.*.. (21)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 602: // ---- float, 7 args *.*.*.. (21)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 603: // ---- Object, 7 args *.*.*.. (21)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 604: // ---- void, 7 args .**.*.. (22)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 605: // ---- int, 7 args .**.*.. (22)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 606: // ---- float, 7 args .**.*.. (22)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 607: // ---- Object, 7 args .**.*.. (22)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 608: // ---- void, 7 args ***.*.. (23)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 609: // ---- int, 7 args ***.*.. (23)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 610: // ---- float, 7 args ***.*.. (23)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 611: // ---- Object, 7 args ***.*.. (23)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 612: // ---- void, 7 args ...**.. (24)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 613: // ---- int, 7 args ...**.. (24)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 614: // ---- float, 7 args ...**.. (24)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 615: // ---- Object, 7 args ...**.. (24)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 616: // ---- void, 7 args *..**.. (25)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 617: // ---- int, 7 args *..**.. (25)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 618: // ---- float, 7 args *..**.. (25)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 619: // ---- Object, 7 args *..**.. (25)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 620: // ---- void, 7 args .*.**.. (26)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 621: // ---- int, 7 args .*.**.. (26)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 622: // ---- float, 7 args .*.**.. (26)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 623: // ---- Object, 7 args .*.**.. (26)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 624: // ---- void, 7 args **.**.. (27)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 625: // ---- int, 7 args **.**.. (27)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 626: // ---- float, 7 args **.**.. (27)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 627: // ---- Object, 7 args **.**.. (27)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 628: // ---- void, 7 args ..***.. (28)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 629: // ---- int, 7 args ..***.. (28)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 630: // ---- float, 7 args ..***.. (28)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 631: // ---- Object, 7 args ..***.. (28)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 632: // ---- void, 7 args *.***.. (29)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 633: // ---- int, 7 args *.***.. (29)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 634: // ---- float, 7 args *.***.. (29)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 635: // ---- Object, 7 args *.***.. (29)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 636: // ---- void, 7 args .****.. (30)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 637: // ---- int, 7 args .****.. (30)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 638: // ---- float, 7 args .****.. (30)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 639: // ---- Object, 7 args .****.. (30)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 640: // ---- void, 7 args *****.. (31)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); break;
   case 641: // ---- int, 7 args *****.. (31)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=1; break;
   case 642: // ---- float, 7 args *****.. (31)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=2; break;
   case 643: // ---- Object, 7 args *****.. (31)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 644: // ---- void, 7 args .....*. (32)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 645: // ---- int, 7 args .....*. (32)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 646: // ---- float, 7 args .....*. (32)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 647: // ---- Object, 7 args .....*. (32)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 648: // ---- void, 7 args *....*. (33)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 649: // ---- int, 7 args *....*. (33)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 650: // ---- float, 7 args *....*. (33)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 651: // ---- Object, 7 args *....*. (33)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 652: // ---- void, 7 args .*...*. (34)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 653: // ---- int, 7 args .*...*. (34)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 654: // ---- float, 7 args .*...*. (34)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 655: // ---- Object, 7 args .*...*. (34)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 656: // ---- void, 7 args **...*. (35)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 657: // ---- int, 7 args **...*. (35)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 658: // ---- float, 7 args **...*. (35)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 659: // ---- Object, 7 args **...*. (35)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 660: // ---- void, 7 args ..*..*. (36)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 661: // ---- int, 7 args ..*..*. (36)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 662: // ---- float, 7 args ..*..*. (36)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 663: // ---- Object, 7 args ..*..*. (36)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 664: // ---- void, 7 args *.*..*. (37)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 665: // ---- int, 7 args *.*..*. (37)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 666: // ---- float, 7 args *.*..*. (37)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 667: // ---- Object, 7 args *.*..*. (37)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 668: // ---- void, 7 args .**..*. (38)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 669: // ---- int, 7 args .**..*. (38)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 670: // ---- float, 7 args .**..*. (38)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 671: // ---- Object, 7 args .**..*. (38)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 672: // ---- void, 7 args ***..*. (39)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 673: // ---- int, 7 args ***..*. (39)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 674: // ---- float, 7 args ***..*. (39)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 675: // ---- Object, 7 args ***..*. (39)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 676: // ---- void, 7 args ...*.*. (40)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 677: // ---- int, 7 args ...*.*. (40)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 678: // ---- float, 7 args ...*.*. (40)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 679: // ---- Object, 7 args ...*.*. (40)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 680: // ---- void, 7 args *..*.*. (41)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 681: // ---- int, 7 args *..*.*. (41)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 682: // ---- float, 7 args *..*.*. (41)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 683: // ---- Object, 7 args *..*.*. (41)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 684: // ---- void, 7 args .*.*.*. (42)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 685: // ---- int, 7 args .*.*.*. (42)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 686: // ---- float, 7 args .*.*.*. (42)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 687: // ---- Object, 7 args .*.*.*. (42)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 688: // ---- void, 7 args **.*.*. (43)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 689: // ---- int, 7 args **.*.*. (43)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 690: // ---- float, 7 args **.*.*. (43)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 691: // ---- Object, 7 args **.*.*. (43)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 692: // ---- void, 7 args ..**.*. (44)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 693: // ---- int, 7 args ..**.*. (44)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 694: // ---- float, 7 args ..**.*. (44)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 695: // ---- Object, 7 args ..**.*. (44)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 696: // ---- void, 7 args *.**.*. (45)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 697: // ---- int, 7 args *.**.*. (45)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 698: // ---- float, 7 args *.**.*. (45)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 699: // ---- Object, 7 args *.**.*. (45)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 700: // ---- void, 7 args .***.*. (46)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 701: // ---- int, 7 args .***.*. (46)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 702: // ---- float, 7 args .***.*. (46)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 703: // ---- Object, 7 args .***.*. (46)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 704: // ---- void, 7 args ****.*. (47)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); break;
   case 705: // ---- int, 7 args ****.*. (47)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 706: // ---- float, 7 args ****.*. (47)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 707: // ---- Object, 7 args ****.*. (47)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 708: // ---- void, 7 args ....**. (48)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 709: // ---- int, 7 args ....**. (48)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 710: // ---- float, 7 args ....**. (48)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 711: // ---- Object, 7 args ....**. (48)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 712: // ---- void, 7 args *...**. (49)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 713: // ---- int, 7 args *...**. (49)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 714: // ---- float, 7 args *...**. (49)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 715: // ---- Object, 7 args *...**. (49)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 716: // ---- void, 7 args .*..**. (50)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 717: // ---- int, 7 args .*..**. (50)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 718: // ---- float, 7 args .*..**. (50)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 719: // ---- Object, 7 args .*..**. (50)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 720: // ---- void, 7 args **..**. (51)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 721: // ---- int, 7 args **..**. (51)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 722: // ---- float, 7 args **..**. (51)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 723: // ---- Object, 7 args **..**. (51)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 724: // ---- void, 7 args ..*.**. (52)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 725: // ---- int, 7 args ..*.**. (52)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 726: // ---- float, 7 args ..*.**. (52)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 727: // ---- Object, 7 args ..*.**. (52)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 728: // ---- void, 7 args *.*.**. (53)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 729: // ---- int, 7 args *.*.**. (53)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 730: // ---- float, 7 args *.*.**. (53)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 731: // ---- Object, 7 args *.*.**. (53)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 732: // ---- void, 7 args .**.**. (54)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 733: // ---- int, 7 args .**.**. (54)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 734: // ---- float, 7 args .**.**. (54)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 735: // ---- Object, 7 args .**.**. (54)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 736: // ---- void, 7 args ***.**. (55)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 737: // ---- int, 7 args ***.**. (55)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 738: // ---- float, 7 args ***.**. (55)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 739: // ---- Object, 7 args ***.**. (55)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 740: // ---- void, 7 args ...***. (56)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 741: // ---- int, 7 args ...***. (56)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 742: // ---- float, 7 args ...***. (56)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 743: // ---- Object, 7 args ...***. (56)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 744: // ---- void, 7 args *..***. (57)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 745: // ---- int, 7 args *..***. (57)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 746: // ---- float, 7 args *..***. (57)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 747: // ---- Object, 7 args *..***. (57)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 748: // ---- void, 7 args .*.***. (58)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 749: // ---- int, 7 args .*.***. (58)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 750: // ---- float, 7 args .*.***. (58)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 751: // ---- Object, 7 args .*.***. (58)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 752: // ---- void, 7 args **.***. (59)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 753: // ---- int, 7 args **.***. (59)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 754: // ---- float, 7 args **.***. (59)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 755: // ---- Object, 7 args **.***. (59)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 756: // ---- void, 7 args ..****. (60)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 757: // ---- int, 7 args ..****. (60)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 758: // ---- float, 7 args ..****. (60)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 759: // ---- Object, 7 args ..****. (60)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 760: // ---- void, 7 args *.****. (61)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 761: // ---- int, 7 args *.****. (61)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 762: // ---- float, 7 args *.****. (61)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 763: // ---- Object, 7 args *.****. (61)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 764: // ---- void, 7 args .*****. (62)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 765: // ---- int, 7 args .*****. (62)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 766: // ---- float, 7 args .*****. (62)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 767: // ---- Object, 7 args .*****. (62)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 768: // ---- void, 7 args ******. (63)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); break;
   case 769: // ---- int, 7 args ******. (63)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=1; break;
   case 770: // ---- float, 7 args ******. (63)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=2; break;
   case 771: // ---- Object, 7 args ******. (63)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 772: // ---- void, 7 args ......* (64)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 773: // ---- int, 7 args ......* (64)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 774: // ---- float, 7 args ......* (64)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 775: // ---- Object, 7 args ......* (64)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 776: // ---- void, 7 args *.....* (65)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 777: // ---- int, 7 args *.....* (65)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 778: // ---- float, 7 args *.....* (65)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 779: // ---- Object, 7 args *.....* (65)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 780: // ---- void, 7 args .*....* (66)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 781: // ---- int, 7 args .*....* (66)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 782: // ---- float, 7 args .*....* (66)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 783: // ---- Object, 7 args .*....* (66)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 784: // ---- void, 7 args **....* (67)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 785: // ---- int, 7 args **....* (67)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 786: // ---- float, 7 args **....* (67)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 787: // ---- Object, 7 args **....* (67)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 788: // ---- void, 7 args ..*...* (68)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 789: // ---- int, 7 args ..*...* (68)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 790: // ---- float, 7 args ..*...* (68)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 791: // ---- Object, 7 args ..*...* (68)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 792: // ---- void, 7 args *.*...* (69)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 793: // ---- int, 7 args *.*...* (69)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 794: // ---- float, 7 args *.*...* (69)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 795: // ---- Object, 7 args *.*...* (69)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 796: // ---- void, 7 args .**...* (70)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 797: // ---- int, 7 args .**...* (70)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 798: // ---- float, 7 args .**...* (70)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 799: // ---- Object, 7 args .**...* (70)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 800: // ---- void, 7 args ***...* (71)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 801: // ---- int, 7 args ***...* (71)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 802: // ---- float, 7 args ***...* (71)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 803: // ---- Object, 7 args ***...* (71)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 804: // ---- void, 7 args ...*..* (72)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 805: // ---- int, 7 args ...*..* (72)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 806: // ---- float, 7 args ...*..* (72)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 807: // ---- Object, 7 args ...*..* (72)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 808: // ---- void, 7 args *..*..* (73)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 809: // ---- int, 7 args *..*..* (73)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 810: // ---- float, 7 args *..*..* (73)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 811: // ---- Object, 7 args *..*..* (73)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 812: // ---- void, 7 args .*.*..* (74)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 813: // ---- int, 7 args .*.*..* (74)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 814: // ---- float, 7 args .*.*..* (74)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 815: // ---- Object, 7 args .*.*..* (74)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 816: // ---- void, 7 args **.*..* (75)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 817: // ---- int, 7 args **.*..* (75)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 818: // ---- float, 7 args **.*..* (75)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 819: // ---- Object, 7 args **.*..* (75)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 820: // ---- void, 7 args ..**..* (76)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 821: // ---- int, 7 args ..**..* (76)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 822: // ---- float, 7 args ..**..* (76)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 823: // ---- Object, 7 args ..**..* (76)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 824: // ---- void, 7 args *.**..* (77)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 825: // ---- int, 7 args *.**..* (77)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 826: // ---- float, 7 args *.**..* (77)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 827: // ---- Object, 7 args *.**..* (77)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 828: // ---- void, 7 args .***..* (78)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 829: // ---- int, 7 args .***..* (78)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 830: // ---- float, 7 args .***..* (78)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 831: // ---- Object, 7 args .***..* (78)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 832: // ---- void, 7 args ****..* (79)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); break;
   case 833: // ---- int, 7 args ****..* (79)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 834: // ---- float, 7 args ****..* (79)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 835: // ---- Object, 7 args ****..* (79)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 836: // ---- void, 7 args ....*.* (80)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 837: // ---- int, 7 args ....*.* (80)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 838: // ---- float, 7 args ....*.* (80)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 839: // ---- Object, 7 args ....*.* (80)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 840: // ---- void, 7 args *...*.* (81)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 841: // ---- int, 7 args *...*.* (81)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 842: // ---- float, 7 args *...*.* (81)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 843: // ---- Object, 7 args *...*.* (81)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 844: // ---- void, 7 args .*..*.* (82)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 845: // ---- int, 7 args .*..*.* (82)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 846: // ---- float, 7 args .*..*.* (82)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 847: // ---- Object, 7 args .*..*.* (82)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 848: // ---- void, 7 args **..*.* (83)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 849: // ---- int, 7 args **..*.* (83)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 850: // ---- float, 7 args **..*.* (83)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 851: // ---- Object, 7 args **..*.* (83)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 852: // ---- void, 7 args ..*.*.* (84)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 853: // ---- int, 7 args ..*.*.* (84)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 854: // ---- float, 7 args ..*.*.* (84)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 855: // ---- Object, 7 args ..*.*.* (84)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 856: // ---- void, 7 args *.*.*.* (85)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 857: // ---- int, 7 args *.*.*.* (85)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 858: // ---- float, 7 args *.*.*.* (85)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 859: // ---- Object, 7 args *.*.*.* (85)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 860: // ---- void, 7 args .**.*.* (86)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 861: // ---- int, 7 args .**.*.* (86)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 862: // ---- float, 7 args .**.*.* (86)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 863: // ---- Object, 7 args .**.*.* (86)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 864: // ---- void, 7 args ***.*.* (87)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 865: // ---- int, 7 args ***.*.* (87)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 866: // ---- float, 7 args ***.*.* (87)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 867: // ---- Object, 7 args ***.*.* (87)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 868: // ---- void, 7 args ...**.* (88)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 869: // ---- int, 7 args ...**.* (88)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 870: // ---- float, 7 args ...**.* (88)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 871: // ---- Object, 7 args ...**.* (88)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 872: // ---- void, 7 args *..**.* (89)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 873: // ---- int, 7 args *..**.* (89)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 874: // ---- float, 7 args *..**.* (89)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 875: // ---- Object, 7 args *..**.* (89)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 876: // ---- void, 7 args .*.**.* (90)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 877: // ---- int, 7 args .*.**.* (90)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 878: // ---- float, 7 args .*.**.* (90)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 879: // ---- Object, 7 args .*.**.* (90)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 880: // ---- void, 7 args **.**.* (91)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 881: // ---- int, 7 args **.**.* (91)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 882: // ---- float, 7 args **.**.* (91)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 883: // ---- Object, 7 args **.**.* (91)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 884: // ---- void, 7 args ..***.* (92)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 885: // ---- int, 7 args ..***.* (92)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 886: // ---- float, 7 args ..***.* (92)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 887: // ---- Object, 7 args ..***.* (92)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 888: // ---- void, 7 args *.***.* (93)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 889: // ---- int, 7 args *.***.* (93)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 890: // ---- float, 7 args *.***.* (93)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 891: // ---- Object, 7 args *.***.* (93)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 892: // ---- void, 7 args .****.* (94)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 893: // ---- int, 7 args .****.* (94)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 894: // ---- float, 7 args .****.* (94)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 895: // ---- Object, 7 args .****.* (94)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 896: // ---- void, 7 args *****.* (95)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); break;
   case 897: // ---- int, 7 args *****.* (95)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=1; break;
   case 898: // ---- float, 7 args *****.* (95)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=2; break;
   case 899: // ---- Object, 7 args *****.* (95)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 900: // ---- void, 7 args .....** (96)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 901: // ---- int, 7 args .....** (96)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 902: // ---- float, 7 args .....** (96)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 903: // ---- Object, 7 args .....** (96)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 904: // ---- void, 7 args *....** (97)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 905: // ---- int, 7 args *....** (97)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 906: // ---- float, 7 args *....** (97)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 907: // ---- Object, 7 args *....** (97)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 908: // ---- void, 7 args .*...** (98)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 909: // ---- int, 7 args .*...** (98)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 910: // ---- float, 7 args .*...** (98)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 911: // ---- Object, 7 args .*...** (98)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 912: // ---- void, 7 args **...** (99)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 913: // ---- int, 7 args **...** (99)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 914: // ---- float, 7 args **...** (99)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 915: // ---- Object, 7 args **...** (99)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 916: // ---- void, 7 args ..*..** (100)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 917: // ---- int, 7 args ..*..** (100)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 918: // ---- float, 7 args ..*..** (100)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 919: // ---- Object, 7 args ..*..** (100)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 920: // ---- void, 7 args *.*..** (101)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 921: // ---- int, 7 args *.*..** (101)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 922: // ---- float, 7 args *.*..** (101)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 923: // ---- Object, 7 args *.*..** (101)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 924: // ---- void, 7 args .**..** (102)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 925: // ---- int, 7 args .**..** (102)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 926: // ---- float, 7 args .**..** (102)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 927: // ---- Object, 7 args .**..** (102)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 928: // ---- void, 7 args ***..** (103)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 929: // ---- int, 7 args ***..** (103)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 930: // ---- float, 7 args ***..** (103)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 931: // ---- Object, 7 args ***..** (103)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 932: // ---- void, 7 args ...*.** (104)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 933: // ---- int, 7 args ...*.** (104)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 934: // ---- float, 7 args ...*.** (104)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 935: // ---- Object, 7 args ...*.** (104)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 936: // ---- void, 7 args *..*.** (105)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 937: // ---- int, 7 args *..*.** (105)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 938: // ---- float, 7 args *..*.** (105)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 939: // ---- Object, 7 args *..*.** (105)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 940: // ---- void, 7 args .*.*.** (106)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 941: // ---- int, 7 args .*.*.** (106)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 942: // ---- float, 7 args .*.*.** (106)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 943: // ---- Object, 7 args .*.*.** (106)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 944: // ---- void, 7 args **.*.** (107)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 945: // ---- int, 7 args **.*.** (107)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 946: // ---- float, 7 args **.*.** (107)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 947: // ---- Object, 7 args **.*.** (107)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 948: // ---- void, 7 args ..**.** (108)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 949: // ---- int, 7 args ..**.** (108)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 950: // ---- float, 7 args ..**.** (108)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 951: // ---- Object, 7 args ..**.** (108)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 952: // ---- void, 7 args *.**.** (109)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 953: // ---- int, 7 args *.**.** (109)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 954: // ---- float, 7 args *.**.** (109)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 955: // ---- Object, 7 args *.**.** (109)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 956: // ---- void, 7 args .***.** (110)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 957: // ---- int, 7 args .***.** (110)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 958: // ---- float, 7 args .***.** (110)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 959: // ---- Object, 7 args .***.** (110)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 960: // ---- void, 7 args ****.** (111)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); break;
   case 961: // ---- int, 7 args ****.** (111)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 962: // ---- float, 7 args ****.** (111)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 963: // ---- Object, 7 args ****.** (111)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 964: // ---- void, 7 args ....*** (112)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 965: // ---- int, 7 args ....*** (112)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 966: // ---- float, 7 args ....*** (112)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 967: // ---- Object, 7 args ....*** (112)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 968: // ---- void, 7 args *...*** (113)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 969: // ---- int, 7 args *...*** (113)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 970: // ---- float, 7 args *...*** (113)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 971: // ---- Object, 7 args *...*** (113)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 972: // ---- void, 7 args .*..*** (114)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 973: // ---- int, 7 args .*..*** (114)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 974: // ---- float, 7 args .*..*** (114)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 975: // ---- Object, 7 args .*..*** (114)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 976: // ---- void, 7 args **..*** (115)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 977: // ---- int, 7 args **..*** (115)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 978: // ---- float, 7 args **..*** (115)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 979: // ---- Object, 7 args **..*** (115)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 980: // ---- void, 7 args ..*.*** (116)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 981: // ---- int, 7 args ..*.*** (116)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 982: // ---- float, 7 args ..*.*** (116)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 983: // ---- Object, 7 args ..*.*** (116)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 984: // ---- void, 7 args *.*.*** (117)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 985: // ---- int, 7 args *.*.*** (117)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 986: // ---- float, 7 args *.*.*** (117)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 987: // ---- Object, 7 args *.*.*** (117)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 988: // ---- void, 7 args .**.*** (118)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 989: // ---- int, 7 args .**.*** (118)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 990: // ---- float, 7 args .**.*** (118)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 991: // ---- Object, 7 args .**.*** (118)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 992: // ---- void, 7 args ***.*** (119)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 993: // ---- int, 7 args ***.*** (119)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 994: // ---- float, 7 args ***.*** (119)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 995: // ---- Object, 7 args ***.*** (119)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 996: // ---- void, 7 args ...**** (120)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 997: // ---- int, 7 args ...**** (120)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 998: // ---- float, 7 args ...**** (120)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 999: // ---- Object, 7 args ...**** (120)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1000: // ---- void, 7 args *..**** (121)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 1001: // ---- int, 7 args *..**** (121)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 1002: // ---- float, 7 args *..**** (121)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 1003: // ---- Object, 7 args *..**** (121)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1004: // ---- void, 7 args .*.**** (122)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 1005: // ---- int, 7 args .*.**** (122)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 1006: // ---- float, 7 args .*.**** (122)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 1007: // ---- Object, 7 args .*.**** (122)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1008: // ---- void, 7 args **.**** (123)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 1009: // ---- int, 7 args **.**** (123)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 1010: // ---- float, 7 args **.**** (123)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 1011: // ---- Object, 7 args **.**** (123)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1012: // ---- void, 7 args ..***** (124)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 1013: // ---- int, 7 args ..***** (124)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 1014: // ---- float, 7 args ..***** (124)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 1015: // ---- Object, 7 args ..***** (124)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1016: // ---- void, 7 args *.***** (125)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 1017: // ---- int, 7 args *.***** (125)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 1018: // ---- float, 7 args *.***** (125)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 1019: // ---- Object, 7 args *.***** (125)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1020: // ---- void, 7 args .****** (126)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 1021: // ---- int, 7 args .****** (126)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 1022: // ---- float, 7 args .****** (126)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 1023: // ---- Object, 7 args .****** (126)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1024: // ---- void, 7 args ******* (127)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); break;
   case 1025: // ---- int, 7 args ******* (127)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=1; break;
   case 1026: // ---- float, 7 args ******* (127)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=2; break;
   case 1027: // ---- Object, 7 args ******* (127)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1028: // ---- void, 8 args ........ (0)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1029: // ---- int, 8 args ........ (0)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1030: // ---- float, 8 args ........ (0)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1031: // ---- Object, 8 args ........ (0)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1032: // ---- void, 8 args *....... (1)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1033: // ---- int, 8 args *....... (1)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1034: // ---- float, 8 args *....... (1)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1035: // ---- Object, 8 args *....... (1)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1036: // ---- void, 8 args .*...... (2)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1037: // ---- int, 8 args .*...... (2)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1038: // ---- float, 8 args .*...... (2)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1039: // ---- Object, 8 args .*...... (2)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1040: // ---- void, 8 args **...... (3)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1041: // ---- int, 8 args **...... (3)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1042: // ---- float, 8 args **...... (3)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1043: // ---- Object, 8 args **...... (3)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1044: // ---- void, 8 args ..*..... (4)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1045: // ---- int, 8 args ..*..... (4)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1046: // ---- float, 8 args ..*..... (4)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1047: // ---- Object, 8 args ..*..... (4)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1048: // ---- void, 8 args *.*..... (5)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1049: // ---- int, 8 args *.*..... (5)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1050: // ---- float, 8 args *.*..... (5)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1051: // ---- Object, 8 args *.*..... (5)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1052: // ---- void, 8 args .**..... (6)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1053: // ---- int, 8 args .**..... (6)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1054: // ---- float, 8 args .**..... (6)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1055: // ---- Object, 8 args .**..... (6)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1056: // ---- void, 8 args ***..... (7)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1057: // ---- int, 8 args ***..... (7)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1058: // ---- float, 8 args ***..... (7)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1059: // ---- Object, 8 args ***..... (7)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1060: // ---- void, 8 args ...*.... (8)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1061: // ---- int, 8 args ...*.... (8)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1062: // ---- float, 8 args ...*.... (8)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1063: // ---- Object, 8 args ...*.... (8)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1064: // ---- void, 8 args *..*.... (9)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1065: // ---- int, 8 args *..*.... (9)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1066: // ---- float, 8 args *..*.... (9)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1067: // ---- Object, 8 args *..*.... (9)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1068: // ---- void, 8 args .*.*.... (10)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1069: // ---- int, 8 args .*.*.... (10)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1070: // ---- float, 8 args .*.*.... (10)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1071: // ---- Object, 8 args .*.*.... (10)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1072: // ---- void, 8 args **.*.... (11)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1073: // ---- int, 8 args **.*.... (11)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1074: // ---- float, 8 args **.*.... (11)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1075: // ---- Object, 8 args **.*.... (11)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1076: // ---- void, 8 args ..**.... (12)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1077: // ---- int, 8 args ..**.... (12)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1078: // ---- float, 8 args ..**.... (12)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1079: // ---- Object, 8 args ..**.... (12)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1080: // ---- void, 8 args *.**.... (13)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1081: // ---- int, 8 args *.**.... (13)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1082: // ---- float, 8 args *.**.... (13)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1083: // ---- Object, 8 args *.**.... (13)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1084: // ---- void, 8 args .***.... (14)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1085: // ---- int, 8 args .***.... (14)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1086: // ---- float, 8 args .***.... (14)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1087: // ---- Object, 8 args .***.... (14)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1088: // ---- void, 8 args ****.... (15)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1089: // ---- int, 8 args ****.... (15)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1090: // ---- float, 8 args ****.... (15)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1091: // ---- Object, 8 args ****.... (15)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1092: // ---- void, 8 args ....*... (16)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1093: // ---- int, 8 args ....*... (16)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1094: // ---- float, 8 args ....*... (16)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1095: // ---- Object, 8 args ....*... (16)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1096: // ---- void, 8 args *...*... (17)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1097: // ---- int, 8 args *...*... (17)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1098: // ---- float, 8 args *...*... (17)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1099: // ---- Object, 8 args *...*... (17)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1100: // ---- void, 8 args .*..*... (18)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1101: // ---- int, 8 args .*..*... (18)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1102: // ---- float, 8 args .*..*... (18)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1103: // ---- Object, 8 args .*..*... (18)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1104: // ---- void, 8 args **..*... (19)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1105: // ---- int, 8 args **..*... (19)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1106: // ---- float, 8 args **..*... (19)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1107: // ---- Object, 8 args **..*... (19)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1108: // ---- void, 8 args ..*.*... (20)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1109: // ---- int, 8 args ..*.*... (20)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1110: // ---- float, 8 args ..*.*... (20)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1111: // ---- Object, 8 args ..*.*... (20)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1112: // ---- void, 8 args *.*.*... (21)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1113: // ---- int, 8 args *.*.*... (21)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1114: // ---- float, 8 args *.*.*... (21)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1115: // ---- Object, 8 args *.*.*... (21)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1116: // ---- void, 8 args .**.*... (22)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1117: // ---- int, 8 args .**.*... (22)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1118: // ---- float, 8 args .**.*... (22)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1119: // ---- Object, 8 args .**.*... (22)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1120: // ---- void, 8 args ***.*... (23)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1121: // ---- int, 8 args ***.*... (23)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1122: // ---- float, 8 args ***.*... (23)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1123: // ---- Object, 8 args ***.*... (23)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1124: // ---- void, 8 args ...**... (24)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1125: // ---- int, 8 args ...**... (24)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1126: // ---- float, 8 args ...**... (24)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1127: // ---- Object, 8 args ...**... (24)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1128: // ---- void, 8 args *..**... (25)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1129: // ---- int, 8 args *..**... (25)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1130: // ---- float, 8 args *..**... (25)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1131: // ---- Object, 8 args *..**... (25)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1132: // ---- void, 8 args .*.**... (26)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1133: // ---- int, 8 args .*.**... (26)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1134: // ---- float, 8 args .*.**... (26)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1135: // ---- Object, 8 args .*.**... (26)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1136: // ---- void, 8 args **.**... (27)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1137: // ---- int, 8 args **.**... (27)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1138: // ---- float, 8 args **.**... (27)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1139: // ---- Object, 8 args **.**... (27)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1140: // ---- void, 8 args ..***... (28)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1141: // ---- int, 8 args ..***... (28)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1142: // ---- float, 8 args ..***... (28)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1143: // ---- Object, 8 args ..***... (28)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1144: // ---- void, 8 args *.***... (29)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1145: // ---- int, 8 args *.***... (29)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1146: // ---- float, 8 args *.***... (29)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1147: // ---- Object, 8 args *.***... (29)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1148: // ---- void, 8 args .****... (30)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1149: // ---- int, 8 args .****... (30)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1150: // ---- float, 8 args .****... (30)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1151: // ---- Object, 8 args .****... (30)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1152: // ---- void, 8 args *****... (31)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); break;
   case 1153: // ---- int, 8 args *****... (31)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1154: // ---- float, 8 args *****... (31)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1155: // ---- Object, 8 args *****... (31)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1156: // ---- void, 8 args .....*.. (32)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1157: // ---- int, 8 args .....*.. (32)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1158: // ---- float, 8 args .....*.. (32)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1159: // ---- Object, 8 args .....*.. (32)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1160: // ---- void, 8 args *....*.. (33)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1161: // ---- int, 8 args *....*.. (33)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1162: // ---- float, 8 args *....*.. (33)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1163: // ---- Object, 8 args *....*.. (33)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1164: // ---- void, 8 args .*...*.. (34)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1165: // ---- int, 8 args .*...*.. (34)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1166: // ---- float, 8 args .*...*.. (34)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1167: // ---- Object, 8 args .*...*.. (34)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1168: // ---- void, 8 args **...*.. (35)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1169: // ---- int, 8 args **...*.. (35)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1170: // ---- float, 8 args **...*.. (35)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1171: // ---- Object, 8 args **...*.. (35)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1172: // ---- void, 8 args ..*..*.. (36)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1173: // ---- int, 8 args ..*..*.. (36)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1174: // ---- float, 8 args ..*..*.. (36)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1175: // ---- Object, 8 args ..*..*.. (36)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1176: // ---- void, 8 args *.*..*.. (37)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1177: // ---- int, 8 args *.*..*.. (37)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1178: // ---- float, 8 args *.*..*.. (37)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1179: // ---- Object, 8 args *.*..*.. (37)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1180: // ---- void, 8 args .**..*.. (38)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1181: // ---- int, 8 args .**..*.. (38)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1182: // ---- float, 8 args .**..*.. (38)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1183: // ---- Object, 8 args .**..*.. (38)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1184: // ---- void, 8 args ***..*.. (39)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1185: // ---- int, 8 args ***..*.. (39)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1186: // ---- float, 8 args ***..*.. (39)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1187: // ---- Object, 8 args ***..*.. (39)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1188: // ---- void, 8 args ...*.*.. (40)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1189: // ---- int, 8 args ...*.*.. (40)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1190: // ---- float, 8 args ...*.*.. (40)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1191: // ---- Object, 8 args ...*.*.. (40)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1192: // ---- void, 8 args *..*.*.. (41)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1193: // ---- int, 8 args *..*.*.. (41)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1194: // ---- float, 8 args *..*.*.. (41)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1195: // ---- Object, 8 args *..*.*.. (41)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1196: // ---- void, 8 args .*.*.*.. (42)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1197: // ---- int, 8 args .*.*.*.. (42)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1198: // ---- float, 8 args .*.*.*.. (42)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1199: // ---- Object, 8 args .*.*.*.. (42)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1200: // ---- void, 8 args **.*.*.. (43)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1201: // ---- int, 8 args **.*.*.. (43)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1202: // ---- float, 8 args **.*.*.. (43)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1203: // ---- Object, 8 args **.*.*.. (43)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1204: // ---- void, 8 args ..**.*.. (44)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1205: // ---- int, 8 args ..**.*.. (44)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1206: // ---- float, 8 args ..**.*.. (44)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1207: // ---- Object, 8 args ..**.*.. (44)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1208: // ---- void, 8 args *.**.*.. (45)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1209: // ---- int, 8 args *.**.*.. (45)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1210: // ---- float, 8 args *.**.*.. (45)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1211: // ---- Object, 8 args *.**.*.. (45)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1212: // ---- void, 8 args .***.*.. (46)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1213: // ---- int, 8 args .***.*.. (46)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1214: // ---- float, 8 args .***.*.. (46)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1215: // ---- Object, 8 args .***.*.. (46)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1216: // ---- void, 8 args ****.*.. (47)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1217: // ---- int, 8 args ****.*.. (47)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1218: // ---- float, 8 args ****.*.. (47)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1219: // ---- Object, 8 args ****.*.. (47)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1220: // ---- void, 8 args ....**.. (48)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1221: // ---- int, 8 args ....**.. (48)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1222: // ---- float, 8 args ....**.. (48)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1223: // ---- Object, 8 args ....**.. (48)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1224: // ---- void, 8 args *...**.. (49)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1225: // ---- int, 8 args *...**.. (49)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1226: // ---- float, 8 args *...**.. (49)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1227: // ---- Object, 8 args *...**.. (49)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1228: // ---- void, 8 args .*..**.. (50)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1229: // ---- int, 8 args .*..**.. (50)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1230: // ---- float, 8 args .*..**.. (50)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1231: // ---- Object, 8 args .*..**.. (50)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1232: // ---- void, 8 args **..**.. (51)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1233: // ---- int, 8 args **..**.. (51)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1234: // ---- float, 8 args **..**.. (51)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1235: // ---- Object, 8 args **..**.. (51)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1236: // ---- void, 8 args ..*.**.. (52)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1237: // ---- int, 8 args ..*.**.. (52)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1238: // ---- float, 8 args ..*.**.. (52)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1239: // ---- Object, 8 args ..*.**.. (52)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1240: // ---- void, 8 args *.*.**.. (53)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1241: // ---- int, 8 args *.*.**.. (53)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1242: // ---- float, 8 args *.*.**.. (53)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1243: // ---- Object, 8 args *.*.**.. (53)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1244: // ---- void, 8 args .**.**.. (54)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1245: // ---- int, 8 args .**.**.. (54)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1246: // ---- float, 8 args .**.**.. (54)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1247: // ---- Object, 8 args .**.**.. (54)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1248: // ---- void, 8 args ***.**.. (55)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1249: // ---- int, 8 args ***.**.. (55)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1250: // ---- float, 8 args ***.**.. (55)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1251: // ---- Object, 8 args ***.**.. (55)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1252: // ---- void, 8 args ...***.. (56)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1253: // ---- int, 8 args ...***.. (56)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1254: // ---- float, 8 args ...***.. (56)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1255: // ---- Object, 8 args ...***.. (56)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1256: // ---- void, 8 args *..***.. (57)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1257: // ---- int, 8 args *..***.. (57)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1258: // ---- float, 8 args *..***.. (57)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1259: // ---- Object, 8 args *..***.. (57)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1260: // ---- void, 8 args .*.***.. (58)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1261: // ---- int, 8 args .*.***.. (58)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1262: // ---- float, 8 args .*.***.. (58)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1263: // ---- Object, 8 args .*.***.. (58)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1264: // ---- void, 8 args **.***.. (59)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1265: // ---- int, 8 args **.***.. (59)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1266: // ---- float, 8 args **.***.. (59)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1267: // ---- Object, 8 args **.***.. (59)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1268: // ---- void, 8 args ..****.. (60)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1269: // ---- int, 8 args ..****.. (60)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1270: // ---- float, 8 args ..****.. (60)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1271: // ---- Object, 8 args ..****.. (60)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1272: // ---- void, 8 args *.****.. (61)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1273: // ---- int, 8 args *.****.. (61)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1274: // ---- float, 8 args *.****.. (61)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1275: // ---- Object, 8 args *.****.. (61)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1276: // ---- void, 8 args .*****.. (62)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1277: // ---- int, 8 args .*****.. (62)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1278: // ---- float, 8 args .*****.. (62)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1279: // ---- Object, 8 args .*****.. (62)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1280: // ---- void, 8 args ******.. (63)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); break;
   case 1281: // ---- int, 8 args ******.. (63)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=1; break;
   case 1282: // ---- float, 8 args ******.. (63)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=2; break;
   case 1283: // ---- Object, 8 args ******.. (63)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, void*, sUI, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1284: // ---- void, 8 args ......*. (64)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1285: // ---- int, 8 args ......*. (64)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1286: // ---- float, 8 args ......*. (64)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1287: // ---- Object, 8 args ......*. (64)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1288: // ---- void, 8 args *.....*. (65)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1289: // ---- int, 8 args *.....*. (65)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1290: // ---- float, 8 args *.....*. (65)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1291: // ---- Object, 8 args *.....*. (65)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1292: // ---- void, 8 args .*....*. (66)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1293: // ---- int, 8 args .*....*. (66)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1294: // ---- float, 8 args .*....*. (66)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1295: // ---- Object, 8 args .*....*. (66)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1296: // ---- void, 8 args **....*. (67)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1297: // ---- int, 8 args **....*. (67)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1298: // ---- float, 8 args **....*. (67)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1299: // ---- Object, 8 args **....*. (67)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1300: // ---- void, 8 args ..*...*. (68)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1301: // ---- int, 8 args ..*...*. (68)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1302: // ---- float, 8 args ..*...*. (68)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1303: // ---- Object, 8 args ..*...*. (68)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1304: // ---- void, 8 args *.*...*. (69)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1305: // ---- int, 8 args *.*...*. (69)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1306: // ---- float, 8 args *.*...*. (69)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1307: // ---- Object, 8 args *.*...*. (69)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1308: // ---- void, 8 args .**...*. (70)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1309: // ---- int, 8 args .**...*. (70)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1310: // ---- float, 8 args .**...*. (70)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1311: // ---- Object, 8 args .**...*. (70)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1312: // ---- void, 8 args ***...*. (71)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1313: // ---- int, 8 args ***...*. (71)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1314: // ---- float, 8 args ***...*. (71)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1315: // ---- Object, 8 args ***...*. (71)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1316: // ---- void, 8 args ...*..*. (72)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1317: // ---- int, 8 args ...*..*. (72)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1318: // ---- float, 8 args ...*..*. (72)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1319: // ---- Object, 8 args ...*..*. (72)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1320: // ---- void, 8 args *..*..*. (73)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1321: // ---- int, 8 args *..*..*. (73)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1322: // ---- float, 8 args *..*..*. (73)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1323: // ---- Object, 8 args *..*..*. (73)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1324: // ---- void, 8 args .*.*..*. (74)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1325: // ---- int, 8 args .*.*..*. (74)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1326: // ---- float, 8 args .*.*..*. (74)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1327: // ---- Object, 8 args .*.*..*. (74)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1328: // ---- void, 8 args **.*..*. (75)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1329: // ---- int, 8 args **.*..*. (75)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1330: // ---- float, 8 args **.*..*. (75)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1331: // ---- Object, 8 args **.*..*. (75)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1332: // ---- void, 8 args ..**..*. (76)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1333: // ---- int, 8 args ..**..*. (76)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1334: // ---- float, 8 args ..**..*. (76)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1335: // ---- Object, 8 args ..**..*. (76)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1336: // ---- void, 8 args *.**..*. (77)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1337: // ---- int, 8 args *.**..*. (77)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1338: // ---- float, 8 args *.**..*. (77)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1339: // ---- Object, 8 args *.**..*. (77)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1340: // ---- void, 8 args .***..*. (78)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1341: // ---- int, 8 args .***..*. (78)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1342: // ---- float, 8 args .***..*. (78)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1343: // ---- Object, 8 args .***..*. (78)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1344: // ---- void, 8 args ****..*. (79)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1345: // ---- int, 8 args ****..*. (79)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1346: // ---- float, 8 args ****..*. (79)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1347: // ---- Object, 8 args ****..*. (79)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1348: // ---- void, 8 args ....*.*. (80)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1349: // ---- int, 8 args ....*.*. (80)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1350: // ---- float, 8 args ....*.*. (80)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1351: // ---- Object, 8 args ....*.*. (80)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1352: // ---- void, 8 args *...*.*. (81)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1353: // ---- int, 8 args *...*.*. (81)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1354: // ---- float, 8 args *...*.*. (81)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1355: // ---- Object, 8 args *...*.*. (81)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1356: // ---- void, 8 args .*..*.*. (82)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1357: // ---- int, 8 args .*..*.*. (82)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1358: // ---- float, 8 args .*..*.*. (82)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1359: // ---- Object, 8 args .*..*.*. (82)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1360: // ---- void, 8 args **..*.*. (83)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1361: // ---- int, 8 args **..*.*. (83)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1362: // ---- float, 8 args **..*.*. (83)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1363: // ---- Object, 8 args **..*.*. (83)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1364: // ---- void, 8 args ..*.*.*. (84)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1365: // ---- int, 8 args ..*.*.*. (84)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1366: // ---- float, 8 args ..*.*.*. (84)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1367: // ---- Object, 8 args ..*.*.*. (84)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1368: // ---- void, 8 args *.*.*.*. (85)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1369: // ---- int, 8 args *.*.*.*. (85)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1370: // ---- float, 8 args *.*.*.*. (85)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1371: // ---- Object, 8 args *.*.*.*. (85)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1372: // ---- void, 8 args .**.*.*. (86)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1373: // ---- int, 8 args .**.*.*. (86)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1374: // ---- float, 8 args .**.*.*. (86)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1375: // ---- Object, 8 args .**.*.*. (86)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1376: // ---- void, 8 args ***.*.*. (87)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1377: // ---- int, 8 args ***.*.*. (87)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1378: // ---- float, 8 args ***.*.*. (87)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1379: // ---- Object, 8 args ***.*.*. (87)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1380: // ---- void, 8 args ...**.*. (88)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1381: // ---- int, 8 args ...**.*. (88)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1382: // ---- float, 8 args ...**.*. (88)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1383: // ---- Object, 8 args ...**.*. (88)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1384: // ---- void, 8 args *..**.*. (89)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1385: // ---- int, 8 args *..**.*. (89)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1386: // ---- float, 8 args *..**.*. (89)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1387: // ---- Object, 8 args *..**.*. (89)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1388: // ---- void, 8 args .*.**.*. (90)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1389: // ---- int, 8 args .*.**.*. (90)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1390: // ---- float, 8 args .*.**.*. (90)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1391: // ---- Object, 8 args .*.**.*. (90)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1392: // ---- void, 8 args **.**.*. (91)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1393: // ---- int, 8 args **.**.*. (91)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1394: // ---- float, 8 args **.**.*. (91)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1395: // ---- Object, 8 args **.**.*. (91)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1396: // ---- void, 8 args ..***.*. (92)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1397: // ---- int, 8 args ..***.*. (92)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1398: // ---- float, 8 args ..***.*. (92)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1399: // ---- Object, 8 args ..***.*. (92)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1400: // ---- void, 8 args *.***.*. (93)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1401: // ---- int, 8 args *.***.*. (93)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1402: // ---- float, 8 args *.***.*. (93)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1403: // ---- Object, 8 args *.***.*. (93)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1404: // ---- void, 8 args .****.*. (94)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1405: // ---- int, 8 args .****.*. (94)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1406: // ---- float, 8 args .****.*. (94)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1407: // ---- Object, 8 args .****.*. (94)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1408: // ---- void, 8 args *****.*. (95)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); break;
   case 1409: // ---- int, 8 args *****.*. (95)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1410: // ---- float, 8 args *****.*. (95)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1411: // ---- Object, 8 args *****.*. (95)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1412: // ---- void, 8 args .....**. (96)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1413: // ---- int, 8 args .....**. (96)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1414: // ---- float, 8 args .....**. (96)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1415: // ---- Object, 8 args .....**. (96)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1416: // ---- void, 8 args *....**. (97)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1417: // ---- int, 8 args *....**. (97)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1418: // ---- float, 8 args *....**. (97)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1419: // ---- Object, 8 args *....**. (97)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1420: // ---- void, 8 args .*...**. (98)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1421: // ---- int, 8 args .*...**. (98)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1422: // ---- float, 8 args .*...**. (98)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1423: // ---- Object, 8 args .*...**. (98)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1424: // ---- void, 8 args **...**. (99)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1425: // ---- int, 8 args **...**. (99)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1426: // ---- float, 8 args **...**. (99)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1427: // ---- Object, 8 args **...**. (99)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1428: // ---- void, 8 args ..*..**. (100)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1429: // ---- int, 8 args ..*..**. (100)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1430: // ---- float, 8 args ..*..**. (100)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1431: // ---- Object, 8 args ..*..**. (100)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1432: // ---- void, 8 args *.*..**. (101)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1433: // ---- int, 8 args *.*..**. (101)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1434: // ---- float, 8 args *.*..**. (101)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1435: // ---- Object, 8 args *.*..**. (101)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1436: // ---- void, 8 args .**..**. (102)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1437: // ---- int, 8 args .**..**. (102)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1438: // ---- float, 8 args .**..**. (102)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1439: // ---- Object, 8 args .**..**. (102)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1440: // ---- void, 8 args ***..**. (103)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1441: // ---- int, 8 args ***..**. (103)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1442: // ---- float, 8 args ***..**. (103)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1443: // ---- Object, 8 args ***..**. (103)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1444: // ---- void, 8 args ...*.**. (104)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1445: // ---- int, 8 args ...*.**. (104)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1446: // ---- float, 8 args ...*.**. (104)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1447: // ---- Object, 8 args ...*.**. (104)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1448: // ---- void, 8 args *..*.**. (105)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1449: // ---- int, 8 args *..*.**. (105)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1450: // ---- float, 8 args *..*.**. (105)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1451: // ---- Object, 8 args *..*.**. (105)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1452: // ---- void, 8 args .*.*.**. (106)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1453: // ---- int, 8 args .*.*.**. (106)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1454: // ---- float, 8 args .*.*.**. (106)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1455: // ---- Object, 8 args .*.*.**. (106)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1456: // ---- void, 8 args **.*.**. (107)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1457: // ---- int, 8 args **.*.**. (107)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1458: // ---- float, 8 args **.*.**. (107)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1459: // ---- Object, 8 args **.*.**. (107)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1460: // ---- void, 8 args ..**.**. (108)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1461: // ---- int, 8 args ..**.**. (108)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1462: // ---- float, 8 args ..**.**. (108)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1463: // ---- Object, 8 args ..**.**. (108)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1464: // ---- void, 8 args *.**.**. (109)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1465: // ---- int, 8 args *.**.**. (109)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1466: // ---- float, 8 args *.**.**. (109)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1467: // ---- Object, 8 args *.**.**. (109)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1468: // ---- void, 8 args .***.**. (110)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1469: // ---- int, 8 args .***.**. (110)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1470: // ---- float, 8 args .***.**. (110)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1471: // ---- Object, 8 args .***.**. (110)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1472: // ---- void, 8 args ****.**. (111)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1473: // ---- int, 8 args ****.**. (111)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1474: // ---- float, 8 args ****.**. (111)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1475: // ---- Object, 8 args ****.**. (111)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1476: // ---- void, 8 args ....***. (112)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1477: // ---- int, 8 args ....***. (112)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1478: // ---- float, 8 args ....***. (112)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1479: // ---- Object, 8 args ....***. (112)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1480: // ---- void, 8 args *...***. (113)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1481: // ---- int, 8 args *...***. (113)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1482: // ---- float, 8 args *...***. (113)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1483: // ---- Object, 8 args *...***. (113)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1484: // ---- void, 8 args .*..***. (114)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1485: // ---- int, 8 args .*..***. (114)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1486: // ---- float, 8 args .*..***. (114)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1487: // ---- Object, 8 args .*..***. (114)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1488: // ---- void, 8 args **..***. (115)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1489: // ---- int, 8 args **..***. (115)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1490: // ---- float, 8 args **..***. (115)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1491: // ---- Object, 8 args **..***. (115)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1492: // ---- void, 8 args ..*.***. (116)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1493: // ---- int, 8 args ..*.***. (116)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1494: // ---- float, 8 args ..*.***. (116)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1495: // ---- Object, 8 args ..*.***. (116)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1496: // ---- void, 8 args *.*.***. (117)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1497: // ---- int, 8 args *.*.***. (117)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1498: // ---- float, 8 args *.*.***. (117)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1499: // ---- Object, 8 args *.*.***. (117)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1500: // ---- void, 8 args .**.***. (118)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1501: // ---- int, 8 args .**.***. (118)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1502: // ---- float, 8 args .**.***. (118)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1503: // ---- Object, 8 args .**.***. (118)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1504: // ---- void, 8 args ***.***. (119)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1505: // ---- int, 8 args ***.***. (119)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1506: // ---- float, 8 args ***.***. (119)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1507: // ---- Object, 8 args ***.***. (119)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1508: // ---- void, 8 args ...****. (120)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1509: // ---- int, 8 args ...****. (120)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1510: // ---- float, 8 args ...****. (120)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1511: // ---- Object, 8 args ...****. (120)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1512: // ---- void, 8 args *..****. (121)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1513: // ---- int, 8 args *..****. (121)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1514: // ---- float, 8 args *..****. (121)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1515: // ---- Object, 8 args *..****. (121)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1516: // ---- void, 8 args .*.****. (122)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1517: // ---- int, 8 args .*.****. (122)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1518: // ---- float, 8 args .*.****. (122)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1519: // ---- Object, 8 args .*.****. (122)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1520: // ---- void, 8 args **.****. (123)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1521: // ---- int, 8 args **.****. (123)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1522: // ---- float, 8 args **.****. (123)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1523: // ---- Object, 8 args **.****. (123)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1524: // ---- void, 8 args ..*****. (124)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1525: // ---- int, 8 args ..*****. (124)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1526: // ---- float, 8 args ..*****. (124)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1527: // ---- Object, 8 args ..*****. (124)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1528: // ---- void, 8 args *.*****. (125)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1529: // ---- int, 8 args *.*****. (125)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1530: // ---- float, 8 args *.*****. (125)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1531: // ---- Object, 8 args *.*****. (125)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1532: // ---- void, 8 args .******. (126)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1533: // ---- int, 8 args .******. (126)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1534: // ---- float, 8 args .******. (126)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1535: // ---- Object, 8 args .******. (126)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1536: // ---- void, 8 args *******. (127)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); break;
   case 1537: // ---- int, 8 args *******. (127)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=1; break;
   case 1538: // ---- float, 8 args *******. (127)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=2; break;
   case 1539: // ---- Object, 8 args *******. (127)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, void*, void*, sUI))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].ui); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1540: // ---- void, 8 args .......* (128)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1541: // ---- int, 8 args .......* (128)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1542: // ---- float, 8 args .......* (128)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1543: // ---- Object, 8 args .......* (128)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1544: // ---- void, 8 args *......* (129)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1545: // ---- int, 8 args *......* (129)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1546: // ---- float, 8 args *......* (129)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1547: // ---- Object, 8 args *......* (129)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1548: // ---- void, 8 args .*.....* (130)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1549: // ---- int, 8 args .*.....* (130)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1550: // ---- float, 8 args .*.....* (130)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1551: // ---- Object, 8 args .*.....* (130)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1552: // ---- void, 8 args **.....* (131)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1553: // ---- int, 8 args **.....* (131)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1554: // ---- float, 8 args **.....* (131)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1555: // ---- Object, 8 args **.....* (131)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1556: // ---- void, 8 args ..*....* (132)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1557: // ---- int, 8 args ..*....* (132)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1558: // ---- float, 8 args ..*....* (132)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1559: // ---- Object, 8 args ..*....* (132)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1560: // ---- void, 8 args *.*....* (133)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1561: // ---- int, 8 args *.*....* (133)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1562: // ---- float, 8 args *.*....* (133)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1563: // ---- Object, 8 args *.*....* (133)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1564: // ---- void, 8 args .**....* (134)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1565: // ---- int, 8 args .**....* (134)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1566: // ---- float, 8 args .**....* (134)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1567: // ---- Object, 8 args .**....* (134)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1568: // ---- void, 8 args ***....* (135)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1569: // ---- int, 8 args ***....* (135)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1570: // ---- float, 8 args ***....* (135)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1571: // ---- Object, 8 args ***....* (135)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1572: // ---- void, 8 args ...*...* (136)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1573: // ---- int, 8 args ...*...* (136)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1574: // ---- float, 8 args ...*...* (136)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1575: // ---- Object, 8 args ...*...* (136)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1576: // ---- void, 8 args *..*...* (137)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1577: // ---- int, 8 args *..*...* (137)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1578: // ---- float, 8 args *..*...* (137)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1579: // ---- Object, 8 args *..*...* (137)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1580: // ---- void, 8 args .*.*...* (138)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1581: // ---- int, 8 args .*.*...* (138)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1582: // ---- float, 8 args .*.*...* (138)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1583: // ---- Object, 8 args .*.*...* (138)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1584: // ---- void, 8 args **.*...* (139)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1585: // ---- int, 8 args **.*...* (139)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1586: // ---- float, 8 args **.*...* (139)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1587: // ---- Object, 8 args **.*...* (139)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1588: // ---- void, 8 args ..**...* (140)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1589: // ---- int, 8 args ..**...* (140)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1590: // ---- float, 8 args ..**...* (140)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1591: // ---- Object, 8 args ..**...* (140)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1592: // ---- void, 8 args *.**...* (141)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1593: // ---- int, 8 args *.**...* (141)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1594: // ---- float, 8 args *.**...* (141)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1595: // ---- Object, 8 args *.**...* (141)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1596: // ---- void, 8 args .***...* (142)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1597: // ---- int, 8 args .***...* (142)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1598: // ---- float, 8 args .***...* (142)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1599: // ---- Object, 8 args .***...* (142)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1600: // ---- void, 8 args ****...* (143)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1601: // ---- int, 8 args ****...* (143)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1602: // ---- float, 8 args ****...* (143)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1603: // ---- Object, 8 args ****...* (143)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1604: // ---- void, 8 args ....*..* (144)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1605: // ---- int, 8 args ....*..* (144)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1606: // ---- float, 8 args ....*..* (144)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1607: // ---- Object, 8 args ....*..* (144)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1608: // ---- void, 8 args *...*..* (145)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1609: // ---- int, 8 args *...*..* (145)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1610: // ---- float, 8 args *...*..* (145)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1611: // ---- Object, 8 args *...*..* (145)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1612: // ---- void, 8 args .*..*..* (146)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1613: // ---- int, 8 args .*..*..* (146)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1614: // ---- float, 8 args .*..*..* (146)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1615: // ---- Object, 8 args .*..*..* (146)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1616: // ---- void, 8 args **..*..* (147)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1617: // ---- int, 8 args **..*..* (147)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1618: // ---- float, 8 args **..*..* (147)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1619: // ---- Object, 8 args **..*..* (147)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1620: // ---- void, 8 args ..*.*..* (148)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1621: // ---- int, 8 args ..*.*..* (148)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1622: // ---- float, 8 args ..*.*..* (148)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1623: // ---- Object, 8 args ..*.*..* (148)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1624: // ---- void, 8 args *.*.*..* (149)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1625: // ---- int, 8 args *.*.*..* (149)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1626: // ---- float, 8 args *.*.*..* (149)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1627: // ---- Object, 8 args *.*.*..* (149)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1628: // ---- void, 8 args .**.*..* (150)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1629: // ---- int, 8 args .**.*..* (150)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1630: // ---- float, 8 args .**.*..* (150)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1631: // ---- Object, 8 args .**.*..* (150)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1632: // ---- void, 8 args ***.*..* (151)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1633: // ---- int, 8 args ***.*..* (151)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1634: // ---- float, 8 args ***.*..* (151)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1635: // ---- Object, 8 args ***.*..* (151)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1636: // ---- void, 8 args ...**..* (152)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1637: // ---- int, 8 args ...**..* (152)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1638: // ---- float, 8 args ...**..* (152)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1639: // ---- Object, 8 args ...**..* (152)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1640: // ---- void, 8 args *..**..* (153)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1641: // ---- int, 8 args *..**..* (153)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1642: // ---- float, 8 args *..**..* (153)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1643: // ---- Object, 8 args *..**..* (153)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1644: // ---- void, 8 args .*.**..* (154)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1645: // ---- int, 8 args .*.**..* (154)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1646: // ---- float, 8 args .*.**..* (154)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1647: // ---- Object, 8 args .*.**..* (154)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1648: // ---- void, 8 args **.**..* (155)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1649: // ---- int, 8 args **.**..* (155)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1650: // ---- float, 8 args **.**..* (155)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1651: // ---- Object, 8 args **.**..* (155)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1652: // ---- void, 8 args ..***..* (156)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1653: // ---- int, 8 args ..***..* (156)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1654: // ---- float, 8 args ..***..* (156)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1655: // ---- Object, 8 args ..***..* (156)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1656: // ---- void, 8 args *.***..* (157)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1657: // ---- int, 8 args *.***..* (157)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1658: // ---- float, 8 args *.***..* (157)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1659: // ---- Object, 8 args *.***..* (157)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1660: // ---- void, 8 args .****..* (158)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1661: // ---- int, 8 args .****..* (158)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1662: // ---- float, 8 args .****..* (158)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1663: // ---- Object, 8 args .****..* (158)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1664: // ---- void, 8 args *****..* (159)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); break;
   case 1665: // ---- int, 8 args *****..* (159)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1666: // ---- float, 8 args *****..* (159)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1667: // ---- Object, 8 args *****..* (159)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1668: // ---- void, 8 args .....*.* (160)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1669: // ---- int, 8 args .....*.* (160)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1670: // ---- float, 8 args .....*.* (160)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1671: // ---- Object, 8 args .....*.* (160)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1672: // ---- void, 8 args *....*.* (161)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1673: // ---- int, 8 args *....*.* (161)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1674: // ---- float, 8 args *....*.* (161)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1675: // ---- Object, 8 args *....*.* (161)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1676: // ---- void, 8 args .*...*.* (162)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1677: // ---- int, 8 args .*...*.* (162)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1678: // ---- float, 8 args .*...*.* (162)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1679: // ---- Object, 8 args .*...*.* (162)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1680: // ---- void, 8 args **...*.* (163)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1681: // ---- int, 8 args **...*.* (163)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1682: // ---- float, 8 args **...*.* (163)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1683: // ---- Object, 8 args **...*.* (163)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1684: // ---- void, 8 args ..*..*.* (164)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1685: // ---- int, 8 args ..*..*.* (164)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1686: // ---- float, 8 args ..*..*.* (164)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1687: // ---- Object, 8 args ..*..*.* (164)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1688: // ---- void, 8 args *.*..*.* (165)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1689: // ---- int, 8 args *.*..*.* (165)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1690: // ---- float, 8 args *.*..*.* (165)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1691: // ---- Object, 8 args *.*..*.* (165)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1692: // ---- void, 8 args .**..*.* (166)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1693: // ---- int, 8 args .**..*.* (166)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1694: // ---- float, 8 args .**..*.* (166)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1695: // ---- Object, 8 args .**..*.* (166)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1696: // ---- void, 8 args ***..*.* (167)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1697: // ---- int, 8 args ***..*.* (167)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1698: // ---- float, 8 args ***..*.* (167)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1699: // ---- Object, 8 args ***..*.* (167)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1700: // ---- void, 8 args ...*.*.* (168)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1701: // ---- int, 8 args ...*.*.* (168)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1702: // ---- float, 8 args ...*.*.* (168)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1703: // ---- Object, 8 args ...*.*.* (168)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1704: // ---- void, 8 args *..*.*.* (169)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1705: // ---- int, 8 args *..*.*.* (169)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1706: // ---- float, 8 args *..*.*.* (169)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1707: // ---- Object, 8 args *..*.*.* (169)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1708: // ---- void, 8 args .*.*.*.* (170)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1709: // ---- int, 8 args .*.*.*.* (170)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1710: // ---- float, 8 args .*.*.*.* (170)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1711: // ---- Object, 8 args .*.*.*.* (170)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1712: // ---- void, 8 args **.*.*.* (171)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1713: // ---- int, 8 args **.*.*.* (171)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1714: // ---- float, 8 args **.*.*.* (171)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1715: // ---- Object, 8 args **.*.*.* (171)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1716: // ---- void, 8 args ..**.*.* (172)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1717: // ---- int, 8 args ..**.*.* (172)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1718: // ---- float, 8 args ..**.*.* (172)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1719: // ---- Object, 8 args ..**.*.* (172)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1720: // ---- void, 8 args *.**.*.* (173)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1721: // ---- int, 8 args *.**.*.* (173)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1722: // ---- float, 8 args *.**.*.* (173)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1723: // ---- Object, 8 args *.**.*.* (173)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1724: // ---- void, 8 args .***.*.* (174)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1725: // ---- int, 8 args .***.*.* (174)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1726: // ---- float, 8 args .***.*.* (174)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1727: // ---- Object, 8 args .***.*.* (174)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1728: // ---- void, 8 args ****.*.* (175)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1729: // ---- int, 8 args ****.*.* (175)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1730: // ---- float, 8 args ****.*.* (175)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1731: // ---- Object, 8 args ****.*.* (175)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1732: // ---- void, 8 args ....**.* (176)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1733: // ---- int, 8 args ....**.* (176)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1734: // ---- float, 8 args ....**.* (176)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1735: // ---- Object, 8 args ....**.* (176)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1736: // ---- void, 8 args *...**.* (177)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1737: // ---- int, 8 args *...**.* (177)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1738: // ---- float, 8 args *...**.* (177)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1739: // ---- Object, 8 args *...**.* (177)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1740: // ---- void, 8 args .*..**.* (178)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1741: // ---- int, 8 args .*..**.* (178)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1742: // ---- float, 8 args .*..**.* (178)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1743: // ---- Object, 8 args .*..**.* (178)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1744: // ---- void, 8 args **..**.* (179)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1745: // ---- int, 8 args **..**.* (179)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1746: // ---- float, 8 args **..**.* (179)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1747: // ---- Object, 8 args **..**.* (179)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1748: // ---- void, 8 args ..*.**.* (180)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1749: // ---- int, 8 args ..*.**.* (180)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1750: // ---- float, 8 args ..*.**.* (180)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1751: // ---- Object, 8 args ..*.**.* (180)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1752: // ---- void, 8 args *.*.**.* (181)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1753: // ---- int, 8 args *.*.**.* (181)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1754: // ---- float, 8 args *.*.**.* (181)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1755: // ---- Object, 8 args *.*.**.* (181)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1756: // ---- void, 8 args .**.**.* (182)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1757: // ---- int, 8 args .**.**.* (182)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1758: // ---- float, 8 args .**.**.* (182)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1759: // ---- Object, 8 args .**.**.* (182)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1760: // ---- void, 8 args ***.**.* (183)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1761: // ---- int, 8 args ***.**.* (183)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1762: // ---- float, 8 args ***.**.* (183)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1763: // ---- Object, 8 args ***.**.* (183)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1764: // ---- void, 8 args ...***.* (184)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1765: // ---- int, 8 args ...***.* (184)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1766: // ---- float, 8 args ...***.* (184)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1767: // ---- Object, 8 args ...***.* (184)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1768: // ---- void, 8 args *..***.* (185)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1769: // ---- int, 8 args *..***.* (185)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1770: // ---- float, 8 args *..***.* (185)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1771: // ---- Object, 8 args *..***.* (185)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1772: // ---- void, 8 args .*.***.* (186)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1773: // ---- int, 8 args .*.***.* (186)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1774: // ---- float, 8 args .*.***.* (186)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1775: // ---- Object, 8 args .*.***.* (186)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1776: // ---- void, 8 args **.***.* (187)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1777: // ---- int, 8 args **.***.* (187)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1778: // ---- float, 8 args **.***.* (187)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1779: // ---- Object, 8 args **.***.* (187)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1780: // ---- void, 8 args ..****.* (188)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1781: // ---- int, 8 args ..****.* (188)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1782: // ---- float, 8 args ..****.* (188)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1783: // ---- Object, 8 args ..****.* (188)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1784: // ---- void, 8 args *.****.* (189)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1785: // ---- int, 8 args *.****.* (189)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1786: // ---- float, 8 args *.****.* (189)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1787: // ---- Object, 8 args *.****.* (189)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1788: // ---- void, 8 args .*****.* (190)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1789: // ---- int, 8 args .*****.* (190)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1790: // ---- float, 8 args .*****.* (190)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1791: // ---- Object, 8 args .*****.* (190)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1792: // ---- void, 8 args ******.* (191)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); break;
   case 1793: // ---- int, 8 args ******.* (191)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=1; break;
   case 1794: // ---- float, 8 args ******.* (191)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=2; break;
   case 1795: // ---- Object, 8 args ******.* (191)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, void*, sUI, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].ui, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1796: // ---- void, 8 args ......** (192)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1797: // ---- int, 8 args ......** (192)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1798: // ---- float, 8 args ......** (192)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1799: // ---- Object, 8 args ......** (192)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1800: // ---- void, 8 args *.....** (193)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1801: // ---- int, 8 args *.....** (193)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1802: // ---- float, 8 args *.....** (193)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1803: // ---- Object, 8 args *.....** (193)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1804: // ---- void, 8 args .*....** (194)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1805: // ---- int, 8 args .*....** (194)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1806: // ---- float, 8 args .*....** (194)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1807: // ---- Object, 8 args .*....** (194)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1808: // ---- void, 8 args **....** (195)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1809: // ---- int, 8 args **....** (195)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1810: // ---- float, 8 args **....** (195)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1811: // ---- Object, 8 args **....** (195)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1812: // ---- void, 8 args ..*...** (196)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1813: // ---- int, 8 args ..*...** (196)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1814: // ---- float, 8 args ..*...** (196)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1815: // ---- Object, 8 args ..*...** (196)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1816: // ---- void, 8 args *.*...** (197)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1817: // ---- int, 8 args *.*...** (197)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1818: // ---- float, 8 args *.*...** (197)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1819: // ---- Object, 8 args *.*...** (197)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1820: // ---- void, 8 args .**...** (198)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1821: // ---- int, 8 args .**...** (198)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1822: // ---- float, 8 args .**...** (198)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1823: // ---- Object, 8 args .**...** (198)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1824: // ---- void, 8 args ***...** (199)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1825: // ---- int, 8 args ***...** (199)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1826: // ---- float, 8 args ***...** (199)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1827: // ---- Object, 8 args ***...** (199)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1828: // ---- void, 8 args ...*..** (200)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1829: // ---- int, 8 args ...*..** (200)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1830: // ---- float, 8 args ...*..** (200)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1831: // ---- Object, 8 args ...*..** (200)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1832: // ---- void, 8 args *..*..** (201)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1833: // ---- int, 8 args *..*..** (201)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1834: // ---- float, 8 args *..*..** (201)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1835: // ---- Object, 8 args *..*..** (201)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1836: // ---- void, 8 args .*.*..** (202)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1837: // ---- int, 8 args .*.*..** (202)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1838: // ---- float, 8 args .*.*..** (202)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1839: // ---- Object, 8 args .*.*..** (202)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1840: // ---- void, 8 args **.*..** (203)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1841: // ---- int, 8 args **.*..** (203)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1842: // ---- float, 8 args **.*..** (203)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1843: // ---- Object, 8 args **.*..** (203)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1844: // ---- void, 8 args ..**..** (204)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1845: // ---- int, 8 args ..**..** (204)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1846: // ---- float, 8 args ..**..** (204)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1847: // ---- Object, 8 args ..**..** (204)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1848: // ---- void, 8 args *.**..** (205)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1849: // ---- int, 8 args *.**..** (205)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1850: // ---- float, 8 args *.**..** (205)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1851: // ---- Object, 8 args *.**..** (205)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1852: // ---- void, 8 args .***..** (206)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1853: // ---- int, 8 args .***..** (206)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1854: // ---- float, 8 args .***..** (206)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1855: // ---- Object, 8 args .***..** (206)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1856: // ---- void, 8 args ****..** (207)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1857: // ---- int, 8 args ****..** (207)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1858: // ---- float, 8 args ****..** (207)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1859: // ---- Object, 8 args ****..** (207)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1860: // ---- void, 8 args ....*.** (208)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1861: // ---- int, 8 args ....*.** (208)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1862: // ---- float, 8 args ....*.** (208)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1863: // ---- Object, 8 args ....*.** (208)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1864: // ---- void, 8 args *...*.** (209)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1865: // ---- int, 8 args *...*.** (209)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1866: // ---- float, 8 args *...*.** (209)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1867: // ---- Object, 8 args *...*.** (209)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1868: // ---- void, 8 args .*..*.** (210)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1869: // ---- int, 8 args .*..*.** (210)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1870: // ---- float, 8 args .*..*.** (210)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1871: // ---- Object, 8 args .*..*.** (210)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1872: // ---- void, 8 args **..*.** (211)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1873: // ---- int, 8 args **..*.** (211)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1874: // ---- float, 8 args **..*.** (211)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1875: // ---- Object, 8 args **..*.** (211)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1876: // ---- void, 8 args ..*.*.** (212)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1877: // ---- int, 8 args ..*.*.** (212)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1878: // ---- float, 8 args ..*.*.** (212)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1879: // ---- Object, 8 args ..*.*.** (212)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1880: // ---- void, 8 args *.*.*.** (213)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1881: // ---- int, 8 args *.*.*.** (213)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1882: // ---- float, 8 args *.*.*.** (213)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1883: // ---- Object, 8 args *.*.*.** (213)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1884: // ---- void, 8 args .**.*.** (214)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1885: // ---- int, 8 args .**.*.** (214)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1886: // ---- float, 8 args .**.*.** (214)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1887: // ---- Object, 8 args .**.*.** (214)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1888: // ---- void, 8 args ***.*.** (215)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1889: // ---- int, 8 args ***.*.** (215)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1890: // ---- float, 8 args ***.*.** (215)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1891: // ---- Object, 8 args ***.*.** (215)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1892: // ---- void, 8 args ...**.** (216)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1893: // ---- int, 8 args ...**.** (216)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1894: // ---- float, 8 args ...**.** (216)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1895: // ---- Object, 8 args ...**.** (216)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1896: // ---- void, 8 args *..**.** (217)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1897: // ---- int, 8 args *..**.** (217)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1898: // ---- float, 8 args *..**.** (217)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1899: // ---- Object, 8 args *..**.** (217)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1900: // ---- void, 8 args .*.**.** (218)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1901: // ---- int, 8 args .*.**.** (218)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1902: // ---- float, 8 args .*.**.** (218)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1903: // ---- Object, 8 args .*.**.** (218)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1904: // ---- void, 8 args **.**.** (219)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1905: // ---- int, 8 args **.**.** (219)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1906: // ---- float, 8 args **.**.** (219)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1907: // ---- Object, 8 args **.**.** (219)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1908: // ---- void, 8 args ..***.** (220)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1909: // ---- int, 8 args ..***.** (220)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1910: // ---- float, 8 args ..***.** (220)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1911: // ---- Object, 8 args ..***.** (220)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1912: // ---- void, 8 args *.***.** (221)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1913: // ---- int, 8 args *.***.** (221)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1914: // ---- float, 8 args *.***.** (221)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1915: // ---- Object, 8 args *.***.** (221)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1916: // ---- void, 8 args .****.** (222)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1917: // ---- int, 8 args .****.** (222)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1918: // ---- float, 8 args .****.** (222)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1919: // ---- Object, 8 args .****.** (222)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1920: // ---- void, 8 args *****.** (223)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); break;
   case 1921: // ---- int, 8 args *****.** (223)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1922: // ---- float, 8 args *****.** (223)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1923: // ---- Object, 8 args *****.** (223)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, sUI, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].ui, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1924: // ---- void, 8 args .....*** (224)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1925: // ---- int, 8 args .....*** (224)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1926: // ---- float, 8 args .....*** (224)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1927: // ---- Object, 8 args .....*** (224)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1928: // ---- void, 8 args *....*** (225)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1929: // ---- int, 8 args *....*** (225)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1930: // ---- float, 8 args *....*** (225)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1931: // ---- Object, 8 args *....*** (225)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1932: // ---- void, 8 args .*...*** (226)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1933: // ---- int, 8 args .*...*** (226)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1934: // ---- float, 8 args .*...*** (226)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1935: // ---- Object, 8 args .*...*** (226)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1936: // ---- void, 8 args **...*** (227)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1937: // ---- int, 8 args **...*** (227)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1938: // ---- float, 8 args **...*** (227)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1939: // ---- Object, 8 args **...*** (227)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1940: // ---- void, 8 args ..*..*** (228)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1941: // ---- int, 8 args ..*..*** (228)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1942: // ---- float, 8 args ..*..*** (228)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1943: // ---- Object, 8 args ..*..*** (228)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1944: // ---- void, 8 args *.*..*** (229)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1945: // ---- int, 8 args *.*..*** (229)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1946: // ---- float, 8 args *.*..*** (229)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1947: // ---- Object, 8 args *.*..*** (229)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1948: // ---- void, 8 args .**..*** (230)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1949: // ---- int, 8 args .**..*** (230)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1950: // ---- float, 8 args .**..*** (230)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1951: // ---- Object, 8 args .**..*** (230)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1952: // ---- void, 8 args ***..*** (231)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1953: // ---- int, 8 args ***..*** (231)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1954: // ---- float, 8 args ***..*** (231)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1955: // ---- Object, 8 args ***..*** (231)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1956: // ---- void, 8 args ...*.*** (232)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1957: // ---- int, 8 args ...*.*** (232)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1958: // ---- float, 8 args ...*.*** (232)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1959: // ---- Object, 8 args ...*.*** (232)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1960: // ---- void, 8 args *..*.*** (233)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1961: // ---- int, 8 args *..*.*** (233)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1962: // ---- float, 8 args *..*.*** (233)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1963: // ---- Object, 8 args *..*.*** (233)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1964: // ---- void, 8 args .*.*.*** (234)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1965: // ---- int, 8 args .*.*.*** (234)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1966: // ---- float, 8 args .*.*.*** (234)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1967: // ---- Object, 8 args .*.*.*** (234)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1968: // ---- void, 8 args **.*.*** (235)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1969: // ---- int, 8 args **.*.*** (235)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1970: // ---- float, 8 args **.*.*** (235)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1971: // ---- Object, 8 args **.*.*** (235)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1972: // ---- void, 8 args ..**.*** (236)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1973: // ---- int, 8 args ..**.*** (236)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1974: // ---- float, 8 args ..**.*** (236)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1975: // ---- Object, 8 args ..**.*** (236)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1976: // ---- void, 8 args *.**.*** (237)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1977: // ---- int, 8 args *.**.*** (237)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1978: // ---- float, 8 args *.**.*** (237)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1979: // ---- Object, 8 args *.**.*** (237)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1980: // ---- void, 8 args .***.*** (238)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1981: // ---- int, 8 args .***.*** (238)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1982: // ---- float, 8 args .***.*** (238)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1983: // ---- Object, 8 args .***.*** (238)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1984: // ---- void, 8 args ****.*** (239)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1985: // ---- int, 8 args ****.*** (239)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1986: // ---- float, 8 args ****.*** (239)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1987: // ---- Object, 8 args ****.*** (239)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, sUI, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].ui, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1988: // ---- void, 8 args ....**** (240)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1989: // ---- int, 8 args ....**** (240)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1990: // ---- float, 8 args ....**** (240)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1991: // ---- Object, 8 args ....**** (240)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1992: // ---- void, 8 args *...**** (241)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1993: // ---- int, 8 args *...**** (241)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1994: // ---- float, 8 args *...**** (241)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1995: // ---- Object, 8 args *...**** (241)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 1996: // ---- void, 8 args .*..**** (242)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 1997: // ---- int, 8 args .*..**** (242)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 1998: // ---- float, 8 args .*..**** (242)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 1999: // ---- Object, 8 args .*..**** (242)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2000: // ---- void, 8 args **..**** (243)
                                 ((void(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2001: // ---- int, 8 args **..**** (243)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2002: // ---- float, 8 args **..**** (243)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2003: // ---- Object, 8 args **..**** (243)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2004: // ---- void, 8 args ..*.**** (244)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2005: // ---- int, 8 args ..*.**** (244)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2006: // ---- float, 8 args ..*.**** (244)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2007: // ---- Object, 8 args ..*.**** (244)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2008: // ---- void, 8 args *.*.**** (245)
                                 ((void(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2009: // ---- int, 8 args *.*.**** (245)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2010: // ---- float, 8 args *.*.**** (245)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2011: // ---- Object, 8 args *.*.**** (245)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2012: // ---- void, 8 args .**.**** (246)
                                 ((void(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2013: // ---- int, 8 args .**.**** (246)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2014: // ---- float, 8 args .**.**** (246)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2015: // ---- Object, 8 args .**.**** (246)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2016: // ---- void, 8 args ***.**** (247)
                                 ((void(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2017: // ---- int, 8 args ***.**** (247)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2018: // ---- float, 8 args ***.**** (247)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2019: // ---- Object, 8 args ***.**** (247)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, sUI, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].ui, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2020: // ---- void, 8 args ...***** (248)
                                 ((void(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2021: // ---- int, 8 args ...***** (248)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2022: // ---- float, 8 args ...***** (248)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2023: // ---- Object, 8 args ...***** (248)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2024: // ---- void, 8 args *..***** (249)
                                 ((void(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2025: // ---- int, 8 args *..***** (249)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2026: // ---- float, 8 args *..***** (249)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2027: // ---- Object, 8 args *..***** (249)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2028: // ---- void, 8 args .*.***** (250)
                                 ((void(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2029: // ---- int, 8 args .*.***** (250)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2030: // ---- float, 8 args .*.***** (250)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2031: // ---- Object, 8 args .*.***** (250)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2032: // ---- void, 8 args **.***** (251)
                                 ((void(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2033: // ---- int, 8 args **.***** (251)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2034: // ---- float, 8 args **.***** (251)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2035: // ---- Object, 8 args **.***** (251)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, sUI, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].ui, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2036: // ---- void, 8 args ..****** (252)
                                 ((void(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2037: // ---- int, 8 args ..****** (252)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2038: // ---- float, 8 args ..****** (252)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2039: // ---- Object, 8 args ..****** (252)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, sUI, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2040: // ---- void, 8 args *.****** (253)
                                 ((void(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2041: // ---- int, 8 args *.****** (253)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2042: // ---- float, 8 args *.****** (253)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2043: // ---- Object, 8 args *.****** (253)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, sUI, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].ui, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2044: // ---- void, 8 args .******* (254)
                                 ((void(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2045: // ---- int, 8 args .******* (254)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2046: // ---- float, 8 args .******* (254)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2047: // ---- Object, 8 args .******* (254)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(sUI, void*, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].ui, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
   case 2048: // ---- void, 8 args ******** (255)
                                 ((void(YAC_CALL*)(void*, void*, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); break;
   case 2049: // ---- int, 8 args ******** (255)
    _env->cret.value.int_val                  =   ((sSI(YAC_CALL*)(void*, void*, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=1; break;
   case 2050: // ---- float, 8 args ******** (255)
    _env->cret.value.float_val                 = ((sF32(YAC_CALL*)(void*, void*, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=2; break;
   case 2051: // ---- Object, 8 args ******** (255)
    _env->cret.value.object_val = (YAC_Object*) ((void*(YAC_CALL*)(void*, void*, void*, void*, void*, void*, void*, void*))command->y_adr)(call_values[0].any, call_values[1].any, call_values[2].any, call_values[3].any, call_values[4].any, call_values[5].any, call_values[6].any, call_values[7].any); _env->cret.type=3; _env->cret.deleteme=0; _env->cret.fixStringType(); break;
