Term size should be 100x12

The tool `asciinema` was used to make the terminal recordings. To make all the themes:

```bash
for theme in asciinema dracula github-dark github-light kanagawa kanagawa-dragon kanagawa-light monokai nord solarized-dark solarized-light gruvbox-dark ; do agg --theme=$theme art/saltcube.cast art/gif/$theme-saltcube.gif ; done ; agg art/saltcube.cast art/gif/default-saltcube.gif
```
For the default only, just the last part is needed.

```bash
agg art/saltcube.cast art/gif/default-saltcube.gif
```

To perform the typing `ydotool` was used.

```bash
sleep 3 ; ydotool type -D 800 -d 50 -e 1 "saltcube pw-add\n" "desert0n1pX\n" "Correct Horse Battery Staple\n" ; sleep 4 ; ydotool type -D 800 -d 50 -e 1 "saltcube\n" "\n" "Correct Horse Battery Staple\n" ; sleep 4 ; ydotool type -D 2000 -d 50 -e 1 "remind\n" "shadow\n" "Correct Horse Battery staple" ; sleep 1 ; ydotool key -d 100 14:1 14:0  14:1 14:0  14:1 14:0  14:1 14:0  14:1 14:0  14:1 14:0 ; ydotool type -D 2000 -d 50 -e 1 "Staple\n" "exit\n"
```