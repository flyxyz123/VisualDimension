This program is made public for demonstration and archive purposes. It is **very unfinished**. It involves a lot of hacks and workarounds. It completely ignores many problems encountered. **So don't even try to read it, it is a waste of time.**

An **unfinished** personal project of vision system (computer/machine vision) metrology software I wrote years ago (November 2017). It uses [OpenCV](https://github.com/opencv/opencv) to measure the machine part's dimensions. I tested it a little bit on some CAD pictures on Windows operating system. I gave up midway due to laziness and lack of knowledge. I was in the process of rewriting the software at that time, so probably **won't work**. It probably won't compile nowadays.

If I remember correctly, the process requires reference objects that have known dimensions. I was using a circle and a polygon. First, the program [thresholding](https://en.wikipedia.org/wiki/Thresholding_(image_processing)) the picture. Then, it finds the contour. Finally, it measures the dimensions based on the contour data.

For example, if we know the dimensions of the reference object and a straight line on the reference object's contour is 1 cm, if a straight line on the target part's contour is 2 times the length of the straight line on my reference object's contour, I can get the target part's straight line is 1x2=2cm. The same also applies to a circle's radius, diagonal line, arc, etc.

In `backup` directory, there are some older source codes files. At that time, I was not using git because I was not fluent and was too afraid to use git.
