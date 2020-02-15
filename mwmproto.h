// jbwm - Minimalist Window Manager for X
// Copyright 2020, Jeffrey E. Bedard <jefbed@gmail.com>
#ifndef JBWM_MWMPROTO_H
#define JBWM_MWMPROTO_H
// These are MWM-specific hints
enum MwmFlags {
    // flags:
    MWM_HINTS_FUNCTIONS = (1L << 0), MWM_HINTS_DECORATIONS = (1L << 1),
    MWM_HINTS_INPUT_MODE = (1L << 2), MWM_HINTS_STATUS = (1L << 3)
};
enum MwmFunctions {
    // functions:
    MWM_FUNC_ALL = (1L << 0), MWM_FUNC_RESIZE = (1L << 1),
    MWM_FUNC_MOVE = (1L << 2), MWM_FUNC_MINIMIZE = (1L << 3),
    MWM_FUNC_MAXIMIZE = (1L << 4), MWM_FUNC_CLOSE = (1L << 5),
};
enum MwmDecor {
    // decor:
    MWM_DECOR_ALL = (1L << 0), MWM_DECOR_BORDER = (1L << 1),
    MWM_DECOR_RESIZEH = (1L << 2), MWM_DECOR_TITLE = (1L << 3),
    MWM_DECOR_MENU = (1L << 4), MWM_DECOR_MINIMIZE = (1L << 5),
    MWM_DECOR_MAXIMIZE = (1L << 6),
};
enum MwmStatus {
    // status:
    MWM_TEAROFF_WINDOW = 1
};
struct JBWMMwm { // paraphrased from MwmUtil.h
    unsigned long flags, functions, decor, input_mode, status;
};
#endif//!JBWM_MWMPROTO_H
