# SVG Maze Generator

This program generates pseudorandom rectangular mazes and outputs the result
as an SVG image.

## Usage:

```
svgmaze [Options]
 -w<n>   Width of Maze (in columns)
 -h<n>   Height of Maze (in rows)
 -c<n>   Width of corridor in pixels (SVG Output)
 -p<n>   Pen radius in pixels (SVG Output)
 -o<fmt> Output format (svg|ascii) (Default: ASCII)
 -r<s>   Random seed as a string (spaces must be quoted)
```

Output will be to stdout.
