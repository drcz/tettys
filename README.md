# tettys
A curses-based tetris clone found on a dusty backup disk, from the C course I conducted @ Technical University of Gdansk, 2007/2008.

The commensts are in Polish; the first one says it's public domain.

Does not calculate the score (try to add it, it should be ((n+1)n)/2 points for each n lines deleted).

there's one tetrino for each day of the week, you know.


To compile on linux:

$ gcc tettys.c -o tettys -lncurses

(requires [n]curses library)
