# cite

A static site generator in ANSI C.

Inspired by using Hugo and seeing [100r's
website][100r]. Written from scratch, just
for the fun!

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

The [Github Pages][gp] site was made with cite, and its source can be found in
docs.

[gp]: https://hannah-scott.github.io/cite
