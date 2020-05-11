## P2

### The source file is P2.ipynb . This writeup will be refering to chapters from the source file like 2.b), 3.a) ...

---

**Advanced Lane Finding Project**

The goals / steps of this project are the following:

* Compute the camera calibration matrix and distortion coefficients given a set of chessboard images.
* Apply a distortion correction to raw images.
* Use color transforms, gradients, etc., to create a thresholded binary image.
* Apply a perspective transform to rectify binary image ("birds-eye view").
* Detect lane pixels and fit to find the lane boundary.
* Determine the curvature of the lane and vehicle position with respect to center.
* Warp the detected lane boundaries back onto the original image.
* Output visual display of the lane boundaries and numerical estimation of lane curvature and vehicle position.

[//]: # (Image References)

[image1]: ./output_images/chessCorners/2b_chessCornerscalibration10.jpg "ChessCorners"
[image2]: ./output_images/3a_chessDist.jpg "OriginalImage"
[image3]: ./output_images/3a_chessUndist.jpg "UndistortedImage"
[image4]: ./output_images/3a_undist.jpg "Undistorted"
[image5]: ./output_images/4a_s_channel.jpg "S-Channel"
[image6]: ./output_images/4b_s_binary.jpg "S-Channel binary"
[image7]: ./output_images/4e_combined_binary.jpg "S and Gray"
[image8]: ./output_images/5a_transRegion.jpg "Trapezoid"
[image9]: ./output_images/5b_undistWarped.jpg "Warped"
[image10]: ./output_images/6d_slidingWindow.jpg "Polynomial"
[image11]: ./output_images/example.jpg "Final"
[image12]: ./output_images/debug1.jpg "debug video"

[video1]: ./project_video_full.mp4 "Video"

## [Rubric](https://review.udacity.com/#!/rubrics/571/view) Points

### Here I will consider the rubric points individually and describe how I addressed each point in my implementation.  

---

### Writeup / README

#### 1. Provide a Writeup / README that includes all the rubric points and how you addressed each one.  You can submit your writeup as markdown or pdf.  [Here](https://github.com/udacity/CarND-Advanced-Lane-Lines/blob/master/writeup_template.md) is a template writeup for this project you can use as a guide and a starting point.  

You're reading it!

### Camera Calibration

#### 1. Briefly state how you computed the camera matrix and distortion coefficients. Provide an example of a distortion corrected calibration image.

For camera calibration a set of 20 images of a chess board was used. The chess board has 9 * 6 corners. 2 sets of object and image points are required for camera calibration. Image points are found using OpenCV function findChessboardCorners(). When the corners are found they are draw on the chess board image like in the example:

![alt text][image1]

Now having image and object points the camera is calibrated with the OpenCV function calibrateCamera(). Using the calibration matrix and distortion coeficient I can now apply the cv2.undistort() to an image and obtain an undistorted image:

Distorted
![alt text][image2] 
Undistorted
![alt text][image3]

### Pipeline (single images)

#### 1. Provide an example of a distortion-corrected image.

Undistored image:
![alt text][image4]

#### 2. Describe how (and identify where in your code) you used color transforms, gradients or other methods to create a thresholded binary image.  Provide an example of a binary image result.

I used a combination of color and gradient thresholds to generate a binary image. I extracted the S-Channel from and HLS image at point 4.a) :
![alt text][image5]
Then I applied an binary threshold at point 4.b) resulting in:
![alt text][image6]
For the same image in grayscale I applied OpenCV Sobel() function and binary threshold at point 4.c) and 4.d). After that I combined the saturation and gray channels binary thresholds resulting in the image:
![alt text][image7]

After testing this I combined all the code above in one function at 4.f) to be used in the video pipeline later.

#### 3. Describe how (and identify where in your code) you performed a perspective transform and provide an example of a transformed image.

For the perspective transformation I wanted to switch the perspective from frontal perspective to top perspective. For this I hand picked 4 points on the test_images/straight_lines2.jpg image. Then I picked another 4 points so that the undistorted and warped perspective match the 1280 * 720 image.

This resulted in the following source and destination points:

| Source        | Destination   | 
|:-------------:|:-------------:| 
| 291, 661      | 265, 720      | 
| 571, 465      | 265, 0        |
| 1030, 661     | 1065, 720     |
| 716, 465      | 1065, 0       |

At point 5.a) I draw a trapezoid on the image coresponding to the Source points.
![alt text][image8]

I then calculated the perspective transform and the inverse perspective transform, applied the undistort and warped the perspective. All these have been done in point 5.b) and using OpenCV functions. The result is bellow:

![alt text][image9]

#### 4. Describe how (and identify where in your code) you identified lane-line pixels and fit their positions with a polynomial?

I calculated a histogram for the first y half of the image at point 6.a) to find maximums for the left and right lines Then using this I found the pixels belonging to each line through the sliding window method(function find_lane_pixels() from point 6.b)). Here I tunned the HYPERPARAMETERS to get the best results. Also in find_lane_pixels() I imposed a minimum width between the lane lines and allowed for a margin. For each frame, except the first frame, I added I margin to where the lines should be searched for. So once the first frame determined the lane lines the condition for the next ones is determined by:

```
if (rightx_base-leftx_base) > minimumWidth:
            lxl = leftx_base - windowmargin
            lxr = leftx_base + windowmargin

            rxl = rightx_base - windowmargin
            rxr = rightx_base + windowmargin

        leftx_base = np.argmax(histogram[lxl:lxr]) + lxl
        rightx_base = np.argmax(histogram[rxl:rxr]) + rxl
```

Next I fitted a polynomial line to the 2 lane lines in point 6.c) . I tested this in point 6.c):


![alt text][image10]

#### 5. Describe how (and identify where in your code) you calculated the radius of curvature of the lane and the position of the vehicle with respect to center.

I calculated the curvature in function measure_curvature_real() at point 7.a). I did the translation from pixels to meters based on Lesson 7: Measuring curvature. At point 7.b) I calculated the shift in meters from the lane center of the vehicle. In function calculateShift() I considered the camera to be perfectly centered to the middle of the vehicle so that the image width / 2 is the camera point. Now having the position of the lane lines from find_lane_pixels() I can calculate the distance in x axis from the image margins. I then substracted from the image width the lane width to calculate how much of a margin must be to right and to the left so that the camera is centered to the lane. Knowing this I can now see how much the deviation is to the left or to the right.

#### 6. Provide an example image of your result plotted back down onto the road such that the lane area is identified clearly.

Here is an example of my result on a test image:

![alt text][image11]

---

### Pipeline (video)

#### 1. Provide a link to your final video output.  Your pipeline should perform reasonably well on the entire project video (wobbly lines are ok but no catastrophic failures that would cause the car to drive off the road!).


Here's a [link to my video result](./project_video_full.mp4)

---

### Discussion

#### 1. Briefly discuss any problems / issues you faced in your implementation of this project.  Where will your pipeline likely fail?  What could you do to make it more robust?

1. The pipeline has issues when detecting lines since the warped image display the line fuzzy at a small y. This is due to the the resolution being poor the further the image is from the camera. Making the trapezoid smaller could improve this. 

2. Other color spaces should be explored to get better lines and avoid confusion with road shades.

3. I've done a lot of "debug" to see how the histogram apears over an video to tune the hyperparameters and to do modifications to find_lane_pixels()

![alt text][image12]
All the video experiments can be found in ./TestingArea

4. The challenge_video.mp4 confuses the pipeline with the direction separation wall, as it casts a shadow that creates high contrast zone near the left lane line. This is something that makes me think that a white and yellow based color transformation is needed.