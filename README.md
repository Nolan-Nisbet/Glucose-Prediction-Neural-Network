# Glucose-Prediction-Neural-Network
A project I worked on for my fourth year Neural Networks course which aimed to allow Type 1 Diabetics on Continuous Glucose Monitors to accurately predict their glucose concentration while asleep. I would recommend reading the Report.docx to get a little more context in regards to the data source used, our approach and final results.

##Data Preprocessing

###CGMParse.cpp
This file reads in data (as seen in tblADataRTCGM_Unblinded_ControlGroup_1.zip) and creates individual csv files for each 24 hour period (beginning at 12pm) in each patients CGM data set. 

###data_preprocessor.py
Within each day's data, only the overnight data was analyzed to determine if it could be included in the training set. Overnight data was selected to minimize the likelihood that the patients would be injecting insulin or consuming food. Patient data was parsed to find 7 consecutive data points separated by 10 minute intervals, and a final data point 20 minutes ahead of the time window to be used as a prediction point. If a valid time window was found, these points were written to either the training set (with probability of 75%) or the test set (with probability of 25%) to be used as a single input into the neural network.

##Neural Nets used for Prediction
(Both predctive nets were implemented using Tensorflow)

###FeedForwardNN.py
The feedforward network followed the tapped delay-line architecture, meaning that there were 7 input nodes to accept each of the last 7 glucose measurements. The network consisted of two hidden layers with 15 nodes in each layer, and a single output neuron. These values were arbitrarily chosen, with no significant improvements resulting when the number of hidden layers or nodes was increased. A linear output function was used in order to achieve the network's purpose: prediction. Since no classification was required, there was no need for a hyperbolic or sigmoidal function; converging output values were not desired. To train the network, the Adam optimization algorithm was used with a mean squared error (MSE) cost function. An extension of gradient descent, Adam is a first-order gradient-based optimization of stochastic objective functions, based on adaptive estimates of lower-order moments.

###RecurrentNN.py
The recurrent neural network model that was tested was composed of a single long short-term memory (LSTM) cell to retain the required values for output prediction – the past 7 glucose measurements. The output produced by the LSTM cell was then multiplied by a weight, with a bias applied, to produce a predicted output value. Several of the same design decisions were made for the RNN as for the feed-forward network. Again, linear output functions were used to achieve prediction, and the Adam optimization algorithm was used to minimize the MSE cost function.
