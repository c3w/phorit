MAILTO="me@domain.tld"
DIALOG=${DIALOG=dialog}
tempfile=`mktemp 2>/dev/null` || tempfile=/tmp/phorit$$
trap "rm -f $tempfile" 0 1 2 5 15
