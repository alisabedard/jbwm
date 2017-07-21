; libconvert.scm
; Simple text database parsing library for C header generation
; vim: sw=2
(define copyright "// Copyright 2017, Jeffrey E. Bedard\n")

; Field separator character storage and accessor
(define __libconvert-field-separator #\:)
(define (set-field-separator c) (set! __libconvert-field-separator c))

; Indentation level storage and accessors
(define __libconvert-indentation 0)
(define (set-indent-level x) (set! __libconvert-indentation x))
(define get-indent
 (lambda () (make-string __libconvert-indentation #\tab)))

; Flatten per https://rosettacode.org/wiki/Flatten_a_list#Scheme
(define flatten
 (lambda (x)
  (cond ((null? x) '())
   ((not (pair? x)) (list x))
   (else (append (flatten (car x))
	  (flatten (cdr x)))))))

; lisp-like operations on character-separated strings:
(define get-string-divider
 (lambda (str)
  (let ((i (string-find-next-char
	    str __libconvert-field-separator)))
   (if i
    (let ((esc (- i 1))
	  (len (string-length str)))
     ; This allows escaping the field separator with \
     (if (and (>= esc 0) (char=? (string-ref str esc) #\\))
      (begin (substring-move-left! str i len str esc)
       ; truncate the hole left at the end with space
       (string-set! str (- len 1) #\space)
       ; return next instance of filed separator
       (get-string-divider (string-head str esc)))
      i)) #f)))) 

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

; returns a list of strings split by the field separator
(define string-list
 (lambda (line)
  (let ((i (get-string-divider line)))
   (if i (cons 
	  (string-head line i)
	  (string-list (string-tail line (+ 1 i))))
    (cons line '())))))

(define string-list-fixed
 (lambda (line size)
  (let ((out_v (make-vector size "")) (in_l (string-list line)) (i 0))
   (let ((in_v (list->vector in_l)))
    (map (lambda (item)
	  (if (< i size) (begin
			  (vector-set! out_v i (vector-ref in_v i))
			  (set! i (+ 1 i))))) in_l))
   (vector->list out_v))))

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

(define c-add-include
 (lambda (file out-port) (display (string-append "#include "
				   file "\n") out-port)))

(define begin-include
 (lambda (guard_tag out) (display (string-append copyright "#ifndef "
 guard_tag "\n#define " guard_tag "\n") out)))

(define end-include
 (lambda (guard_tag out)
  (display (string-append "#endif//!" guard_tag "\n") out)))
