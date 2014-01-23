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
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with AntiRight; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef LIBAR_MACROS_H
#define LIBAR_MACROS_H

#define ARBUGPARAM(message) \
	"%s:%d: %s", __FILE__, __LINE__, message

#define ARBUG(message) \
	g_debug(ARBUGPARAM(message))

#define ARWARN(message) \
	g_warning(ARBUGPARAM(message))

#define ARERR(message) \
	g_error(ARBUGPARAM(message))

#define ARBBOOL(x) (x == TRUE)
#define ARPBOOL(x) (x != NULL)

#ifdef DEBUG
#define ARASSERT g_assert
#else /* not DEBUG */
#define ARASSERT	/* undefined */
#endif /* DEBUG */

#ifdef DEBUG
#define ARBASSERT(x) \
	ARASSERT(ARBBOOL(x))

#define ARPASSERT(x) \
	ARASSERT(ARPBOOL(x))
#else /* not DEBUG */
#define ARBASSERT(x)	/* undefined */
#define ARPASSERT(x)	/* undefined */
#endif /* DEBUG */

#define ARIFB(x) \
	if(ARBBOOL(x))

#define ARIFNB(x) \
	if(!ARBBOOL(x))

#define ARIFP(x) \
	if(ARPBOOL(x))

#define ARIFNP(x) \
	if(!ARPBOOL(x))

#define ARWIFNP(x) \
	ARIFNP(x) \
		ARWARN("NULL data passed, operation not performed"); \
	else

#endif /* not LIBAR_MACROS_H */
