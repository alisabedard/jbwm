; for MIT Scheme
(define copyright "Copyright 2017, Jeffrey E. Bedard")
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
(define get-copyright-line (lambda ()
	(string-append "// " copyright "\n")))
(define begin-array-definition (lambda (name) (begin
	(display (string-append (get-copyright-line)
		"static char * " name " [] = {\n"))
	0)))
(define begin-enum-definition (lambda (name)
	(display (string-append "enum " name "{\n"))))
(define end-c-definition (lambda () (display "};\n")))
(define master-prefix "_NET_")
(define endl (lambda () (display "\n") 0))
(define print-each-array (lambda (prefix l)
	(if (null? l)
		(endl)
		(begin
			(display (string-append "\t\"" master-prefix prefix
				(car l) "\",\n"))
			(print-each-array prefix (cdr l))))))
(define get-enum-line (lambda (prefix item)
	(string-append "\t" master-prefix prefix item ",\n")))
(define print-enum-line (lambda (prefix item)
	(display (get-enum-line prefix item))))
(define print-each-enum (lambda (prefix l)
	(if (null? l)
		(endl)
		(begin
			(print-enum-line prefix (car l))
			(print-each-enum prefix (cdr l))))))
(define get-guard (lambda (name) 
	(string-append "JBWM_" name "_H\n")))
(define print-each (lambda (function data)
	(function (car data) (cdr data))))
(define print-all (lambda (function) (begin
	(print-each function net)
	(print-each function wm)
	(print-each function wm-action)
	(print-each function wm-state))))
(define redirect-output (lambda (file) (begin
	(define outf (open-output-file file))
	(set-current-output-port! outf))))
; Generate "ewmh_atoms.c"
(define write-ewmh-atoms-c (lambda ()
	(let ((outf (open-output-file "emwh_atoms.c")))
	(set-current-output-port! outf)
	(begin-array-definition "jbwm_atom_names")
	(print-all print-each-array)
	(end-c-definition)
	(close-port outf))))
; Generate "JBWMAtomIndex.h"
(define write-jbwmatomindex-h (lambda ()
	(let ((outf (open-output-file "JBWMAtomIndex.h")))
	(set-current-output-port! outf)
	(define ig "JBWMATOMINDEX")
	(display (string-append
		(get-copyright-line)
		"#ifndef " (get-guard ig)
		"#define " (get-guard ig)))
	(begin-enum-definition "JBWMAtomIndex")
	(set! master-prefix "JBWM_EWMH_")
	(print-all print-each-enum)
	(display "\t// The following entry must be last:\n")
	(print-enum-line "" "ATOMS_COUNT")
	(end-c-definition)
	(display (string-append "#endif//!" (get-guard ig)))
	(close-port outf))))
; Execution:
(write-ewmh-atoms-c)
(write-jbwmatomindex-h)
; Restore normal output:
(set-current-output-port! console-i/o-port)
