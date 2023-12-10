#!/bin/bash

set -xe

clang -Wall -Wextra main.c json/json.c -o ./out/app
