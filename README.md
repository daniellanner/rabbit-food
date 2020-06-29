# foliage-texture-fill
(very WIP)

An implementation of Sean Feeley's mip chains flood-filling from his 2019 GDC talk [Interactive Wind and Vegetation in 'God of War'](https://schedule2019.gdconf.com/session/interactive-wind-and-vegetation-in-god-of-war/860472) which I was lucky enough to attend.

Foliage texture dilation is used to avoid color bleeding for foliage alpha cut-outs when game engines generate mip maps. Traditionally this was done by blurring the image a couple of times. The proposed solution is faster and has a higher compression rate.
