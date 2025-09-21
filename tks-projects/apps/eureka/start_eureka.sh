#!/bin/sh
SELF=$(readlink -f "$0")
cd $(dirname "$SELF")
tks eureka >/tmp/eureka_stdout.txt
