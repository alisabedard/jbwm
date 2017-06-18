(define copyright "// Copyright 2017, Jeffrey E. Bedard\n")
; for MIT Scheme
(load "libconvert.scm")
; Store the prefix as the first element in these lists:
(define net '("" "SUPPORTED" "CURRENT_DESKTOP" "NUMBER_OF_DESKTOPS"
	"DESKTOP_VIEWPORT" "DESKTOP_GEOMETRY" "SUPPORTING_WM_CHECK"
	"ACTIVE_WINDOW" "MOVERESIZE_WINDOW" "CLOSE_WINDOW" "CLIENT_LIST"
	"VIRTUAL_ROOTS" "CLIENT_LIST_STACKING" "FRAME_EXTENTS"))
(define wm '("WM_" "ALLOWED_ACTIONS" "NAME" "DESKTOP" "MOVERESIZE" "PID"
	"WINDOW_TYPE" "STATE"))
(define wm-action '("WM_ACTION_" "MOVE" "RESIZE" "CLOSE" "SHADE"
	"FULLSCREEN" "CHANGE_DESKTOP" "ABOVE" "BELOW" "MAXIMIZE_HORZ"
	"MAXIMIZE_VERT"))
(define wm-state '("WM_STATE_" "STICKY" "MAXIMIZED_VERT" "MAXIMIZED_HORZ"
	"SHADED" "HIDDEN" "FULLSCREEN" "ABOVE" "BELOW" "FOCUSED"))
(define print-all (lambda (function)
	(map (lambda (cat) (print-each function cat))
		(list net wm wm-action wm-state))))
; Generate "ewmh_atoms.c"
(define write-ewmh-atoms-c (lambda ()
	(let ((outf (open-output-file "ewmh_atoms.c")))
	(set-current-output-port! outf)
	(begin-array-definition "jbwm_atom_names")
	(print-all print-each-array-element)
	(end-c-definition)
	(close-port outf))))
; Generate "JBWMAtomIndex.h"
(define write-jbwmatomindex-h (lambda ()
	(let ((outf (open-output-file "JBWMAtomIndex.h"))
		(ig "JBWMATOMINDEX"))
	(set-current-output-port! outf)
	(write-include-header ig)
	(begin-enum-definition "JBWMAtomIndex")
	(set! master-prefix "JBWM_EWMH_")
	(print-all print-each-enum)
	(display "\t// The following entry must be last:\n")
	(print-enum-line "" "ATOMS_COUNT")
	(end-c-definition)
	(write-include-fin ig)
	(close-port outf))))
; Execution:
(write-ewmh-atoms-c)
(write-jbwmatomindex-h)
; Restore normal output:
(set-current-output-port! console-i/o-port)
