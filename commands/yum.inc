#!/bin/bash
#.# yumupdate
host=${1}

sudo -u root ssh $host "yum -y update"
