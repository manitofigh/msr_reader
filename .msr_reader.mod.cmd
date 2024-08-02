savedcmd_/home/mani/dev/km/msr_reader/msr_reader.mod := printf '%s\n'   msr_reader.o | awk '!x[$$0]++ { print("/home/mani/dev/km/msr_reader/"$$0) }' > /home/mani/dev/km/msr_reader/msr_reader.mod
