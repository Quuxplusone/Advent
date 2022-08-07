/*  $VER: vbcc (vbcc_cpp.h) $Revision: 1.2 $     */


/* fix some name clashes between vbcc and ucpp and include cpp.h */
/* has to be include prior to other vbcc includes                */


#define STRING T_STRING
#define CHAR T_CHAR
#define CAST T_CAST
#define AND T_AND
#define OR T_OR
#define COLON T_COLON
#define LOR T_LOR
#define LAND T_LAND
#define MINUS T_MINUS

#define NO_UCPP_ERROR_FUNCTIONS 1
#include "ucpp/cpp.h"

typedef struct token token;
typedef struct lexer_state lexer_state;
typedef struct stack_context stack_context;

extern token *ctok;
extern lexer_state ls;

#undef STRING
#undef CHAR
#undef CAST
#undef AND
#undef OR
#undef COLON
#undef LOR
#undef LAND
#undef MINUS

#define next_token vbcc_next_token
#undef error

#undef S_TOKEN
#define S_TOKEN(x) ((x) >= NUMBER && (x) <= T_CHAR) 
 
#define ttMWS(x)        ((x) == NONE || (x) == COMMENT || (x) == OPT_NONE)
#define ttWHI(x)        (ttMWS(x) || (x) == NEWLINE)

char *ucpp_token_name();
