#!/bin/bash
#.# /etc/hosts
host=${1}

sudo -u root scp ../packages/hosts/hosts $host:/etc
