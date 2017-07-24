; Copyright 2017, Jeffrey E. Bedard
; ewmh atom name to index enum correlation utility
; vim: sw=2
(load "libjb/libconvert.scm")
(set-indent-level 1)
  ; Store the prefix as the first element in these lists:
  (define ewmh-data ; atom association list broken into prefix categories
   '((net "" "SUPPORTED" "CURRENT_DESKTOP" "NUMBER_OF_DESKTOPS"
   "DESKTOP_VIEWPORT" "DESKTOP_GEOMETRY"
   "SUPPORTING_WM_CHECK" "ACTIVE_WINDOW"
   "MOVERESIZE_WINDOW" "CLOSE_WINDOW" "CLIENT_LIST"
   "VIRTUAL_ROOTS"
   "CLIENT_LIST_STACKING" "FRAME_EXTENTS")
   (wm "WM_" "ALLOWED_ACTIONS" "NAME" "DESKTOP" "MOVERESIZE" "PID"
    "WINDOW_TYPE" "STATE")
   (wm-action "WM_ACTION_" "MOVE" "RESIZE" "CLOSE" "SHADE"
    "FULLSCREEN" "CHANGE_DESKTOP" "ABOVE" "BELOW" "MAXIMIZE_HORZ"
    "MAXIMIZE_VERT")
   (wm-state "WM_STATE_" "STICKY" "MAXIMIZED_VERT" "MAXIMIZED_HORZ"
    "SHADED" "HIDDEN" "FULLSCREEN" "ABOVE" "BELOW" "FOCUSED")))


(define element (lambda (e) (cdr (assq e ewmh-data))))

(define print-all
 (lambda (function out_port)
  (let ((net (element 'net)) (wm (element 'wm))
	(wm-action (element 'wm-action)) (wm-state (element 'wm-state)))
   (map (lambda (cat) (print-each function cat out_port))
    (list net wm wm-action wm-state)))))

; Generate "ewmh_init.c"
(define write-ewmh-init-c
 (lambda ()
  (let ((outf (open-output-file "ewmh_init.c")))
   (display "static void jbwm_ewmh_init(Display * d)\n{\n" outf)
   (begin-array-definition "char *" "jbwm_atom_names" outf)
   (print-all print-each-array-element outf)
   (end-c-definition outf)
   (display "\tXInternAtoms(d, jbwm_atom_names, " outf)
   (display "JBWM_EWMH_ATOMS_COUNT,\n\t\tfalse, jbwm_ewmh);\n}\n" outf)
   (close-port outf))))

; Generate "ewmh_allowed.c"
(define write-jbwm-ewmh-allowed-c
 (lambda ()
  (let ((outf (open-output-file "ewmh_allowed.c"))
	(act (cdr (element 'wm-action))))
   (display "\tAtom a[] = {\n" outf)
   (map (lambda (x)
	 (display (string-append "\t\tjbwm_ewmh[JBWM_EWMH_WM_ACTION_" x
		   "],\n") outf)) act)
   (display "\t};\n" outf)
   (close-port outf))))

; Generate "JBWMAtomIndex.h"
(define write-jbwmatomindex-h
 (lambda ()
  (let ((outf (open-output-file "JBWMAtomIndex.h"))
	(ig "JBWM_JBWMATOMINDEX_H"))
   (begin-include ig outf)
   (begin-enum-definition "JBWMAtomIndex" outf)
   (set! master-prefix "JBWM_EWMH_")
   (print-all print-each-enum outf)
   (display (string-append "\t// The following entry must be last:\n"
	"\tJBWM_EWMH_ATOMS_COUNT\n\t};\n") outf)
   (end-include ig outf)
   (close-port outf))))

; Execution:
(write-ewmh-init-c)
(write-jbwmatomindex-h)
(write-jbwm-ewmh-allowed-c)
