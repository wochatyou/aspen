// Scintilla source code edit control
/** @file Position.h
 ** Defines global type name Position in the Sci internal namespace.
 **/
// Copyright 2015 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef POSITION_H
#define POSITION_H

/**
 * A Position is a position within a document between two characters or at the beginning or end.
 * Sometimes used as a character index where it identifies the character after the position.
 * A Line is a document or screen line.
 */

namespace Sci {

typedef ptrdiff_t Position; //X- std::ptrdiff_t is the signed integer type of the result of subtracting two pointers. 这是一个有符号的整型
typedef ptrdiff_t Line;

inline constexpr Position invalidPosition = -1; //X- -1表示无效的位置

}

#endif
