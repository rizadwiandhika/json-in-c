# Pure Json Parser in C! 

No external library is being used. Just pure C code to work with Json data structure.

This project was created only for fun. I was curious on how Json can be manipulated in such a very basic like C.

### How the Json is Representated in C

Basically the json key-value pair is representated as a struct as the following:
```
struct Entry
{
  const char *key;
  void *value;
  Entry *next;
  ElementValueType type;
};
```

- The `Entry *next` attribute here is to store what is the next key-value pair. 
- While the `ElementValueType type` is to tell the data type of the value itself. It can be a STRING, NUMBER, BOOLEAN, or even nested JSON.

### Next Plan

- Create utility function to parse Json string into Json `struct` 
- Do unit the test for the every functions
