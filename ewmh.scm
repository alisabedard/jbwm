; Copyright 2017, Jeffrey E. Bedard
; ewmh atom name to index enum correlation utility
; vim: sw=2
(load "libconvert.scm")
; Store the prefix as the first element in these lists:
(letrec
 ((net '("" "SUPPORTED" "CURRENT_DESKTOP" "NUMBER_OF_DESKTOPS"
	 "DESKTOP_VIEWPORT" "DESKTOP_GEOMETRY"
	 "SUPPORTING_WM_CHECK" "ACTIVE_WINDOW"
	 "MOVERESIZE_WINDOW" "CLOSE_WINDOW" "CLIENT_LIST" "VIRTUAL_ROOTS"
	 "CLIENT_LIST_STACKING" "FRAME_EXTENTS"))
  (wm '("WM_" "ALLOWED_ACTIONS" "NAME" "DESKTOP" "MOVERESIZE" "PID"
	"WINDOW_TYPE" "STATE"))
  (wm-action
   '("WM_ACTION_" "MOVE" "RESIZE" "CLOSE" "SHADE"
   "FULLSCREEN" "CHANGE_DESKTOP"
   "ABOVE" "BELOW" "MAXIMIZE_HORZ" "MAXIMIZE_VERT"))
  (wm-state
   '("WM_STATE_" "STICKY" "MAXIMIZED_VERT" "MAXIMIZED_HORZ"
   "SHADED" "HIDDEN"
   "FULLSCREEN" "ABOVE" "BELOW" "FOCUSED"))
  (print-all
   (lambda (function out_port)
    (map (lambda (cat)
	  (print-each function cat out_port))
     (list net wm wm-action wm-state))))
  ; Generate "ewmh_init.c"
  (write-ewmh-init-c
   (lambda ()
    (let ((outf (open-output-file "ewmh_init.c")))
     (set-current-output-port! outf)
     (display "static void jbwm_ewmh_init(Display * d)\n{\n" outf)
     (begin-array-definition "char *" "jbwm_atom_names" outf)
     (print-all print-each-array-element outf)
     (end-c-definition outf)
     (display (string-append "\tXInternAtoms(d, jbwm_atom_names,"
	" JBWM_EWMH_ATOMS_COUNT,\n\t\tfalse, jbwm_ewmh);\n}") outf)
     (close-port outf))))
  ; Generate "ewmh_allowed.c"
  ; Generate "JBWMAtomIndex.h"
  (write-jbwmatomindex-h
   (lambda ()
    (let ((outf (open-output-file "JBWMAtomIndex.h"))
	  (ig "JBWM_JBWMATOMINDEX"))
     (set-current-output-port! outf)
     (begin-include ig outf)
     (begin-enum-definition "JBWMAtomIndex" outf)
     (set! master-prefix "JBWM_EWMH_")
     (print-all print-each-enum outf)
     (display "\t// The following entry must be last:\n")
     (print-enum-line "" "ATOMS_COUNT" outf)
     (end-c-definition outf)
     (end-include ig outf)
     (close-port outf)))))
  ; Execution:
  (write-ewmh-init-c)
  (write-jbwmatomindex-h)
    ; Restore normal output:
  (set-current-output-port! console-i/o-port))
