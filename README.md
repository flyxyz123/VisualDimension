This program is made public for demonstration and archive purpose. It is **very unfinished**. It involves a lot of hacks and workaround. It completely ignores many problems encountered. **So don't even try to read it, it is a waste of time.**

An **unfinished** personal project of vision system (computer/machine vision) metrology software I wrote years ago (November 2017). It uses [OpenCV](https://github.com/opencv/opencv) to measure machine parts dimensions. Tested a little bit on some CAD pictures on Windows operating system. Gave up midway due to laziness and lack of knowledge. Was in the process of rewriting the software at that time, so probably **won't work**. Probably won't compile nowadays.

If I remember correctly, the process requires reference objects that have known dimensions. I was using a circle and a polygon. First the program [thresholding](https://en.wikipedia.org/wiki/Thresholding_(image_processing)) the picture. Then it finds the contours. Finally, it measures the dimensions based on the contours data.

For example, if we know dimensions of the reference object and a straight line on reference object is 1 cm, if a straight line on the target part's contour is 2 times the length of the straight line on my reference object's contour, I can get the target part's straight line is 1x2=2cm. Same also applies to circle's radius, diagonal line, arc, etc.

I think either main.c or main.c.bak is in the process of rewriting. Because it is wrote years ago, I can't remember which one, main.c.bak is more likely in the rewriting process. At that time, I was not using git because I was not fluent and was too afraid to use git.
