":";exec guile -s $0 "$@"
; for MIT Scheme
(define copyright "Copyright 2017, Jeffrey E. Bedard")
(define net '("SUPPORTED" "CURRENT_DESKTOP" "NUMBER_OF_DESKTOPS"
	"DESKTOP_VIEWPORT" "DESKTOP_GEOMETRY" "SUPPORTING_WM_CHECK"
	"ACTIVE_WINDOW" "MOVERESIZE_WINDOW" "CLOSE_WINDOW" "CLIENT_LIST"
	"VIRTUAL_ROOTS" "CLIENT_LIST_STACKING" "FRAME_EXTENTS"))
(define wm '("ALLOWED_ACTIONS" "NAME" "DESKTOP" "MOVERESIZE" "PID"
	"WINDOW_TYPE" "STATE"))
(define wm-action '("MOVE" "RESIZE" "CLOSE" "SHADE" "FULLSCREEN"
	"CHANGE_DESKTOP" "ABOVE" "BELOW" "MAXIMIZE_HORZ" "MAXIMIZE_VERT"))
(define wm-state '("STICKY" "MAXIMIZED_VERT" "MAXIMIZED_HORZ" "SHADED"
	"HIDDEN" "FULLSCREEN" "ABOVE" "BELOW" "FOCUSED"))
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
(define print-each (lambda (prefix l)
	(if (null? l)
		(begin (display "\n") 0)
		(begin
			(display (string-append "\t\"" master-prefix
				prefix (car l) "\",\n"))
			(print-each prefix (cdr l))))))
(define get-enum-line (lambda (prefix item)
	(string-append "\t" master-prefix prefix item ",\n")))
(define print-enum-line (lambda (prefix item)
	(display (get-enum-line prefix item))))
(define print-each-enum (lambda (prefix l)
	(if (null? l)
		(begin (display "\n") 0)
		(begin
			(print-enum-line prefix (car l))
			(print-each-enum prefix (cdr l))))))
(define get-guard (lambda (name) 
	(string-append "JBWM_" name "_H\n")))

; The execution begins:

; For atom_names:
(define f (open-output-file "ewmh_atoms.c"))
(set-current-output-port! f)
(begin-array-definition "jbwm_atom_names")
(print-each "" net)
(print-each "WM_" wm)
(print-each "WM_ACTION_" wm-action)
(print-each "WM_STATE_" wm-state)
(end-c-definition)
;(display "};\n")
(close-port f)

; JBWMAtomIndex:
(define f (open-output-file "JBWMAtomIndex.h"))
(set-current-output-port! f)
(define ig "JBWMATOMINDEX")
(display (string-append
	(get-copyright-line)
	"#ifndef " (get-guard ig)
	"#define " (get-guard ig)))
(begin-enum-definition "JBWMAtomIndex")
(set! master-prefix "JBWM_EWMH_")
(print-each-enum "" net)
(print-each-enum "WM_" wm)
(print-each-enum "WM_ACTION_" wm-action)
(print-each-enum "WM_STATE_" wm-state)
(display "\t// The following entry must be last:\n")
(print-enum-line "" "ATOMS_COUNT")
(end-c-definition)
(display (string-append "#endif//!" (get-guard ig)))
(close-port f)
