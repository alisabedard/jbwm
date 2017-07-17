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
#ifdef DEBUG
#define JB_LOG_ADD(cls, v) JBCERR << "NEW " #cls "--> " << ++v << '\n';
#define JB_LOG_DEL(cls, v) JBCERR << "DEL " #cls "--> " << --v << '\n';
#define JB_LOG_OP(cls, msg) JBCERR << "@"#cls << ' ' << msg << '\n' ;
#else//!DEBUG
#define JB_LOG_ADD(c, d)
#define JB_LOG_DEL(c, d)
#define JB_LOG_OP(c, d)
#endif//DEBUG
#endif//!LIBJB_CPP_H
#endif//__cplusplus
