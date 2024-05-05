CodeCritic er en hjemmeside, hvor man kan se og lave forskellige programmeringsopgaver.
Alle kan se alle opgaver, og alle kan se et leaderboard til hver opgave.
Under hver opgave kan man vælge et sprog at programmere i (lige nu er der kun mulighed for C++ og C#)
Herefter kan man uploade kode, som skal kunne løse opgaven. Koden bliver compiled og testet af serveren på forskellige test cases,
hvorefter resultatet af testen bliver gemt i en sqlite3 database, som vises på leaderboardet.
Koden bliver vurderet på antal point for selve løsningen af opgaven, og på tiden det tager koden at løse opgaven.
Hermed motiveres mere effektive løsninger, for at komme øverst på leaderboardet.

Åbn build/CodeCritic.exe for at starte serveren, og tilslut på en browser (default via 127.0.0.1)

Submissions:
For at submissions virker med:
- C++, skal MinGW være installeret med Basic Setup -> mingw32-gcc-g++
- C#, skal visual studio community være installeret med C#

build/config.txt:
- IP til serveren kan sættes på første linje
- PORT til serveren kan sættes på anden linje
- max queue til http serveren kan sættes på tredje linje
