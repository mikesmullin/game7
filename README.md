# Game 7

This prototype explores 8-bit/retro style gaming.

## Building

### on Windows
Start the game.
```
C:\> node build_scripts/Makefile.mjs all
```
Enable hot-reload. (ie. when editing [`Logic.c`](src/game/Logic.c))
```
C:\> node build_scripts/Makefile.mjs watch
```

## Inspirations

- Markus Persson ([@notch](https://x.com/notch))
   - **(Persson 2021)** *Prelude of the Chambered* - LD48:
      [playthrough](https://www.youtube.com/watch?v=RE3dRh4wMc8) |
      [timelapse](https://www.youtube.com/watch?v=IoR-G8Ud0JM) | 
      [livestream](https://www.youtube.com/watch?v=GQO3SSlsgJM) |
      [code](https://github.com/skeeto/Prelude-of-the-Chambered) |
      [host](https://s3.amazonaws.com/ld48/index.html)

## References

- [ChatGPT Analysis](docs/90s-raycasting.md): No worries brah. GPT can ELI5 Notch's crypticly-abbreviated and 100% uncommented code, probably better than the author himself.
- [Permadi’s Raycasting Tutorial](https://permadi.com/1996/05/ray-casting-tutorial-table-of-contents/): This classic resource provides a detailed step-by-step guide to raycasting, including wall detection, fisheye distortion correction, and basic shading techniques. It's accompanied by demos and interactive examples to help visualize the concepts, especially for beginners.
- [Tim Wheeler’s Wolfenstein 3D Raycasting Tutorial in C](https://timallanwheeler.com/blog/2023/04/01/wolfenstein-3d-raycasting-in-c/): This blog post dives into the math and code behind raycasting in C, offering explanations on how to compute ray directions, handle wall intersections, and render columns of pixels efficiently. It’s a good resource for programmers working in C.
- [Gabriel Gambetta’s 2.5D Engines and Raycasting Tutorial](https://gabrielgambetta.com/computer-graphics-from-scratch/02-basic-raytracing.html): A detailed modern take on the subject, explaining how to implement 2.5D rendering using raycasting. It covers everything from ray generation to texture mapping and optimizing the rendering process for smooth performance.
- C String Manipulation
  - https://www.youtube.com/watch?v=2wio9UOFcow
- C Arena Memory Management
   - https://www.youtube.com/watch?v=TZ5a3gCCZYo
   - https://www.youtube.com/watch?v=3IAlJSIjvH0
   - https://www.youtube.com/watch?v=Tc763vPm4Ws