; for MIT Scheme
(define copyright "// Copyright 2017, Jeffrey E. Bedard\n")
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
; The following prefix is applied to each entry
(define master-prefix "_NET_")
(define begin-array-definition (lambda (name) (begin
	(display (string-append copyright
		"static char * " name " [] = {\n"))
	0)))
(define begin-enum-definition (lambda (name)
	(display (string-append "enum " name "{\n"))))
(define end-c-definition (lambda () (display "};\n")))
(define endl (lambda () (display "\n") 0))
(define get-array-line (lambda (prefix item)
	(string-append "\t\"" master-prefix prefix item "\",\n")))
(define print-each-array (lambda (prefix l)
	(if (null? l)
		(endl)
		(begin
			(display (get-array-line prefix (car l)))
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
(define print-all-listed (lambda (function cat) (begin
	(if (null? cat)
		0
		(begin
			(print-each function (car cat))
			(print-all-listed function (cdr cat)))))))
(define print-all (lambda (function)
	(print-all-listed function (list net wm wm-action wm-state))))
; Generate "ewmh_atoms.c"
(define write-ewmh-atoms-c (lambda ()
	(let ((outf (open-output-file "ewmh_atoms.c")))
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
	(display (string-append copyright
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
