#!/usr/bin/env bash

openssl genrsa -out ./build/server.key 2048
openssl req -new -key ./build/server.key -out ./build/server.csr
openssl x509 -req -days 365 -in ./build/server.csr -signkey ./build/server.key -out ./build/server.crt

cp ./build/server.key ./app/
cp ./build/server.crt ./app/