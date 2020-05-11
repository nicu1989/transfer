# **Finding Lane Lines on the Road** 

## Overview

### Detect and highlight lane lines from pictures and videos.

---

**Finding Lane Lines on the Road**

The goals / steps of this project are the following:
* Make a pipeline that finds lane lines on the road
* Reflect on my work in a written report

All the results can be found in test_images_output and test_videos_output folders.


---

### Reflection

### 1. Pipeline description

First I made a copy of the image. Then I converted the copy to grayscale. After that I applied a Gaussian blur to remove image noise.

I applied a Canny edge detector algorithm on the whole image. After that I defined a region of interest(ROI) on wich I ran Hough transform to determine the Hough lines.

After the Hough lines are calculated the lines are drawn using the helper function draw_lines(). These lines were then drawn on the original image.

### Draw_lines() modifications

The function now take 2 new parameters: roi_x(region of interest x value) and roi_y(region of interest y value). These 2 new parameters are required in order to know how much
to extend the left and right line segments and also to know the middle of the region of interest.

First thing is to sort the Hough lines coordinates in ascending order, by x1, and create an ordered array of [x1,y1,x2,y2] sets. 

Now we know that the first set in the array represents the bottom-left point of the left line and that the last set in the array is the bottom-right set of the right line.

Since our region of interest is a triangle shape we can now split the array in left line sets and right line sets by the parameter roi_x. 

I then stored only the segments endings and calculated the slopes for the right and left segments. I extended the segments to the picture maximum height and roi_y on the y axis.

I then recalculated the new x's coordinates with the help of the slope and the newly calculated y's.

At the end I drew 2 lines(left and right) with the resultes x's and y's. The use math.isinf() functions is done for rigidity.

### 2. Identify potential shortcomings with your current pipeline


The biggest shortcoming is that the region of interest(ROI) is hardcoded to pixel values. Also other parameters are hardcoded and that make the pipeline rigid.

Another shortcoming is that the middle of the ROI is passed as parameter through the pipeline and makes the pipeline less flexible to further changes.




### 3. Suggest possible improvements to your pipeline

Define ROI to be able to scale to the image. All the hardcoded data should be determined relative to the input image. This would lead to a usability of the pipeline with 
more variate images(enviroments and resolutions).

Make the pipeline as decoupled as possible. The helper functions should be generic and the specific code should be moved in other functions.
