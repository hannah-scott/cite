# cite

A static site generator in ANSI C.

Inspired by using Hugo and seeing [100r's
website][100r]. Written from scratch, just
for fun!

[100r]: https://github.com/hundredrabbits/100r.co 

## Use

Edit `config.h` so it matches your environment, then rebuild and run:

```
$ make clean cite
$ ./cite
```

cite works well in a build script with a tool like markdown or Pandoc. This is
how I'm building my personal site right now.

## Example

My personal website [hddn][hddn] is made with cite and discount Markdown. The
site is [open-source][repo] and contains my config file and build script.

[hddn]: https://hannahs.ddns.net
[repo]: https://github.com/hannah-scott/hannahs.ddns.net
