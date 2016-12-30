// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
// This is a header for C++-specific extensions
#ifdef __cplusplus
#ifndef LIBJB_CPP_H
#define LIBJB_CPP_H
#include <iostream>
#include <string>
#define JBCERR std::cerr << __FILE__ << ':' << __LINE__ << ' '
#define libjb_strlen(s) std::string(s).size()
#define libjb_strlcpy(dst, src, sz) std::string(src).copy(dst, sz)
#endif//!LIBJB_CPP_H
#endif//__cplusplus
