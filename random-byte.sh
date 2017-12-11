#!/bin/bash

echo $(dd if=/dev/random bs=1 count=1 status=none)

