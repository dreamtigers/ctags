/*
*   Copyright (c) 2016, Masatake YAMATO <yamato@redhat.com>
*   Copyright (c) 2016, Red Hat, Inc.
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License version 2 or (at your option) any later version.
*
*   This module contains functions for generating tags for Python language
*   files.
*/

#include "general.h"  /* must always come first */
#include "mio.h"
#include "objpool.h"
#include "vstring.h"

#ifndef CTAGS_MAIN_TOKEN_H
#define CTAGS_MAIN_TOKEN_H

struct tokenClass;
struct tokenTypePair;

typedef short tokenType;
typedef short tokenKeyword;

#define tokenInfoMembers					\
	tokenType		type;					\
	tokenKeyword	keyword;				\
	vString *	string;							\
	struct tokenInfoClass *klass;				\
	unsigned long	lineNumber;					\
	MIOPos		filePosition

typedef struct sTokenInfo {
	tokenInfoMembers;
} tokenInfo;

struct tokenTypePair {
	tokenType start;
	tokenType end;
};

#define TOKEN(X)  ((tokenInfo *)X)
#define TOKEN_X(X,T)  ((T *)(((char *)TOKEN(X)) + sizeof (tokenInfo)))

struct tokenInfoClass {
	unsigned int nPreAlloc;
	tokenType typeForUndefined;
	tokenKeyword keywordNone;
	tokenType typeForKeyword;
	tokenType typeForEOF;
	size_t extraSpace;
	struct tokenTypePair   *pairs;
	unsigned int        pairCount;
	void (*read)   (tokenInfo *token, void *data);
	void (*clear)  (tokenInfo *token);
	void (*destroy) (tokenInfo *token);
	void (*copy)   (tokenInfo *dest, tokenInfo *src, void *data);
	objPool *pool;
	ptrArray *backlog;
};

void *newToken       (struct tokenInfoClass *klass);
void  flashTokenBacklog (struct tokenInfoClass *klass);
void  tokenDestroy    (tokenInfo *token);

void tokenReadFull   (tokenInfo *token, void *data);
void tokenRead       (tokenInfo *token);
void tokenUnreadFull (tokenInfo *token, void *data); /* DATA passed to copy method internally. */
void tokenUnread     (tokenInfo *token);


void tokenCopyFull   (tokenInfo *dest, tokenInfo *src, void *data);
void tokenCopy       (tokenInfo *dest, tokenInfo *src);

/* Helper macro & functions */

#define TOKEN_IS_TYPE(TKN,T)     ((TKN)->type == TOKEN_##T)
#define TOKEN_IS_KEYWORD(TKN,K)  ((TKN)->type == TKN->klass->typeForKeyword \
									&& (TKN)->keyword == KEYWORD_##K)
#define TOKEN_IS_EOF(TKN)      ((TKN)->type == (TKN)->klass->typeForEOF)

#define TOKEN_STRING(TKN)	   (vStringValue ((TKN)->string))
#define TOKEN_PUTC(TKN,C)      (vStringPut ((TKN)->string, C))

bool tokenIsType          (tokenInfo *token, tokenType t);
bool tokenIsKeyword       (tokenInfo *token, tokenKeyword k);

const char* const tokenString (tokenInfo *token);

/* return true if t is found. In that case token holds an
   language object type t.
   return false if it reaches EOF. */
bool tokenSkipToType (tokenInfo *token, tokenType t);
bool tokenSkipOverPair (tokenInfo *token);

#endif