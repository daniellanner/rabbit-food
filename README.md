![](rabbit-food-header.png)

## Introduction

An implementation of Sean Feeley's mip chains texture flood-filling approach from his 2019 GDC talk [Interactive Wind and Vegetation in 'God of War'](https://schedule2019.gdconf.com/session/interactive-wind-and-vegetation-in-god-of-war/860472) which I was lucky enough to attend.

Foliage texture dilation is used to avoid color bleeding for foliage alpha cut-outs when game engines generate mip maps. Traditionally this was done by blurring the image a couple of times. The proposed solution is computational faster and has a higher compression rate.

## Examples

![](bg.jpg)

Example of exported foliage saved as png from photoshop with alpha made visible on the left, same png after being processed by rabbit fill on the right.

![](alpha.jpg)

Loaded into Unity engine as a alpha cutout texture. See the white edges of the left image. Filling the alpha channel with the correct colors prevents color bleeding.

![](mip4.jpg)

Loaded into Unity engine and creating automatic mip maps. See the colors on the edges bleeding into white on the left image.



## Usage

### Drag and Drop

Drag and drop a .png file onto the .exe application. A new .png with the original name and the suffix "_fill" will be created in the same directory as the original file.



### Commandline

Commandline arguments:

rabbit-food.exe <inputpath> <outputpath> <optional arguments>

-h, help:	(bool) print help

-v, verbose:	(bool) print status while computing

-r, resize:	(argv) "smaller", "nearest" or "larger" determines the behaviour if the imported image is non-power of two

-o, overwrite:	(bool) if the output path already exists, overwrite file or find next incremental name



## Limitations

Power of 2