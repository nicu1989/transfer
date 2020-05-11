# **Traffic Sign Recognition** 

## Writeup

### In this document I explain how I implemented a Traffic Sign Classifier using LeNet neural network.

---

**Build a Traffic Sign Recognition Project**

The goals / steps of this project are the following:
* Load the data set (see below for links to the project data set)
* Explore, summarize and visualize the data set
* Design, train and test a model architecture
* Use the model to make predictions on new images
* Analyze the softmax probabilities of the new images
* Summarize the results with a written report


[//]: # (Image References)

[image1]: ./images/trainDistr.PNG "Distribution"
[image2]: ./images/34762_original.PNG "Original"
[image3]: ./images/34762_BB.PNG "Zoomed"
[image4]: ./images/34762_BB_gray.PNG "Grayscale"
[image5]: ../GermanSigns/de_12.PNG "Priority road"
[image6]: ../GermanSigns/de_14.PNG "Stop"
[image7]: ../GermanSigns/de_22.PNG "Bumpy road"
[image8]: ../GermanSigns/de_23.PNG "Slippery road"
[image9]: ../GermanSigns/de_28.PNG "Children crossing"
[image10]: ./images/imageViz.PNG "Input image"
[image11]: ./images/conv1Viz.PNG "Convolution 1"
[image12]: ./images/conv2Viz.PNG "Convolution 2"

## Rubric Points
### Here I will consider the [rubric points](https://review.udacity.com/#!/rubrics/481/view) individually and describe how I addressed each point in my implementation.  

---
### Writeup / README

#### 1. Provide a Writeup / README that includes all the rubric points and how you addressed each one. You can submit your writeup as markdown or pdf. You can use this template as a guide for writing the report. The submission includes the project code.

You're reading it! and here is a link to my [project code](./Traffic_Sign_Classifier.ipynb)

### Data Set Summary & Exploration

#### 1. Provide a basic summary of the data set. In the code, the analysis should be done using python, numpy and/or pandas methods rather than hardcoding results manually.

I used the pandas library to calculate summary statistics of the traffic
signs data set:

* The size of training set is 34799
* The size of the validation set is 4410
* The size of test set is 12630
* The shape of a traffic sign image is (32, 32, 3)
* The number of unique classes/labels in the data set is 43

#### 2. Include an exploratory visualization of the dataset.

Here is an exploratory visualization of the training data set. It is a bar chart showing the percentage of each class from the entire
training set.

![alt text][image1]

### Design and Test a Model Architecture

#### 1. Describe how you preprocessed the image data. What techniques were chosen and why did you choose these techniques? Consider including images showing the output of each preprocessing technique. Pre-processing refers to techniques such as converting to grayscale, normalization, etc. (OPTIONAL: As described in the "Stand Out Suggestions" part of the rubric, if you generated additional data for training, describe why you decided to generate additional data, how you generated the data, and provide example images of the additional data. Then describe the characteristics of the augmented training set like number of images in the set, number of images for each class, etc.)

As a first step, I decided to zoom in the images. The provided images contains the sign and some less usefull background information.
Zooming in is done by increasing the provided 32x32 image to original size and then extracting just the sign in the bounding box. 
This resulted image is then scaled to 32x32 image.

Here is an example of a traffic sign image before and before and after zooming in.

![alt text][image2]

![alt text][image3]

As a last step I transformed each zoomed-in picture to a grayscale image since the color information is not neccesary and
grayscaling helps with dark images.

Here is an example of an grayscale image image:

![alt text][image4]



#### 2. Describe what your final model architecture looks like including model type, layers, layer sizes, connectivity, etc.) Consider including a diagram and/or table describing the final model.

My final model is a LeNet model consisted of the following layers:

| Layer         		|     Description	        					| 
|:---------------------:|:---------------------------------------------:| 
| Input         		| 32x32x1 grayscale image   					| 
| Convolution 3x3     	| 1x1 stride, valid padding, outputs 28x28x6 	|
| RELU					|												|
| Max pooling	      	| 2x2 stride,  outputs 14x14x6 					|
| Convolution 3x3	    | 1x1 stride, valid padding, outputs 10x10x16.	|
| RELU					|												|
| Max pooling	      	| 2x2 stride,  outputs 5x5x16 					|
| Flatten			 	| Output = 400.									|
| Fully connected		| Output = 120 									|
| RELU					|												|
| Dropout				| Droprate = 0.8 								|
| Fully connected		| Output = 84 									|
| RELU					|												|
| Dropout				| Droprate = 0.8 								|
| Fully connected		| Output = 43 									|
|						|												|
 
I added dropout layers for the fully connected layers to minimalize overfitting. The keep rate is 0.8 because it seems that this value
keeps the validation accuracy around 95.

#### 3. Describe how you trained your model. The discussion can include the type of optimizer, the batch size, number of epochs and any hyperparameters such as learning rate.

To train the model, I used tf.nn.softmax_cross_entropy_with_logits() to get the cross entropy between logits and labels. This function is recomended for discrete classification. I also used AdamOptimizer as it is an implementation of stochastic gradient descent. The reason for choosing batch size, epochs and arhitectue changes are described bellow, at chapter 4.

Here are the model values and hyperparameters:

Batch size = 64
Number of epochs = 90
Learning rate = 0.0005
mu = 0
sigma = 0.1
dropRate = 0.8

#### 4. Describe the approach taken for finding a solution and getting the validation set accuracy to be at least 0.93. Include in the discussion the results on the training, validation and test sets and where in the code these were calculated. Your approach may have been an iterative process, in which case, outline the steps you took to get to the final solution and why you chose those steps. Perhaps your solution involved an already well known implementation or architecture. In this case, discuss why you think the architecture is suitable for the current problem.

My final model results were:
* training set accuracy of 0.995
* validation set accuracy of 0.941 
* test set accuracy of 0.688

If an iterative approach was chosen:
* The first arhitecture that I chose was standard LeNet. I tried running the default LeNet but I only got 0.9 accuracy at most.
* The first thing I did afterwards was to look at the input images and do some data preprocessing. This improved the accuracy but did
  not passed 0.93.
* I played with the learning rate and saw that decreasing the learning rate improves accuracy only when more epochs are added. This makes sense since a smaller learning rate adjusts the weights slightly and requires more interations in order to achieve maximal accuracy.
* I played also with the batch size and saw that a smaller batch size increases the accuracy. I think this is due to the fact that more combinations of the training data are done. I'm not sure about this conclussion.
* I added 2 dropout layers to avoid overfitting. I have read that dropout layers are best added at fully connected layers.

If a well known architecture was chosen:
* I have chosed the LeNet arhitecture since it works well with MNIST data set wich is similar in size(32,32,1).
 

### Test a Model on New Images

#### 1. Choose five German traffic signs found on the web and provide them in the report. For each image, discuss what quality or qualities might be difficult to classify.

Here are five German traffic signs that I found on the web(https://en.wikipedia.org/wiki/Road_signs_in_Germany):

![alt text][image5] ![alt text][image6] ![alt text][image7] 
![alt text][image8] ![alt text][image9]

These images are easy to clasify since they are crisp and not distorted or rotated. If the signs are rotated or distorted aditional training data should be generated to fit "uglier" signs.

#### 2. Discuss the model's predictions on these new traffic signs and compare the results to predicting on the test set. At a minimum, discuss what the predictions were, the accuracy on these new predictions, and compare the accuracy to the accuracy on the test set (OPTIONAL: Discuss the results in more detail as described in the "Stand Out Suggestions" part of the rubric).

Here are the results of the prediction:

| Image			        |     Prediction	        					| 
|:---------------------:|:---------------------------------------------:| 
| Priority road      	| Priority road   								| 
| Bumpy road   			| Bumpy road									|
| Slippery road			| Slippery road									|
| Stop					| Stop					 						|
| Priority road			| Priority road      							|


The model was able to correctly guess 5 of the 5 traffic signs, which gives an accuracy of 100%. This compares favorably to the accuracy on the test set of 68%.

#### 3. Describe how certain the model is when predicting on each of the five new images by looking at the softmax probabilities for each prediction. Provide the top 5 softmax probabilities for each image along with the sign type of each probability. (OPTIONAL: as described in the "Stand Out Suggestions" part of the rubric, visualizations can also be provided such as bar charts)

The code for making predictions on my final model is located in the 35th cell of the Ipython notebook(Top 5 probabilities).

For all images, the model is almost 100% sure that the images are what they realy are. Here is a summary of the predictions and probabilities:

Input sign 0: Slippery road

Top 5: 
Slippery road | Bicycles crossing | Dangerous curve to the left | Wild animals crossing | Road narrows on the right<br/>
Logits values: [[ 107.02990723   96.82907867   80.18231964   33.70703506  -20.9582901 ]]<br/>
Softmax probabilities [[  9.99962807e-01   3.71381502e-05   2.18889403e-12   1.43305677e-32   0.00000000e+00]]<br/>

Input sign 1: Children crossing

Top 5: Children crossing | Bicycles crossing | Slippery road | Beware of ice/snow | Dangerous curve to the right<br/>
Logits values: [[ 119.94853973   33.1582756    10.85764217   -5.88948917  -13.25874519]]<br/>
Softmax probabilities [[  1.00000000e+00   2.02985748e-38   0.00000000e+00   0.00000000e+00    0.00000000e+00]]<br/>

Input sign 2: Stop

Top 5: Stop | Keep right | Speed limit (50km/h) | Speed limit (30km/h) | No vehicles<br/>
Logits values: [[ 60.27801895   0.26411378  -8.68488884 -11.45818329 -12.77952194]]<br/>
Softmax probabilities [[  1.00000000e+00   8.63559793e-27   1.12147738e-30   7.00424892e-32    1.86858610e-32]]<br/>

Input sign 3: Bumpy road

Top 5: Bumpy road | Road narrows on the right | Bicycles crossing | Road work | Beware of ice/snow<br/>
Logits values: [[ 105.39032745   52.422966     42.74328232   25.75564194    7.80620003]]<br/>
Softmax probabilities [[  1.00000000e+00   9.92126852e-24   6.20488839e-28   2.60072156e-35    4.16185644e-43]]<br/>

Input sign 4: Priority road

Top 5: Priority road | Roundabout mandatory | Speed limit (100km/h) | No passing | Stop<br/>
Logits values: [[ 64.66733551   5.14706755  -4.97055531 -15.2597971  -21.20010376]]<br/>
Softmax probabilities [[  1.00000000e+00   1.41473883e-26   5.71014389e-31   1.94127812e-35    5.10793021e-38]]<br/>

The softmax probabilities are hard to be observed when the classifier is this sure but the logits can be observed better. 

### (Optional) Visualizing the Neural Network (See Step 4 of the Ipython notebook for more details)
#### 1. Discuss the visual output of your trained network's feature maps. What characteristics did the neural network use to make classifications?

The last 3 cells displays a random image and 2 convolution feature maps:

Input image:

![alt text][image10]

Convolution 1 maps:

![alt text][image11]

Convolution 2 maps:

![alt text][image12]
