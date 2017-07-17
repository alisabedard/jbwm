; libconvert.scm
; Simple colon-separated database parsing library for C header generation
; vim: sw=2
(define copyright "// Copyright 2017, Jeffrey E. Bedard\n")

(define __libconvert-indentation 0)
(define set-indent (lambda (x) (set! __libconvert-indentation x)))
(define get-indent
 (lambda () (make-string __libconvert-indentation #\tab)))

; Flatten per https://rosettacode.org/wiki/Flatten_a_list#Scheme
(define flatten
 (lambda (x)
  (cond ((null? x) '())
   ((not (pair? x)) (list x))
   (else (append (flatten (car x))
	  (flatten (cdr x)))))))

; lisp-like operations on colon-separated strings:
(define get-string-divider
 (lambda (str) (string-find-next-char str #\:)))


; returns original string if not a list
(define string-car
 (lambda (str)
  (let ((i (get-string-divider str)))
   (if i (string-head str i) str))))

; returns "" when list empty
(define string-cdr
 (lambda (str)
  (let ((i (get-string-divider str)))
   (if i (string-tail str (+ 1 i)) ""))))

; The following prefix is applied to each entry
(define master-prefix "_NET_")

(define begin-array-definition
 (lambda (type name out)
  (display (string-append (get-indent) "static " type " " name " [] = {\n")
  out)))

(define begin-enum-definition
 (lambda (name out)
  (display (string-append (get-indent) "enum " name " {\n") out)))

(define end-c-definition (lambda (out) (display
 (string-append (get-indent) "};\n") out)))

(define get-array-line
 (lambda (prefix item)
  (string-append (get-indent) "\t\"" master-prefix prefix item "\",\n")))

(define print-each-array-element
 (lambda (prefix elements out_port)
  (map (lambda (item)
	(display (get-array-line prefix item) out_port)) elements)))

(define get-enum-line
 (lambda (prefix item) (string-append (get-indent) master-prefix
  prefix item ",\n")))

(define print-enum-line
 (lambda (prefix item out_port)
  (display (get-enum-line prefix item) out_port)))

(define print-each-enum
 (lambda (prefix elements out_port)
  (map (lambda (item) (print-enum-line prefix item out_port)) elements)))

(define print-each
 (lambda (function data out_port)
  (function (car data) (cdr data) out_port)))

(define begin-include
 (lambda (guard_tag out) (display (string-append copyright "#ifndef "
 guard_tag "\n#define " guard_tag "\n") out)))

(define end-include
 (lambda (guard_tag out)
  (display (string-append "#endif//!" guard_tag "\n") out)))
