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
void __Json_AppendArray(Json *json, ElementValueType type, void *value);
void Json_AppendArrayAsString(Json *json, const char *value);
void Json_AppendArrayAsNumber(Json *json, double value);
void Json_AppendArrayAsBoolean(Json *json, int value);
void Json_AppendArrayAsNested(Json *json, Json *value);
Entry *Json_Get(Json *json, const char *key);
void Json_WalkDFS(Json *json, JsonWalker callback, ...);
void Json_Print(Json *json);
void __Json_PrintCallback(Entry *entry, ...);
char *Json_Stringify(Json *json);
char *__Json_StringifyEntry(Entry *entry, JsonType context);

Json *__Json_New(JsonType type);
Entry *__Json_NewEntry(ElementValueType type, const char *key, void *value);
Entry *__Json_FindEntryByKey(Json *json, const char *key);
void __Json_Set(Json *json, ElementValueType type, const char *key, void *value);
void __Json_EntryDelete(Entry *entry);
char *__Json_Util_CopyString(const char *source);
void __Json_Util_ExitIfAllocationFailed(void *p);

/* Implementation */

const char *ELEMENT_VALUE_TYPE_STRING[] = {
    "STRING",
    "NUMBER",
    "BOOLEAN",
    "JSON"};
const char *JSON_TYPE_STRING[] = {
    "OBJECT",
    "ARRAY",
};

Json *__Json_New(JsonType type)
{
  Json *json = malloc(sizeof(Json));
  json->root = json->tail = NULL;
  json->total_entries = 0;
  json->TYPE = type;
  return json;
}
Json *Json_NewObject()
{
  return __Json_New(OBJECT);
}
Json *Json_NewArray()
{
  return __Json_New(ARRAY);
}
Entry *__Json_NewEntry(ElementValueType type, const char *key, void *value)
{
  Entry *entry = malloc(sizeof(Entry));

  entry->next = NULL;
  entry->key = key;
  entry->type = type;
  entry->value = value;
  return entry;
}

void Json_WalkDFS(Json *json, JsonWalker callback, ...)
{
  if (json == NULL || json->root == NULL)
  {
    return;
  }

  va_list args;
  va_start(args, callback);

  Entry *entry = json->root;
  while (entry != NULL)
  {
    callback(entry, args);
    entry = entry->next;
  }

  va_end(args);
}
void Json_Print(Json *json)
{
  Json_WalkDFS(json, __Json_PrintCallback);
}
void __Json_PrintCallback(Entry *entry, ...)
{
  Json *j = NULL;
  switch (entry->type)
  {
  case JSON:
    j = (Json *)(entry->value);
    printf("%s: [%s, total entry = %u]\n", entry->key, JSON_TYPE_STRING[j->TYPE], j->total_entries);
    Json_Print(entry->value);
    break;
  case STRING:
    printf("%s: %s\n", entry->key, (char *)entry->value);
    break;

  case NUMBER:
    printf("%s: %.2lf\n", entry->key, *(double *)entry->value);
    break;

  case BOOLEAN:
    printf("%s: %d\n", entry->key, *((int *)entry->value));
    break;
  }
}

Entry *__Json_FindEntryByKey(Json *json, const char *key)
{
  if (json == NULL)
  {
    return NULL;
  }

  Entry *entry = json->root;
  while (entry != NULL)
  {
    if (strcmp(entry->key, key) == 0)
    {
      return entry;
    }
    entry = entry->next;
  }

  return NULL;
}

void Json_Delete(Json *json)
{
  if (json == NULL)
  {
    return;
  }

  __Json_EntryDelete(json->root);
  json->root = json->tail = NULL;
  json->total_entries = 0;
  json->TYPE = -1;

  free(json);
}
void __Json_EntryDelete(Entry *entry)
{
  if (entry == NULL)
  {
    return;
  }

  //! free() cannot be run twice on the same pointer or it will cause error
  entry->type == JSON ? Json_Delete(entry->value) : free(entry->value);
  entry->value = NULL;

  __Json_EntryDelete(entry->next);
  entry->next = NULL;

  free((void *)entry->key);
  entry->key = NULL;

  free(entry);
}

void __Json_Set(Json *json, ElementValueType type, const char *key, void *value)
{
  Entry *entry = __Json_FindEntryByKey(json, key);
  if (entry != NULL)
  {
    printf("Existing key: %s\n", key);
    entry->type == JSON ? Json_Delete(entry->value) : free(entry->value);

    entry->value = value;
    entry->type = type;
    return;
  }

  json->total_entries += 1;
  entry = __Json_NewEntry(type, __Json_Util_CopyString(key), value);
  // printf("New entry[type=%s, key=%s, value=%s]\n", ELEMENT_VALUE_TYPE_STRING[entry->type], entry->key, (char *)entry->value);
  if (json->root == NULL)
  {
    json->root = json->tail = entry;
    return;
  }

  json->tail = json->tail->next = entry;
}
void Json_SetString(Json *json, const char *key, const char *value)
{
  __Json_Set(json, STRING, key, __Json_Util_CopyString(value));
}
void Json_SetNumber(Json *json, const char *key, double value)
{
  double *v = malloc(sizeof(double));
  *v = value;

  __Json_Set(json, NUMBER, key, v);
}
void Json_SetBoolean(Json *json, const char *key, int value)
{
  int *v = malloc(sizeof(int));
  *v = value;

  __Json_Set(json, BOOLEAN, key, v);
}
void Json_SetNested(Json *json, const char *key, Json *value)
{
  __Json_Set(json, JSON, key, value);
}
void __Json_AppendArray(Json *json, ElementValueType type, void *value)
{
  char key[8] = {0};
  sprintf(key, "%u", json->total_entries);
  __Json_Set(json, type, key, value);
}
void Json_AppendArrayAsString(Json *json, const char *value)
{
  __Json_AppendArray(json, STRING, __Json_Util_CopyString(value));
}
void Json_AppendArrayAsNumber(Json *json, double value)
{
  double *v = malloc(sizeof(double));
  *v = value;
  __Json_AppendArray(json, NUMBER, v);
}
void Json_AppendArrayAsBoolean(Json *json, int value)
{
  int *v = malloc(sizeof(int));
  *v = value;
  __Json_AppendArray(json, NUMBER, v);
}
void Json_AppendArrayAsNested(Json *json, Json *value)
{
  __Json_AppendArray(json, JSON, value);
}

Entry *Json_Get(Json *json, const char *key)
{
  if (json == NULL || json->root == NULL)
  {
    return NULL;
  }

  char **paths = __Json_Util_SplitString(key, ".");

  const char *current_key = NULL;
  Entry *entry = NULL;
  Json *current_json = json;
  for (int i = 0; paths[i] != NULL; i++)
  {
    const char *current_key = paths[i];
    entry = __Json_FindEntryByKey(current_json, current_key);
    if (entry == NULL)
    {
      __Json_Util_DeleteArrayString(paths);
      return NULL;
    }

    // if entry is not JSON, then it is what we're looking
    if (entry->type != JSON)
    {
      break;
    }

    current_json = entry->value;
  }

  __Json_Util_DeleteArrayString(paths);
  return entry->value;
}

int __Json_IsWhiteSpace(char ch)
{
  return ch == ' ' || ch == '\n';
}

void __ExitIf(int condition, const char *message)
{
  if (condition)
  {
    fprintf(stderr, message);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
  }
}

char *__Json_ParseObject(const char *json)
{
  __ExitIf(json[0] != '{', "Invalid json object!");

  // traverese key
}
char *__Json_ParseArray(const char *json)
{
}

char *Json_ParseFromString(const char *json)
{
  int invalid_json = json[0] != '{' || json[0] != '[';
  __ExitIf(invalid_json, "Json is invalid!");

  int is_object = json[0] == '{';
  return is_object ? __Json_ParseObject(json) : __Json_ParseArray(json);
}

char *__Json_Util_CopyString(const char *source)
{
  char *copied = malloc(strlen(source) + 1); // +1 for '\0' character
  strcpy(copied, source);
  return copied;
}
char *Json_Stringify(Json *json)
{
  if (json == NULL)
  {
    return NULL;
  }

  const char *comma = ",\n";
  Entry *entry = json->root;

  int size = 1;
  char *overall = malloc(sizeof(char) * size);
  memset(overall, 0, size);

  while (entry != NULL)
  {
    const char *entry_string = __Json_StringifyEntry(entry, json->TYPE);

    int size = strlen(overall) + strlen(entry_string) + strlen(comma) + 1; // +1 for "\0"
    char *new_result = malloc(sizeof(char) * size);

    memset(new_result, 0, size);
    snprintf(new_result, size, "%s%s%s", overall, entry_string, comma);

    free(overall);
    free((void *)entry_string);

    overall = new_result;
    entry = entry->next;
  }

  // remove comma at the last element
  int overall_len = strlen(overall);
  overall[overall_len - strlen(comma)] = ' ';

  size = overall_len + 2 + 1; // +2 for brackets, +1 for NULL Terminator
  char *end_result = malloc(sizeof(char) * size);
  json->TYPE == OBJECT
      ? snprintf(end_result, size, "{%s}", overall)
      : snprintf(end_result, size, "[%s]", overall);

  free(overall);
  return end_result;
}
char *__Json_StringifyEntry(Entry *entry, JsonType context)
{
  int key_len = context == OBJECT ? strlen(entry->key) : 0;

  if (entry->type == STRING)
  {
    const char *decorator = context == OBJECT ? "\"\": \"\"" : "\"\"";
    const char *value = (char *)entry->value;

    int size = key_len + strlen(decorator) + strlen(value) + 1;
    char *data = malloc(sizeof(char) * size);

    memset(data, 0, size);
    context == OBJECT
        ? snprintf(data, size, "\"%s\": \"%s\"", entry->key, value)
        : snprintf(data, size, "\"%s\"", value);

    return data;
  }

  const char *decorator = context == OBJECT ? "\"\": " : "";

  if (entry->type == NUMBER)
  {
    const double *value = (double *)entry->value;
    const int INT_STR_SIZE = 32;

    int size = key_len + INT_STR_SIZE + strlen(decorator) + 1;
    char *data = malloc(sizeof(char) * size);

    memset(data, 0, size);
    context == OBJECT
        ? snprintf(data, size, "\"%s\": %.3f", entry->key, *value)
        : snprintf(data, size, "%.3f", *value);
    return data;
  }

  if (entry->type == BOOLEAN)
  {
    const int *value = (int *)entry->value;
    const char *stringify_value = *value ? "true" : "false";

    int size = key_len + strlen(stringify_value) + strlen(decorator) + 1;
    char *data = malloc(sizeof(char) * size);

    memset(data, 0, size);
    context == OBJECT
        ? snprintf(data, size, "\"%s\": %s", entry->key, stringify_value)
        : snprintf(data, size, "%s", stringify_value);
    return data;
  }

  if (entry->type == JSON)
  {
    const char *str_value = Json_Stringify(entry->value);

    int size = key_len + strlen(str_value) + strlen(decorator) + 1;
    char *data = malloc(sizeof(char) * size);

    memset(data, 0, size);
    context == OBJECT
        ? snprintf(data, size, "\"%s\": %s", entry->key, str_value)
        : snprintf(data, size, "%s", str_value);

    free((void *)str_value);

    return data;
  }

  printf("[ERROR]: Unknown type of entry: %s\n", ELEMENT_VALUE_TYPE_STRING[entry->type]);
  exit(EXIT_FAILURE);
}
char **__Json_Util_SplitString(const char *str, const char *delimiter)
{
  int size = 0;
  char *to_free, *copied_str, *token = NULL;
  to_free = copied_str = strdup(str);
  __Json_Util_ExitIfAllocationFailed(to_free);

  while ((token = strsep(&copied_str, delimiter)) != NULL)
  {
    size++;
  }

  char **container = malloc(sizeof(char *) * (size + 1));
  __Json_Util_ExitIfAllocationFailed(container);

  size = 0;
  copied_str = to_free;
  while ((token = strsep(&copied_str, delimiter)) != NULL)
  {
    container[size++] = strdup(token);
    __Json_Util_ExitIfAllocationFailed(container[size - 1]);
  }

  free(to_free);
  return container;
}
void __Json_Util_DeleteArrayString(char **container)
{
  if (container == NULL)
  {
    return;
  }
  for (int i = 0; container[i] != NULL; i++)
  {
    free(container[i]);
  }
  free(container);
}
void __Json_Util_ExitIfAllocationFailed(void *p)

{
  if (p == NULL)
  {
    fprintf(stderr, "[Error] Memory allocation failed!\n");
    exit(EXIT_FAILURE);
  }
}