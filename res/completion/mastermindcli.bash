_complete() {
	opts='-s -c -h -a -v --set --score --help --account --version'
	curopt="${COMP_WORDS[$COMP_CWORD]}"
	prevopt="${COMP_WORDS[$((COMP_CWORD - 1))]}"
	args=""

	case "$prevopt" in
		'-s')
			args="remise save_on_exit save_on_play account colors holes guesses";;
		*)
			;;
	esac

	COMPREPLY=( $(compgen -W "$opts $args" -- ${COMP_WORDS[$COMP_CWORD]}) )
	return 0
}

complete -F _complete mastermindsdl mastermindcli
