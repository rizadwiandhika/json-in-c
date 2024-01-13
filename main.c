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
  Json_SetNumber(riza, "age", 22);
  Json_SetBoolean(riza, "is_graduated", 1);

  Json *occupation = Json_NewObject();
  Json_SetString(occupation, "company", "Mandiri");
  Json_SetBoolean(occupation, "is_active", 1);

  Json *hobbies = Json_NewArray();
  Json_AppendArrayAsString(hobbies, "Music");
  Json_AppendArrayAsString(hobbies, "Coding");
  Json_AppendArrayAsString(hobbies, "Travel");
  Json *swiming = Json_NewObject();
  Json_SetString(swiming, "style", "frog");
  Json_AppendArrayAsNested(hobbies, swiming);

  Json *college = Json_NewObject();
  Json_SetString(college, "name", "Institut Teknologi Sepuluh Nopember");
  Json_SetString(college, "major", "Informatics");
  Json_SetNumber(college, "gpa", 3.68);
  Json_SetBoolean(college, "is_active", 0);
  Json *highscool = Json_NewObject();
  Json_SetString(highscool, "name", "SMAN 1 Depok");
  Json_SetString(highscool, "major", "Science");
  Json_SetBoolean(highscool, "is_active", 0);
  Json *middleschool = Json_NewObject();
  Json_SetString(middleschool, "name", "SMPN 4 Depok");
  Json_SetBoolean(middleschool, "is_active", 0);

  Json *academics = Json_NewArray();
  Json_AppendArrayAsNested(academics, college);
  Json_AppendArrayAsNested(academics, highscool);
  Json_AppendArrayAsNested(academics, middleschool);

  Json_SetNested(riza, "occupation", occupation);
  Json_SetNested(riza, "hobbies", hobbies);
  Json_SetNested(riza, "academics", academics);

  printf("----------\n");
  Json_Print(riza);
  char *json_string = Json_Stringify(riza);

  FILE *file = fopen("ignore/file-c.json", "w");
  fputs(json_string, file);
  fclose(file);

  const char *key = "name";
  printf("");

  free(json_string);
  Json_Delete(riza);

  return 0;
}
