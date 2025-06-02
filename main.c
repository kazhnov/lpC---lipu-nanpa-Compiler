#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#ifdef DEBUG
const int debug = 1;
#else
const int debug = 0;
#endif

#define TOKEN_NAME 0
#define TOKEN_SEMI 1
#define TOKEN_OPAREN 2
#define TOKEN_CPAREN 3
#define TOKEN_COMMA 4
#define TOKEN_NUMBER 5
#define TOKEN_OCURLY 6
#define TOKEN_CCURLY 7
#define TOKEN_STRING_LITERAL 8
#define TOKEN_OTAWA 9
#define TOKEN_COLON 10
#define TOKEN_LA 11
#define TOKEN_ANTE 12
#define TOKEN_O 13
#define TOKEN_LI 14
#define TOKEN_ASEN 15
#define TOKEN_TENPO 16
#define TOKEN_PINI 17
#define TOKEN_SIGNED 98
#define TOKEN_UNSIGNED 99
#define TOKEN_NANPA 100
#define TOKEN_SITELEN 101
#define TOKEN_SULI 102
#define TOKEN_LILI 103
#define TOKEN_TELO 104
#define TOKEN_TELOTU 105
#define TOKEN_LINJA 106
#define TOKEN_AWEN 107
#define TOKEN_EQ 199
#define TOKEN_DEQ 200
#define TOKEN_LT 201
#define TOKEN_GT 202
#define TOKEN_PLUS 203
#define TOKEN_MINUS 204
#define TOKEN_STAR 205
#define TOKEN_FSLASH 206

typedef enum {
      Undefined = 0, 
      Comparing,
      Linear,
      Scaling,
} Precedence;

Precedence getPrecedence (size_t tokenType) {
      switch (tokenType) {
      case TOKEN_GT:
      case TOKEN_EQ:
      case TOKEN_LT:
            return Comparing;
      case TOKEN_PLUS:
      case TOKEN_MINUS:
            return Linear;
      case TOKEN_STAR:
      case TOKEN_FSLASH:
            return Scaling;
      }
      return Undefined;
}

typedef struct {
      int32_t type;
      void *value;
} Token;

int32_t cur = 0;

typedef struct {
      size_t size;
      size_t capacity;
      Token *tokens;
} Tokens;

Tokens tokensNew() {
      Tokens tokens;
      tokens.capacity = 1;
      tokens.size = 0;
      tokens.tokens = calloc(1, sizeof(Token));
      return tokens;
}

typedef struct {
      bool lon;
      char *value;
} NodeAsenpeli;

typedef struct {
      bool lon;
      int64_t value;
} NodeNanpaExpression;

typedef struct {
      bool lon;
      char* value;
} NodeNimiExpression;

typedef struct {
      bool lon;
      char *string;
} NodeLinjaExpression;

struct NodeExpression_t;
typedef struct NodeExpression_t NodeExpression;

typedef struct {
      bool lon;
      NodeNimiExpression nimi;
      NodeExpression *expr;
} NodeKamaExpression;

typedef struct {
      bool lon;
      enum {
            NanpaExpr = 0,
            NimiExpr = 1,
            LinjaExpr,
            KamaExpr,
      } type;
      union {
            NodeNanpaExpression nanpa;
            NodeNimiExpression nimi;
            NodeLinjaExpression linja;
            NodeKamaExpression kama;
      } value;
} NodeTerm;

typedef enum {
      BinAdd,
      BinMul,
      BinSub,
      BinDiv,
      BinGt,
      BinEq,
      BinLt,
} BinaryExpressionType;

typedef struct {
      bool lon;
      NodeExpression *lhs;
      NodeExpression *rhs;
      BinaryExpressionType type;
} NodeBinaryExpression;

typedef struct NodeExpression_t {
      bool lon;
      enum {
            BinaryExpr,
            TermExpr
      } type;
      union {
            NodeBinaryExpression *binExpr;
            NodeTerm term;
      } value;
} NodeExpression;

typedef struct {
      bool lon;
      NodeExpression *expr;
} NodeOtawa;

typedef struct {
      bool lon;
      enum {
            Nanpa,
            NanpaLili,
            NanpaSuli,
            Telo,
            TeloLili,
            TeloSuli,
            Pule,
            Linja,
      } type;
      bool awen;
} NodeType;

typedef struct {
      bool lon;
      NodeType type;
      Token name;
      NodeExpression *expr;
} NodeO;

typedef struct {
      bool lon;
      NodeKamaExpression *kama;
} NodeKama;

typedef struct NodeTenpo_t NodeTenpo;

typedef union {
      NodeExpression *expr;
      NodeOtawa *otawa;
      NodeO *o;
      NodeType type;
      NodeAsenpeli asen;
      NodeTenpo *tenpo;
      NodeKama kama;
} NodeUnion;

typedef enum {
      Expression,
      Otawa,
      O,
      Type,
      Asen,
      Tenpo,
      Kama,
} TypeOfNode;

typedef struct {
      TypeOfNode type;
      NodeUnion node;
} Node;

typedef struct {
      size_t size;
      size_t capacity;
      Node *nodes;
} Nodes;

typedef struct NodeTenpo_t {
      bool lon;
      NodeExpression *expr;
      Nodes nodes;
} NodeTenpo;

size_t tenpoNumber;

Nodes nodesNew() {
      Nodes nodes;
      nodes.capacity = 10;
      nodes.size = 0;
      nodes.nodes = calloc(nodes.capacity, sizeof(Node));
      return nodes;
}; 

void addToken(Tokens *tokens, Token token) {
      tokens->size++;
      if (tokens->size >= tokens->capacity) {
            tokens->capacity *= 2;
            tokens->tokens = realloc(tokens->tokens, sizeof(Token)*tokens->capacity);
      }
      tokens->tokens[tokens->size-1] = token;
}

Token getToken(Tokens *tokens, size_t index) {
      if (index == tokens->size) {
            fprintf(stderr, "ERROR: Token index out of bounds\n");
            exit(1);
      }
      return tokens->tokens[index];
}

void addNode(Nodes *nodes, Node node) {
      nodes->size++;
      if (nodes->size >= nodes->capacity) {
            nodes->capacity *= 2;
            nodes->nodes = realloc(nodes->nodes, sizeof(Node)*nodes->capacity);
      }
      nodes->nodes[nodes->size-1] = node;
}

Node getNode(Nodes *nodes, size_t index) {
      if (index == nodes->size) {
            fprintf(stderr, "ERROR: Node index out of bounds\n");
            exit(1);
      }
      return nodes->nodes[index];
}

typedef struct {
      uint8_t *buffer;
      size_t ptr;
      size_t capacity;
} Arena;

typedef struct {
      Arena arena;
      Nodes nodes;
} Prog;

Arena arenaNew(size_t size) {
      Arena arena;
      arena.buffer = calloc(size, sizeof(uint8_t));
      arena.ptr = 0;
      arena.capacity = size;
      if (debug) {
            printf("Initialized an arena of size %zu, first byte is %c, last byte is %c\n", size, arena.buffer[0], arena.buffer[size-1]);
      }
      return arena;
}

void *pushArena(Arena *arena, void *src, size_t size) {
      size_t oldptr = arena->ptr;
      if (debug) {
            printf("Pushing an object of size %zu onto the arena stack with a ptr %zu...\n", size, oldptr);
      }

      memccpy(arena->buffer + oldptr, src, '\0', size);

      arena->ptr += size;

      if (debug) printf("Succes\n");
      return arena->buffer + oldptr;
}

void deleteArena(Arena *arena) {
      free(arena->buffer);
};

typedef struct {
      size_t size;
      size_t capacity;
      char **names;
      size_t *values;
      NodeType *types;
} NameMap;

NameMap nameMapNew() {
      NameMap map;
      map.capacity = 1;
      map.size = 0;
      map.names = calloc(1, sizeof(char*));
      map.values = calloc(1, sizeof(size_t));
      map.types = calloc(1, sizeof(NodeType));
      return map;
}

void addNameMap(NameMap *map, char *name, size_t value, NodeType type) {
      map->size++;
      if (map->size >= map->capacity) {
            map->capacity *= 2;
            map->names = realloc(map->names, sizeof(char*)*map->capacity);
            map->values = realloc(map->values, sizeof(size_t)*map->capacity);
            map->types = realloc(map->types, sizeof(NodeType)*map->capacity);
      }
      //map->names[size-1] = calloc(strlen(name), sizeof(char));
      map->names[map->size-1] = strdup(name);
      map->values[map->size-1] = value;
      map->types[map->size-1] = type;      
}

size_t getNameMap(NameMap *map, char *name) {
      for (size_t i = 0; i < map->size; i++) {
            if(!strcmp(name, map->names[i])) {
                  return map->values[i];
            }
      }
      assert(false);
      return -1;
}


NodeType getNameMapType(NameMap *map, char *name) {
      for (size_t i = 0; i < map->size; i++) {
            if(!strcmp(name, map->names[i])) {
                  return map->types[i];
            }
      }
      assert(false);
      return (NodeType){};
}

bool hasNameMap(NameMap *map, char *name) {
      for (size_t i = 0; i < map->size; i++) {
            if(!strcmp(name, map->names[i])) {
                  return true;
            }
      }
      return false;
}

char peek(char* buffer) {
      //char c = fgetc(buffer);
      //ungetc(c, buffer);
      //return c;
      if (buffer[cur] == 0) return EOF;
      return buffer[cur];
}

NameMap vars;
Arena arena;

char consume(char* buffer) {
      //return fgetc(buffer);
      return buffer[cur++];
}

Tokens tokenize(char* buffer) {
      char c;
      Tokens tokens = tokensNew();
      Tokens *tokensptr = &tokens;
      cur = 0;
      while ((c = peek(buffer)) != EOF) {
            if (isalpha(c)) {
                  int32_t firstchar = cur;
                  while(isalnum(c = peek(buffer)) && c != EOF) {
                        consume(buffer);
                  }
                  char *name = calloc(cur - firstchar, sizeof(char));
                  
                  strncpy(name, buffer+firstchar, cur - firstchar);
                  name[cur - firstchar] = 0;
                  Token token;
                  char* prefix = "keyword";
                  // check for keywords
                  if (!strcmp(name, "otawa")) {
                        token.type = TOKEN_OTAWA;
                  } else if (!strcmp(name, "o")) {
                        token.type = TOKEN_O;
                  } else if (!strcmp(name, "li")) {
                        token.type = TOKEN_LI;
                  } else if (!strcmp(name, "nanpa")) {
                        token.type = TOKEN_NANPA;
                  } else if (!strcmp(name, "sitelen")) {
                        token.type = TOKEN_SITELEN;
                  } else if (!strcmp(name, "suli")) {
                        token.type = TOKEN_SULI;
                  } else if (!strcmp(name, "lili")) {
                        token.type = TOKEN_LILI;
                  } else if (!strcmp(name, "telotu")) {
                        token.type = TOKEN_TELOTU;
                  } else if (!strcmp(name, "signed")) {
                        token.type = TOKEN_SIGNED;
                  } else if (!strcmp(name, "unsigned")) {
                        token.type = TOKEN_UNSIGNED;
                  } else if (!strcmp(name, "la")) {
                        token.type = TOKEN_LA;
                  } else if (!strcmp(name, "ante")) {
                        token.type = TOKEN_ANTE;
                  } else if (!strcmp(name, "tenpo")) {
                        token.type = TOKEN_TENPO;
                  } else if (!strcmp(name, "pini")) {
                        token.type = TOKEN_PINI;
                  } else if (!strcmp(name, "awen")) {
                        token.type = TOKEN_AWEN;
                  } else if (!strcmp(name, "case")) {
                  } else if (!strcmp(name, "default")) {
                  } else if (!strcmp(name, "break")) {
                  } else if (!strcmp(name, "linja")) {
                        token.type = TOKEN_LINJA;
                  } else if (!strcmp(name, "asen")) {
                        token.type = TOKEN_ASEN;
                  } else {
                        token.type = TOKEN_NAME;
                        token.value = name;
                        prefix = "name";
                  }
                  if (debug) printf("%s: '%s'\n", prefix, name);
                  addToken(tokensptr, token);
            }
            else if (isdigit(c)) {
                  int32_t firstchar = cur;
                  while(isalnum(c = peek(buffer)) && c != EOF) {
                        consume(buffer);
                  }
                  char *number = calloc(cur - firstchar, sizeof(char));
                  strncpy(number, buffer+firstchar, cur - firstchar);
                  number[cur - firstchar] = 0;
                  
                  if (debug) printf("number: '%s'\n", number);
                  Token token = (Token){.type = TOKEN_NUMBER, .value = number};
                  addToken(&tokens, token);
            }
            else if (c == '"') {
                  consume(buffer);
                  int32_t firstchar = cur;
                  int32_t stringSize = 0;
                  
                  while((c = peek(buffer)) && c != '"' && c != EOF) {
                        c = consume(buffer);
                        stringSize++;
                  }
                  
                  char *string = calloc(cur - firstchar, sizeof(char));
                  strncpy(string, buffer+firstchar, cur - firstchar);
                  string[cur - firstchar] = 0;
                  consume(buffer);

                  Token token = (Token){.type = TOKEN_STRING_LITERAL, .value = string};
                  if (debug) printf("string literal: %s\n", string);
                  addToken(&tokens, token);
            }
            else if (c == ';') {             
                  Token token = (Token){.type = TOKEN_SEMI, .value = 0};
                  consume(buffer);
                  if (debug) printf("semicolon\n");
                  addToken(&tokens, token);
            }
            else if (c == '(') {
                  if (debug) printf("oparen\n");
                  consume(buffer);
                  addToken(&tokens, (Token){.type = TOKEN_OPAREN});
            }
            else if (c == ')') {
                  if (debug) printf("cparen\n");
                  consume(buffer);
                  addToken(&tokens, (Token){.type = TOKEN_CPAREN});
            }
            else if (c == '{') {
                  if (debug) printf("ocurly\n");
                  consume(buffer);
                  addToken(&tokens, (Token){.type = TOKEN_OCURLY});
            }
            else if (c == '}') {
                  if (debug) printf("ccurly\n");
                  consume(buffer);
                  addToken(&tokens, (Token){.type = TOKEN_CCURLY});
            }
            else if (c == ',') {
                  if (debug) printf("comma\n");
                  consume(buffer);
                  addToken(&tokens, (Token){.type = TOKEN_COMMA});
            }
            else if (c == '=') {
                  consume(buffer);
                  if (peek(buffer) == '=') {
                        if (debug) printf("double equals\n");
                        addToken(&tokens, (Token){.type = TOKEN_DEQ});
                        consume(buffer);
                        continue;
                  } else {
                        if (debug) printf("equals\n");
                        addToken(&tokens, (Token){.type = TOKEN_EQ});
                  }
            }
            else if (c == '<') {
                  if (debug) printf("less than\n");
                  consume(buffer);
                  addToken(&tokens, (Token){.type = TOKEN_LT});
            }
            else if (c == '>') {
                  if (debug) printf("greater than\n");
                  consume(buffer);
                  addToken(&tokens, (Token){.type = TOKEN_GT});
            }
            else if (c == ':') {
                  if (debug) printf("colon\n");
                  consume(buffer);
                  addToken(&tokens, (Token){.type = TOKEN_COLON});
            }
            else if (c == '+') {
                  if (debug) printf("plus\n");
                  consume(buffer);
                  addToken(&tokens, (Token){.type = TOKEN_PLUS});
            } 
            else if (c == '-') {
                  if (debug) printf("minus\n");
                  consume(buffer);
                  addToken(&tokens, (Token){.type = TOKEN_MINUS});
            }
            else if (c == '*') {
                  if (debug) printf("star\n");
                  consume(buffer);
                  addToken(&tokens, (Token){.type = TOKEN_STAR});
            } 
            else if (c == '/') {
                  if (debug) printf("fslash\n");
                  if (peek(buffer) == '/') 
                  consume(buffer);
                  if (peek(buffer) == '/') {
                        while (peek(buffer) != '\n') {
                              consume(buffer);
                        }
                        continue;
                  }

                  addToken(&tokens, (Token){.type = TOKEN_FSLASH});
            }
            else if (isspace(c)) {
                  consume(buffer);
            }
            else {                  
                  if (debug) printf("Unexpected character %c\n", c);
                  consume(buffer);
            }

      }
      return tokens;
}

size_t curToken = 0;

Token tokenPeek(Tokens *tokens) {
      if (curToken >= tokens->size) return (Token) {.type = -1};
      return tokens->tokens[curToken];
}

Token tokenPeekAhead(Tokens *tokens, size_t ahead) {
      if (curToken + ahead >= tokens->size) return (Token) {.type = -1};
      return tokens->tokens[curToken + ahead];
}

Token tokenConsume(Tokens *tokens) {
      return tokens->tokens[curToken++];
}


NodeNanpaExpression parseNanpaExpr(Tokens *tokens, Arena *arena) {
      if(tokenPeek(tokens).type == TOKEN_NUMBER) {
            Token token = tokenConsume(tokens);
            return (NodeNanpaExpression){.lon = true, .value = atoi(token.value)};
      }
      assert(false);
}

NodeNimiExpression parseNimiExpr(Tokens *tokens, Arena *arena) {
      if(tokenPeek(tokens).type == TOKEN_NAME) {
            Token token = tokenConsume(tokens);
            return (NodeNimiExpression){.lon = true, .value = strdup((char*)token.value)};
      }
      assert (false);
}

NodeLinjaExpression parseLinjaExpr(Tokens *tokens, Arena *arena) {
      if (tokenPeek(tokens).type == TOKEN_STRING_LITERAL) {
            Token token = tokenConsume(tokens);
            NodeLinjaExpression linja;
            linja.lon = true;
            linja.string = strdup(token.value);
            return linja;
      };
      assert(false);
}

NodeExpression *parseExpr(Tokens *tokens, Arena *arena, Precedence minPrec);

NodeKamaExpression *parseKamaExpr(Tokens *tokens, Arena *arena) {
      if (tokenPeek(tokens).type == TOKEN_NAME) {
            NodeNimiExpression nimi = parseNimiExpr(tokens, arena);
            if (!nimi.lon) {
                  fprintf(stderr, "No name given in kama expression\n");
                  exit(1);
            }
            
            if (tokenPeek(tokens).type != TOKEN_EQ) {
                  fprintf(stderr, "No '=' in kama expression\n");
                  exit(1);
            }
            tokenConsume(tokens);

            NodeExpression *expr = parseExpr(tokens, arena, 0);

            if (tokenPeek(tokens).type != TOKEN_SEMI) {
                  fprintf(stderr, "No ';' in kama expression\n");
                  exit(1);
            }
            
            NodeKamaExpression *node = malloc(sizeof(NodeKamaExpression));
            node->lon = true;
            node->expr = expr;
            node->nimi = nimi;
            return node;
      }
      return NULL;
}

NodeKama parseKama(Tokens *tokens, Arena *arena) {
      NodeKamaExpression *expr = parseKamaExpr(tokens, arena);
      if (!expr->lon) {
            fprintf(stderr, "Invalid kama statement\n");
            exit(1);
      }
      if (tokenPeek(tokens).type != TOKEN_SEMI) {
            fprintf(stderr, "No ';' after kama statement\n");
            exit(1);
      }
      tokenConsume(tokens);
      return (NodeKama){.lon = true, .kama = expr};
}

NodeTerm parseTerm(Tokens *tokens, Arena *arena) {
      if (tokenPeek(tokens).type == TOKEN_NUMBER) {
            NodeNanpaExpression node;
            if (!(node = parseNanpaExpr(tokens, arena)).lon) {
                  fprintf(stderr, "No number given\n");
                  exit(1);
            }
            NodeTerm term;
            term.lon = true;
            term.type = NanpaExpr;
            term.value.nanpa = node;
            return term;
      }
      if (tokenPeek(tokens).type == TOKEN_NAME) {
            NodeNimiExpression node;
            if (!(node = parseNimiExpr(tokens, arena)).lon) {
                  fprintf(stderr, "No name given\n");
                  exit(1);
            }

            *node.value;
            
            return (NodeTerm) {.lon = true, .type = NimiExpr, .value.nimi = node};

      }
      if (tokenPeek(tokens).type == TOKEN_STRING_LITERAL) {
            NodeLinjaExpression node;
            if (!(node = parseLinjaExpr(tokens, arena)).lon) {
                  fprintf(stderr, "No string literal given\n");
                  exit(1);
            }
            return (NodeTerm){.lon = true, .type = LinjaExpr, .value.linja = node};
      }
      return (NodeTerm) {.lon = false};
}

NodeExpression *parseExpr(Tokens *tokens, Arena *arena, Precedence minPrec) {
      NodeTerm lhsTerm = parseTerm(tokens, arena);

      if (!lhsTerm.lon) {
            fprintf(stderr, "No term!\n");
            exit(1);
      }

      NodeExpression *lhsExpr = malloc(sizeof(NodeExpression));
      lhsExpr->lon = true;
      lhsExpr->type = TermExpr;
      lhsExpr->value.term = lhsTerm;
      
      while (true) {
            // determine the type
            Token token = tokenPeek(tokens);
            BinaryExpressionType type;

            if (token.type == TOKEN_PLUS) {
                  type = BinAdd;
            } else if (token.type == TOKEN_MINUS) {
                  type = BinSub;
            } else if (token.type == TOKEN_STAR) {
                  type = BinMul;
            } else if (token.type == TOKEN_FSLASH) {
                  type = BinDiv;
            } else if (token.type == TOKEN_GT) {
                  type = BinGt;
            } else if (token.type == TOKEN_DEQ) {
                  type = BinEq;
            } else if (token.type == TOKEN_LT) {
                  type = BinLt;
            }
            else {
                  break;
            }

            Precedence curPrec = getPrecedence(token.type);
            if (curPrec < minPrec) break;
            Precedence nextMinPrec = curPrec + 1;
            tokenConsume(tokens);

            NodeExpression *rhsExpr = parseExpr(tokens, arena, nextMinPrec);

            if (!rhsExpr->lon) {
                  fprintf(stderr, "No second Expression!\n");
                  exit(1);
            }

            NodeBinaryExpression *binExpr = malloc(sizeof(NodeBinaryExpression));
            binExpr->lon = true;

            NodeExpression *lhsExpr2 = malloc(sizeof(NodeExpression));
            lhsExpr2->lon = true;
            
            
            //if (type == BinAdd) {
            if (lhsExpr->type == BinaryExpr) {
                  lhsExpr2->value.binExpr = lhsExpr->value.binExpr;
            } else {
                  lhsExpr2->value.term = lhsExpr->value.term;
            }

            lhsExpr2->type = lhsExpr->type;

            binExpr->lhs = lhsExpr2;
            binExpr->rhs = rhsExpr;
            binExpr->type = type;
            binExpr->lon = true;
            //} else {
            //assert(false);
            //}

            
            lhsExpr->value.binExpr = binExpr;
            lhsExpr->type = BinaryExpr;

      }
      return lhsExpr;
}

NodeAsenpeli parseAsenpeli(Tokens *tokens, Arena *arena) {
      if (tokenPeek(tokens).type != TOKEN_ASEN) {
            fprintf(stderr, "No 'asem' in assembly call\n");
            exit(1);

      }
      
      tokenConsume(tokens);
      if (tokenPeek(tokens).type != TOKEN_STRING_LITERAL) {
            fprintf(stderr, "No assembly instructions given\n");
            exit(1);
      }
      NodeExpression *instructions = parseExpr(tokens, arena, 0);

      if (!instructions->lon) {
            fprintf(stderr, "No expression given after asen\n");
            exit(1);
      }

      if (instructions->type != TermExpr) {
            fprintf(stderr, "Not a valid expression after asen\n");
            exit(1);
      }

      if (instructions->value.term.type != LinjaExpr) {
            fprintf(stderr, "Not a string literal after asen\n");
            exit(1);
      }

      if (tokenPeek(tokens).type != TOKEN_SEMI) {
            fprintf(stderr, "No ';' after asen\n");
            exit(1);
      }
      tokenConsume(tokens);
      
      NodeAsenpeli node;
      node.lon = true;
      node.value = strdup(instructions->value.term.value.linja.string);
      
      return node;
}


NodeOtawa *parseOtawa(Tokens *tokens, Arena *arena) {
      if (tokenPeek(tokens).type == TOKEN_OTAWA) {

            tokenConsume(tokens);

            NodeExpression *expr;
            if ((expr = parseExpr(tokens, arena, 0))->lon == false) {
                  fprintf(stderr, "No value given after otawa\n");
                  exit(1);
            }

            if (tokenPeek(tokens).type != TOKEN_SEMI) {
                  fprintf(stderr, "No ';' after otawa\n");
                  exit(1);
            }
            //printf("Expression is %s\n", expr.value.value);
            //NodeExpression* exprptr = pushArena(arena, &expr, sizeof(expr));

            tokenConsume(tokens);
            NodeOtawa *node = malloc(sizeof(NodeOtawa));
            node->expr = expr;
            node->lon = true;
            return node;
      }
      return NULL;
}

NodeType parseType(Tokens *tokens) {
      NodeType type;
      type.lon = true;
      type.awen = false;
      if (tokenPeek(tokens).type == TOKEN_AWEN) {
            tokenConsume(tokens);
            type.awen = true;
      }
      
      if (tokenPeek(tokens).type == TOKEN_NANPA) {
            tokenConsume(tokens);
            type.type = Nanpa;
      } else if (tokenPeek(tokens).type == TOKEN_LINJA) {
            tokenConsume(tokens);
            type.type = Linja;
      } else {
            return (NodeType){};
      }

      if (tokenPeek(tokens).type == TOKEN_AWEN) {
            tokenConsume(tokens);
            type.awen = true;
      }

      return type;
};

NodeO *parseO(Tokens *tokens, Arena *arena) {
      if (tokenPeek(tokens).type == TOKEN_O) {
            tokenConsume(tokens);
            if (tokenPeek(tokens).type != TOKEN_NAME) {
                  fprintf(stderr, "No name after o\n");
                  exit(1);
            }
            Token name = tokenConsume(tokens);

            if (tokenPeek(tokens).type != TOKEN_LI) {
                  fprintf(stderr, "No li after o\n");
                  exit(1);
            }
            tokenConsume(tokens);

            NodeType type = parseType(tokens);

            if (!type.lon) {
                  fprintf(stderr, "No type after o\n");
                  exit(1);                  
            }

            if (tokenPeek(tokens).type != TOKEN_EQ) { 
                  fprintf(stderr, "No '=' after o\n");
                  exit(1);                  
            }
            tokenConsume(tokens);

            NodeExpression *expr = parseExpr(tokens, arena, 0);

            if (!expr->lon) {
                  fprintf(stderr, "No expression after o\n");
                  exit(1);                  
            }

            if (tokenPeek(tokens).type != TOKEN_SEMI) {
                  fprintf(stderr, "No ';' after o\n");
                  exit(1);                  
            }
            tokenConsume(tokens);

            NodeO *node = malloc(sizeof(NodeO));
            node->lon = true;
            node->type = type;
            node->expr = expr;
            node->name = name;
            
            return node;
      }
      return NULL;
}

void parseStatement(Tokens *tokens, Arena *arena, Nodes *nodes);
      
NodeTenpo *parseTenpo(Tokens *tokens, Arena* arena) {
      if (tokenPeek(tokens).type != TOKEN_TENPO) {
            assert(false);
      }
      tokenConsume(tokens);

      NodeExpression *expr = parseExpr(tokens, arena, 0);
      if (!expr) {
            fprintf(stderr, "Invalid expression after tenpo\n");
            exit(1);
      }

      if (tokenPeek(tokens).type != TOKEN_LA) {
            fprintf(stderr, "Expected 'la' after tenpo\n");
            exit(1);
      }

      tokenConsume(tokens);
     
      NodeTenpo *node = malloc(sizeof(NodeTenpo));
      node->expr = expr;
      node->lon = true;
      node->nodes = nodesNew();

      while (tokenPeek(tokens).type != TOKEN_PINI) {
            if (tokenPeek(tokens).type == -1) {
                  fprintf(stderr, "Reached end of the expression while in 'tenpo'\n");
                  exit(1);
            }
            parseStatement(tokens, arena, &node->nodes);
      }

      tokenConsume(tokens);
      
      return node;
}

void parseStatement(Tokens *tokens, Arena *arena, Nodes *nodes) {
      Token token = tokenPeek(tokens);
      if (token.type == TOKEN_OTAWA) {
            NodeOtawa *otawa = parseOtawa(tokens, arena);
            if (!otawa) {
                  fprintf(stderr, "Unable to parse O-expression\n");
                  exit(1);
            }
                  
            addNode(nodes, (Node) {.type = Otawa, .node.otawa = otawa});
      } else if (token.type == TOKEN_O) {
            NodeO *o = parseO(tokens, arena);
            if (!o) {
                  fprintf(stderr, "Unable to parse O-expression\n");
                  exit(1);
            }
            addNode(nodes, (Node){.type = O, .node.o = o});
      } else if (token.type == TOKEN_ASEN) {
            NodeAsenpeli asen = parseAsenpeli(tokens, arena);
            addNode(nodes, (Node){.type = Asen, .node.asen = asen});
      } else if (token.type == TOKEN_TENPO) {
            NodeTenpo *tenpo = parseTenpo(tokens, arena);
            addNode(nodes, (Node){.type = Tenpo, .node.tenpo = tenpo});
      } else if (token.type == TOKEN_NAME) {
            NodeKama kama = parseKama(tokens, arena);
            addNode(nodes, (Node){.type = Kama, .node.kama = kama});
      } else {
            fprintf(stderr, "Unable to parse the expression\n");
            exit(1);
      }
}

Prog parse(Tokens *tokens) {
      Prog program;
      //program.arena = arenaNew(1*1024*1024);
      program.nodes = nodesNew();
      while(tokenPeek(tokens).type != -1) {
            parseStatement(tokens, &program.arena, &program.nodes);
      }
      return program;
}

size_t stackOffset = 0;

void push(size_t i) {
      printf("    mov r8, %ld\n"
             "    push r8\n",
             i);
      stackOffset++;
}

void push_str(char *str) {
      printf("    mov r8, %s\n"
             "    push r8\n",
             str);
      stackOffset++;
}

void push_reg(char *str) {
      printf("    push %s\n",
             str);
      stackOffset++;
}

void pop(char* reg) {
      printf("    pop %s\n",
             reg);
      stackOffset--;
}

void generateAsenpeli(NodeAsenpeli asen) {
      printf("\n    ;; Start raw assembly instructions\n");
      printf("%s", asen.value);
      printf("\n    ;; End raw assebly instructions\n");
}

void generateExpression(NodeExpression expr);

void generateTerm(NodeTerm term) {
      if (term.type == NanpaExpr) {
             push(term.value.nanpa.value);
      } else if (term.type == NimiExpr) {
            if (!hasNameMap(&vars, term.value.nimi.value)) {
                  printf("Undefined identifier %s\n", (char*) term.value.nimi.value);
                  exit(1);
            }
            size_t offset = getNameMap(&vars, term.value.nimi.value);
            printf("    mov r8, rsp\n"
                   "    add r8, %ld\n", (stackOffset - offset) * 8);
            
            push_str("[r8]");
      } else if (term.type == KamaExpr) {
            //printf("kama expression\n");
            if (!hasNameMap(&vars, term.value.kama.nimi.value)) {
                  fprintf(stderr, "Undefined identifier %s\n", (char*) term.value.nimi.value);
                  exit(1);
            }
            NodeType type = getNameMapType(&vars, term.value.kama.nimi.value);
            if (type.awen) {
                  fprintf(stderr, "Trying to change an awen value\n");
                  exit(1);
            }
            size_t offset = getNameMap(&vars, term.value.kama.nimi.value);
            //printf("offset is %ld\n", offset);
            generateExpression(*term.value.kama.expr);

            pop("r8");
            printf("    mov r9, rsp\n"
                   "    add r9, %ld\n"
                   "    mov [r9], r8\n",
                   (stackOffset - offset) * 8);
            push_reg("r8");
      }
}

void generateBinaryExpression(NodeBinaryExpression binExpr);

void generateExpression(NodeExpression expr) {
      if (expr.type == TermExpr) {
            generateTerm(expr.value.term);
      } else if (expr.type == BinaryExpr) {
            generateBinaryExpression(*expr.value.binExpr);
      }
}

void generateKama(NodeKama kama) {
      NodeTerm term = (NodeTerm){.lon = true, .type = KamaExpr, .value.kama = *kama.kama};
      generateTerm(term);
      pop("r8");
}

void generateBinaryExpression(NodeBinaryExpression binExpr) {
      generateExpression(*binExpr.lhs);
      generateExpression(*binExpr.rhs);

      switch(binExpr.type) {
      case BinAdd:
            pop("r9");
            pop("r8");
            printf("    add r8, r9\n");
            push_reg("r8");
            break;
      case BinSub:
            pop("r9");
            pop("r8");
            printf("    sub r8, r9\n");
            push_reg("r8");
            break;
      case BinMul:
            pop("r8");
            pop("rax");

            printf("    mov rdx, 0\n"
                   "    mul r8\n");
            push_reg("rax");
            break;
      case BinDiv:
            pop("r8");
            pop("rax");

            printf("    mov rdx, 0\n"
                   "    div r8\n");
            push_reg("rax");
            break;
      case BinGt:
            pop("r9");
            pop("r8");
            printf("    push 1\n"
                   "    cmp r8, r9\n"
                   "    jg $+6\n"
                   "    pop r8\n"
                   "    push 0\n");                   
            stackOffset++;
            break;
      case BinEq:
            pop("r9");
            pop("r8");
            printf("    push 1\n"
                   "    cmp r8, r9\n"
                   "    je $+6\n"
                   "    pop r8\n"
                   "    push 0\n");
            stackOffset++;
            break;
      case BinLt:
            pop("r9");
            pop("r8");
            printf("    push 1\n"
                   "    cmp r8, r9\n"
                   "    jl $+6\n"
                   "    pop r8\n"
                   "    push 0\n");
            stackOffset++;
                   break;                   
      default:
            return;
            break;
      }
      
}

void generateO(NodeO o) {
      int x = 5;
      if (hasNameMap(&vars, o.name.value)) {
            fprintf(stderr, "Duplicate variable declaration");
            exit(1);
      }
      generateExpression(*o.expr);
     
      pop("r8");
      push_reg("r8");

      if (o.name.type != TOKEN_NAME)
            assert(false);

      if (!o.type.lon)
            assert(false);
      
      addNameMap(&vars, o.name.value, stackOffset, o.type);
}

void generateOtawa(NodeOtawa otawa) {
      generateExpression(*otawa.expr);
      pop("rdi");
      printf("    mov rax, 60\n"
             "    syscall\n");
}

void generateTenpo(NodeTenpo tenpo);

void generateStatement(Node* node) {
      if (node->type == Otawa) generateOtawa(*node->node.otawa);
      else if (node->type == Asen) generateAsenpeli(node->node.asen);
      else if (node->type == O) generateO(*node->node.o);
      else if (node->type == Kama) generateKama(node->node.kama);
      else if (node->type == Tenpo) generateTenpo(*node->node.tenpo);
}

size_t loopNumber = 0;

void generateTenpo(NodeTenpo tenpo) {
      printf(".loopin%ld:\n", loopNumber);
      size_t oldLoop = loopNumber++;
      generateExpression(*tenpo.expr);
      pop("rcx");
      printf("    cmp rcx, 0\n"
             "    je .loopout%ld\n",
             oldLoop);
      for (size_t i = 0; i < tenpo.nodes.size; i++) {
            Node node = getNode(&tenpo.nodes, i);
            generateStatement(&node);
      }
      printf("    jmp .loopin%ld\n"
             ".loopout%ld:\n",
             oldLoop, oldLoop);
}

void generate(Prog prog) {
      printf("global _start\n"
             "_start:\n");
      for (size_t i = 0; i < prog.nodes.size; i++) {
            Node node = getNode(&prog.nodes, i);
            generateStatement(&node);
      }
      printf("    mov rax, 60\n"
             "    mov rdi, 0\n"
             "    syscall\n");
}
      
char* file_to_charptr_new(char* filename) {
      char * buffer = 0;
      long length;
      FILE * f = fopen (filename, "rb");
      if (f)
            {
                  fseek (f, 0, SEEK_END);
                  length = ftell (f);
                  fseek (f, 0, SEEK_SET);
                  buffer = malloc (length);
                  if (buffer)
                        {
                              fread (buffer, 1, length, f);
                        }
                  fclose (f);
            }
      if (buffer) {
               return buffer;
      }
      else {
            printf("ERROR file not found\n");
            exit(0);
      }
}

int main(int argc, char **argv) {
      vars = nameMapNew();
      size_t length = 0;
      char *f = file_to_charptr_new("test.ln");
      Tokens tokens = tokenize(f);
      Prog prog = parse(&tokens);

      generate(prog);
      
      free(f);
      return 0;
}

