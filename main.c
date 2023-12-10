#include <stdio.h>
#include <stdlib.h>

#include "json/json.h"

int main()
{

  printf("\nRunning...\n");
  printf("----------\n");

  Json *riza = Json_NewObject();
  Json_SetString(riza, "name", "Riza Dwi");
  Json_SetString(riza, "password", "12345678");
  Json_SetString(riza, "password", "abcdefg");

  Json *college = Json_NewObject();
  Json_SetString(college, "name", "ITS");
  Json_SetNumber(college, "gpa", 3.68);

  Json_SetNested(riza, "college", college);

  Json *hobbies = Json_NewArray();
  Json_AppendArrayAsString(hobbies, "Music");
  Json_AppendArrayAsString(hobbies, "Coding");
  Json_AppendArrayAsString(hobbies, "Travel");

  Json_SetNested(riza, "hobbies", hobbies);

  Json_SetNumber(riza, "age", 22);
  Json_SetBoolean(riza, "is graduated", 1);

  printf("----------\n");
  Json_Print(riza);
  char *json_string = Json_Stringify(riza);

  FILE *file = fopen("ignore/file-c.json", "w");
  fputs(json_string, file);
  fclose(file);

  free(json_string);
  Json_Delete(riza);

  return 0;
}
