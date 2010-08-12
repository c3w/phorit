#!/bin/sh

function rollSystem() {
SCRIPT=${1}
PRODUCT=${2}
REVISION=${3}
$DIALOG \
	--title "${PRODUCT} revision ${REVISION}" --clear \
        --msgbox "
$(sudo -u deploy ./${SCRIPT} ${REVISION})
" 20 61
}

function rollWindow() {
COMMAND="${1}"
$DIALOG \
	--title "${COMMAND}" --clear \
        --msgbox "\n
$(${COMMAND})
" 20 61
}

function stubSelect {
$DIALOG \
	--title "${1} revision" --clear \
        --checklist "select your ${1} revision for ${AREA}" 20 61 5  \
	`grep -H -e "#\.*#" *|sed -e "s/\([^:]*\):#\.# \(.*\)/\1 \"\2\" off/"` \
	2> $tempfile

retval=$?
case $retval in
  0)
        return;;
  1)
    echo "Cancelled"; exit 2;;
  255)
    echo "Aborted"; exit 2;;
esac

}

function sysSelect() {

$DIALOG \
        --title "${1} revision" --clear \
        --checklist "select ${1} ${AREA}" 20 61 5  \
        $(if [ -f ${tempfile}.hints ] && [ "${_toggle}" = "hint" ]; then { cat ${tempfile}.hints; }; fi) \
	$(echo "--") \
	$(cat /etc/hosts|grep -e "^[0-9]"|awk '{print $2,$2,ENVIRON["_toggle"]}') \
        2> $tempfile

retval=$?
case $retval in
  0)
        return;;
  1)
    echo "Cancelled"; exit 2;;
  255)
    echo "Aborted"; exit 2;;
esac

}

function execConfirm() {
while [ ${1} != "--" ]; do {
	SYS="${SYS} ${1}"; shift
}; done
shift
while [ ${1} ]; do {
	HOSTS="${HOSTS} ${1}"; shift;
}; done

for system in ${SYS}; do {
if $(grep "c3w " ${system}>/dev/null); then {
RM_TEXT=$(grep "c3w " ${system})
WARNINGS="${WARNINGS} // ***** WARNING***** 'rm' found in ${system}: ${RM_TEXT}"
}; fi
}; done

$DIALOG --title "CONFIRMATION" --clear \
        --yesno "PRESS ENTER TO CONFIRM OR CTRL-C to EXIT\n \
	Please Confirm your system rollout\n \
	for ${HOSTS}\n \
	\n \
	EXEC ${SYS} \
	\
	${WARNINGS} \
	" 20 70

echo "PHORIT for ${HOSTS} ***EXEC*** ${SYS}" > /tmp/phorit.out

return $?
}

function execSys() {
while [ ${1} != "--" ]; do {
	SYS="${SYS} ${1}"; shift
}; done
shift
while [ ${1} ]; do {
	HOSTS="${HOSTS} ${1}"; shift;
}; done
	
	#rm -f /tmp/phorit.out; touch /tmp/phorit.out

	for host in ${HOSTS}; do { 
		for sys in ${SYS}; do { 
		echo ${host} >> /tmp/phorit.out
		eval ./${sys} ${host} | tee -a /tmp/phorit.out 2>&1
		}; done 
	}; done 

$DIALOG \
	--title "execSys" --clear \
        --textbox /tmp/phorit.out 20 61

}

function execNotify() {
file=${1}
mail -s "PHORIT Notification" ${MAILTO} <<EOF
$(cat /tmp/phorit.out)
EOF
}

case ${1} in
  stub)
	exit;;
  servers)
	FUNC=""
	cd commands
	stubSelect
	STUB=$(cat ${tempfile})
	HINT=$(cat ${tempfile}|sed -e "s/\"//g;s/\.inc//")
	if [ -f ../hints/${HINT}.hint ]; then {
		cat ../hints/${HINT}.hint | awk '{print $1,$1,"on"}' > $tempfile.hints
	}; fi
	sysSelect
	SYSTEMS=$(cat ${tempfile})
	if ( execConfirm ${STUB} -- ${SYSTEMS} ) ; then {
		execSys ${STUB} -- ${SYSTEMS}
		execNotify /tmp/phorit.out
	}; fi
esac

exit;
