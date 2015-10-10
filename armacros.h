/*
  AntiRight
  (c) 2007 Jeffrey Bedard
  antiright@gmail.com

  This file is part of AntiRight.

  AntiRight is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  AntiRight is distributed in the hope that it will be useful,
  but WITHOUT ANY WJBRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PJBTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with AntiRight; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef LIBJB_MACROS_H
#define LIBJB_MACROS_H

#define JBBUGPARAM(message) \
	"%s:%d: %s", __FILE__, __LINE__, message

#define JBBUG(message) \
	g_debug(JBBUGPARAM(message))

#define JBWARN(message) \
	g_warning(JBBUGPARAM(message))

#define JBERR(message) \
	g_error(JBBUGPARAM(message))

#define JBBBOOL(x) (x == TRUE)
#define JBPBOOL(x) (x != NULL)

#ifdef DEBUG
#define JBASSERT g_assert
#else /* not DEBUG */
#define JBASSERT	/* undefined */
#endif /* DEBUG */

#ifdef DEBUG
#define JBBASSERT(x) \
	JBASSERT(ARBBOOL(x))

#define JBPASSERT(x) \
	JBASSERT(ARPBOOL(x))
#else /* not DEBUG */
#define JBBASSERT(x)	/* undefined */
#define JBPASSERT(x)	/* undefined */
#endif /* DEBUG */

#define JBIFB(x) \
	if(JBBBOOL(x))

#define JBIFNB(x) \
	if(!JBBBOOL(x))

#define JBIFP(x) \
	if(JBPBOOL(x))

#define JBIFNP(x) \
	if(!JBPBOOL(x))

#define JBWIFNP(x) \
	JBIFNP(x) \
		JBWARN("NULL data passed, operation not performed"); \
	else

#endif /* not LIBJB_MACROS_H */
