#!/bin/bash
#.# ssh-keys
host=${1}

# this will overwrite any root ssh keys!
sudo -u root ssh $host "mkdir ~/.ssh;chmod og-rwx ~/.ssh;"
# COMMENTED OUT - PUT YOUR REAL KEY IN THE DIR LISTED BELOW
#sudo -u root scp ../packages/sshkeys/mykey.pub $host:~/.ssh/authorized_keys
