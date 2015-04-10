#compdef mastermindsdl mastermindcli
_arguments  \
	{-s,--set=}"[Mastermind settings]:settings of mastermind:((\
		remise\:'color repeated only once on combination' \
		colors\:'number of colors' \
		guesses\:'number of guesses' \
		holes\:'number of holes' \
		account\:'account name' \
		save_on_exit\:'save session on exit' \
		save_on_play\:'save session on every played guess' \
		))" \
	{-a,--account=}"[session account]" \
	{-c,--score}"[top scores]" \
	{-v,--version}"[Mastermind version]" \
	{-h,--help}"[show help message]"
#	"*:options:(-s -c -h -a -v --set --score --help --account --version)"
return 0
