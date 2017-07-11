; Copyright 2017, Jeffrey E. Bedard
; vim: sw=2
(load "libconvert.scm")
(define convert_keys
 (lambda (in_filename out_filename)
  (letrec
   ((i (open-input-file in_filename))
    (o (open-output-file out_filename))
    (guard "JBWM_JBWMKEYS")
    (parse
     (lambda (i o)
      (and-let*
       ((line (read-line i))
	((not (eof-object? line)))
	((> (string-length line) 1)))
       (let ((key (string-car line)) (value (string-cdr line)))
	(display (string-append "\tJBWM_KEY_" key " = XK_" value ",\n") o)
	(parse i o))))))
   (begin-include guard o)
   (display-c-include "<X11/keysym.h>" o)
   (display "enum JBWMKeys {\n" o)
   (parse i o)
   (display "};\n" o)
   (display-c-include "\"key_combos.h\"" o)
   (end-include guard o)
   (close-port i)
   (close-port o))))
(convert_keys "keys.txt" "JBWMKeys.h")
