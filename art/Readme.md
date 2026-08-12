Term size should be 100x12

The tool `asciinema` was used to make the terminal recordings. To make all the themes:

```bash
for theme in asciinema dracula github-dark github-light kanagawa kanagawa-dragon kanagawa-light monokai nord solarized-dark solarized-light gruvbox-dark ; do agg --theme=$theme art/saltcube.cast art/gif/$theme-saltcube.gif ; done
```

To perform the typing `ydotool` was used.

```bash
sleep 3 ; ydotool type -D 800 -d 50 -e 1 "saltcube pw-add\n" "desert0n1pX\n" "Correct Horse Battery Staple\n"  ; sleep 4 ; ydotool type -D 800 -d 50 -e 1 "saltcube\n" "\n" "Correct Horse Battery Staple\n" ; sleep 4 ; ydotool type -D 800 -d 50 -e 1 "remind\n" "Correct Horse Battery Staple\n" "exit\n"
```