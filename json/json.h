#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

typedef struct Entry Entry;
typedef struct Json Json;
typedef enum ElementValueType ElementValueType;
typedef enum JsonType JsonType;

typedef void (*JsonWalker)(Entry *, ...);

enum ElementValueType
{
  STRING,
  NUMBER,
  BOOLEAN,
  JSON,
};

enum JsonType
{
  OBJECT,
  ARRAY
};

struct Json
{
  Entry *root;
  Entry *tail;
  JsonType TYPE;
  unsigned total_entries;
};
struct Entry
{
  const char *key;
  void *value;
  Entry *next;
  ElementValueType type;
};

Json *Json_NewObject();
Json *Json_NewArray();

void Json_Delete(Json *json);
void Json_SetString(Json *json, const char *key, const char *value);
void Json_SetNumber(Json *json, const char *key, double value);
void Json_SetBoolean(Json *json, const char *key, int value);
void Json_SetNested(Json *json, const char *key, Json *value);
void Json_AppendArrayAsString(Json *json, const char *value);
void *Json_Get(Json *json, const char *key);
void Json_WalkDFS(Json *json, JsonWalker callback, ...);
void Json_Print(Json *json);
char *Json_Stringify(Json *json);
