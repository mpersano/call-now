#!/bin/sh
tail -n+4 $0 | gunzip -c - > /tmp/intro; chmod a+x /tmp/intro; /tmp/intro
exit 0
