#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"
#include <string.h>
#include <ctype.h>
extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_SPACE))
  {
    readChar();
  }
}

void skipComment() {
  int state = 0;
  while (currentChar != EOF)
  {
    switch (charCodes[currentChar])
    {
    case CHAR_TIMES:
      state = 1;
      break;
    case CHAR_RPAR:
      if (state == 1){
        readChar();
        return;
      }
      state = 0;
      break;
    default: 
      state = 0;
      break;
    }
    readChar();
  }
  error(ERR_ENDOFCOMMENT, lineNo, colNo);
}

Token* readIdentKeyword(void) {
  Token* token;
  int ln = lineNo;
  int cn = colNo;
  char tokenStr[MAX_IDENT_LEN + 1];
  int count = 0;
  while (currentChar != EOF && (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT))
  {
    if (count < MAX_IDENT_LEN)
    {
      tokenStr[count] = (char)currentChar;
    }
    count++;
    readChar();
  }
  tokenStr[count < MAX_IDENT_LEN ? count : MAX_IDENT_LEN] = '\0';
  if (count > MAX_IDENT_LEN) {
    error(ERR_IDENTTOOLONG, ln, cn);
  }
  TokenType tokenType = checkKeyword(tokenStr);
  if (tokenType != TK_NONE){
    token = makeToken(tokenType, ln, cn);
  } else {
    token = makeToken(TK_IDENT, ln, cn);
  }
  
  strcpy(token->string, tokenStr);
  return token;
}

Token* readNumber(void) {
  Token* token;
  int ln = lineNo;
  int cn = colNo;
  char tokenStr[MAX_IDENT_LEN + 1];
  int count = 0;
  while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT)
  {
    if (count < MAX_IDENT_LEN)
    {
      tokenStr[count] = (char)currentChar;
    }
    count++;
    readChar();
  }
  tokenStr[count < MAX_IDENT_LEN ? count : MAX_IDENT_LEN] = '\0';
  if (count > MAX_IDENT_LEN) {
    // error(ERR_IDENTTOOLONG, ln, cn);
  }
  token = makeToken(TK_NUMBER, ln, cn);
  strcpy(token->string, tokenStr);
  return token;
}

Token* readConstChar(void) {
  Token *token;
  int ln = lineNo;
  int cn = colNo;
  char charValue;
  readChar(); 

  if (currentChar == EOF) {
    error(ERR_INVALIDCHARCONSTANT, ln, cn);
    return makeToken(TK_NONE, ln, cn);
  }

  charValue = (char)currentChar;
  readChar(); 

  if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    if (charValue == '\n' || charCodes[(unsigned char)charValue] == CHAR_UNKNOWN) {
        error(ERR_INVALIDCHARCONSTANT, ln, cn);
        readChar(); 
        return makeToken(TK_NONE, ln, cn);
    }
    readChar();
  } else if (charCodes[(unsigned char)charValue] == CHAR_SINGLEQUOTE && charCodes[(unsigned char)currentChar] == CHAR_SINGLEQUOTE) {
    charValue = '\'';
    readChar(); 
  } else {
    error(ERR_INVALIDCHARCONSTANT, ln, cn);
    while (currentChar != EOF && charCodes[currentChar] != CHAR_SINGLEQUOTE) {
        readChar();
    }
    if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
        readChar();
    }
    return makeToken(TK_NONE, ln, cn);
  }
  
  token = makeToken(TK_CHAR, ln, cn);
  token->string[0] = charValue;
  token->string[1] = '\0';
  
  return token;
}

Token* getToken(void) {
  Token *token;
  int ln = lineNo; 
  int cn = colNo;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, ln, cn);
    readChar(); 
    return token;
  case CHAR_MINUS: 
    token = makeToken(SB_MINUS, ln, cn);
    readChar(); 
    return token;
  case CHAR_TIMES: 
    token = makeToken(SB_TIMES, ln, cn);
    readChar(); 
    return token;
  case CHAR_SLASH: 
    token = makeToken(SB_SLASH, ln, cn);
    readChar(); 
    return token;
  case CHAR_EQ:
    token = makeToken(SB_EQ, ln, cn);
    readChar();
    return token;
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, ln, cn);
    readChar();
    return token;
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, ln, cn);
    readChar();
    return token;
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, ln, cn);
    readChar();
    return token;
  case CHAR_LT: 
    readChar();
    if (currentChar == EOF) return makeToken(SB_LT, ln, cn);
    if (charCodes[currentChar] == CHAR_EQ) {
      token = makeToken(SB_LE, ln, cn); 
      readChar();
    } else {
      token = makeToken(SB_LT, ln, cn); 
    }
    return token;
  case CHAR_GT: 
    readChar();
    if (currentChar == EOF) return makeToken(SB_GT, ln, cn);
    if (charCodes[currentChar] == CHAR_EQ) {
      token = makeToken(SB_GE, ln, cn); 
      readChar();
    } else {
      token = makeToken(SB_GT, ln, cn); 
    }
    return token;
  case CHAR_EXCLAIMATION: 
    readChar();
    if (currentChar == EOF) {
        error(ERR_INVALIDSYMBOL, ln, cn);
        return makeToken(TK_NONE, ln, cn);
    }
    if (charCodes[currentChar] == CHAR_EQ) {
      token = makeToken(SB_NEQ, ln, cn); 
      readChar();
    } else {
      token = makeToken(TK_NONE, ln, cn);
      error(ERR_INVALIDSYMBOL, ln, cn);
      readChar(); 
    }
    return token;
  case CHAR_COLON: 
    readChar();
    if (currentChar == EOF) return makeToken(SB_COLON, ln, cn);
    if (charCodes[currentChar] == CHAR_EQ) {
      token = makeToken(SB_ASSIGN, ln, cn); 
      readChar();
    } else {
      token = makeToken(SB_COLON, ln, cn);
    }
    return token;
  case CHAR_PERIOD:
    readChar();
    if (currentChar == EOF) return makeToken(SB_PERIOD, ln, cn);
    if (charCodes[currentChar] == CHAR_RPAR) {
      token = makeToken(SB_RSEL, ln, cn); 
      readChar();
    } else {
      token = makeToken(SB_PERIOD, ln, cn); 
    }
    return token;
  case CHAR_LPAR: 
    readChar();
    if (currentChar == EOF) return makeToken(SB_LPAR, ln, cn);
    if (charCodes[currentChar] == CHAR_TIMES) {
      skipComment();
      return getToken(); 
    } else if (charCodes[currentChar] == CHAR_PERIOD) {
      token = makeToken(SB_LSEL, ln, cn); 
      readChar();
    } else {
      token = makeToken(SB_LPAR, ln, cn);
    }
    return token;
  case CHAR_SINGLEQUOTE: 
    return readConstChar();
  default:
    token = makeToken(TK_NONE, ln, cn);
    error(ERR_INVALIDSYMBOL, ln, cn);
    readChar(); 
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}