; Copyright 2017, Jeffrey E. Bedard
(load "libconvert.scm")
(define convert_keys
 (lambda (in_filename out_filename)
  (letrec ((i (open-input-file in_filename))
	   (o (open-output-file out_filename))
	   (guard "JBWM_JBWMKEYS_H")
	   (data '())
	   (parse
	    (lambda (i o)
	     (and-let* ((line (read-line i))
			((not (eof-object? line)))
			((> (string-length line) 1)))
	      (let ((key (string-car line))
		    (value (string-cdr line)))
	       (set! data (cons (cons key value) data))
	       (parse i o)))))
	   (format-cell ; function to pass to map
	    (lambda (datum out_port)
		(display (string-append "\tJBWM_KEY_" (car datum)
		 " = XK_" (cdr datum) ",\n") out_port)))
	   (compare-cell ; function to pass to sort
	    (lambda (a b) ; compare the keys:
	     (string<? (car a) (car b)))))
	(begin-include guard o)
	(display (string-append "#include <X11/keysym.h>\n"
		"enum JBWMKeys {\n") o)
	(parse i o)
	(set! data (sort data compare-cell))
	(display data)
	(map (lambda (n) (format-cell n o)) data)
	(display "};\n" o)
	(display "#include \"key_combos.h\"\n" o)
	(end-include guard o)
	(close-port i)
	(close-port o))))
(convert_keys "keys.txt" "JBWMKeys.h")
