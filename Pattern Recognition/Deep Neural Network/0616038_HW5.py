#!/usr/bin/env python
# coding: utf-8

# In[4]:


import os
import numpy as np

import keras
from keras.models import Sequential
from keras.layers import Dense, Activation, Flatten, Conv2D, MaxPooling2D
from sklearn.metrics import accuracy_score
from keras.preprocessing.image import ImageDataGenerator

# ## Load data

# In[5]:


x_train = np.load("x_train.npy")
y_train = np.load("y_train.npy")

x_test = np.load("x_test.npy")
y_test = np.load("y_test.npy")

print(x_train.shape[0], 'train samples')
print(x_test.shape[0], 'test samples')


# In[6]:


# It's a multi-class classification problem 
class_index = {'airplane': 0, 'automobile': 1, 'bird': 2, 'cat': 3, 'deer': 4,
               'dog': 5, 'frog': 6,'horse': 7,'ship': 8, 'truck': 9}
print(np.unique(y_train))


# ![image](https://img-blog.csdnimg.cn/20190623084800880.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3lqcDE5ODcxMDEz,size_16,color_FFFFFF,t_70)

# ## Data preprocess

# In[7]:


x_train = x_train.astype('float32')
x_test = x_test.astype('float32')
x_train /= 255
x_test /= 255

# Convert class vectors to one-hot encoding (keras model requires one-hot label as inputs)
num_classes = 10
y_train = keras.utils.to_categorical(y_train, num_classes)
y_test = keras.utils.to_categorical(y_test, num_classes)


# ## Build model & training (Keras)

# In[9]:
from keras.regularizers import l2
from keras.layers import Dropout
from keras.constraints import maxnorm
from keras.optimizers import SGD
from keras import optimizers
import warnings
warnings.filterwarnings('ignore')
# Builde model
model = Sequential() # Sequential groups a linear stack of layers 
#
model.add(Conv2D(filters=32, kernel_size=(3, 3), input_shape=x_train.shape[1:])) # Add Convolution layers
model.add(Activation('relu')) # Add Relu activation for non-linearity
model.add(Conv2D(filters=32, kernel_size=(3, 3))) # Add Convolution layers
model.add(Activation('relu')) # Add Relu activation for non-

model.add(MaxPooling2D(pool_size=(4, 4))) # Add Max pooling to lower the sptail dimension

model.add(Flatten()) # Flatten the featuremaps
#model.add(Dropout(0.2))
model.add(Dense(units=1024)) # Add dense layer with 512 neurons
model.add(Activation('relu')) # Add Relu activation for non-linearity
model.add(Dropout(0.25))
model.add(Dense(units=512)) # Add dense layer with 512 neurons
model.add(Activation('relu')) # Add Relu activation for non-linearity



model.add(Dense(units=num_classes)) # Add final output layer for 10 classes
model.add(Activation('softmax')) # Add softmax activation 

# initiate SGD optimizer
opt = keras.optimizers.Adam()

# Compile the model with loss function and optimizer
model.compile(loss='categorical_crossentropy',optimizer=opt,metrics=['accuracy'])

print(len(x_train))
batch_size = 256
epochs = 30
# Fit the data into model
# model.fit(x_train, y_train,
#           batch_size=batch_size,
#           epochs=epochs,
#           validation_data=(x_test, y_test),
#           shuffle=True)


#construct the training image generator for data augmentation
# aug = ImageDataGenerator(rotation_range=20, zoom_range=0.15,
# 	width_shift_range=0.2, height_shift_range=0.2, shear_range=0.15,
# 	horizontal_flip=True, fill_mode="nearest")

# train the network
# H = model.fit_generator(aug.flow(x_train, y_train, batch_size=batch_size),
# 	validation_data=(x_test, y_test), steps_per_epoch=len(x_train)//batch_size,
# 	epochs=epochs * 1.5)

train_datagen = ImageDataGenerator(    

    rotation_range=40,

    width_shift_range=0.2,

    height_shift_range=0.2,

    shear_range=0.2,

    zoom_range=0.2,

    horizontal_flip=True,

    fill_mode='nearest')


model.fit_generator(

train_datagen.flow(x_train, y_train, batch_size=batch_size),

samples_per_epoch= len(x_train) * 2 // batch_size , nb_epoch=epochs,

validation_data=(x_test, y_test))

# datagen = ImageDataGenerator(
#   rotation_range=20,
#   width_shift_range=0.2,
#   height_shift_range=0.2,
#   horizontal_flip=True,
#   vertical_flip=False,
#   )
# datagen.fit(x_train)

# model.fit_generator(datagen.flow(x_train, y_train,
#   batch_size=batch_size),
#   epochs=epochs,
#   validation_data=(x_test, y_test),
#   shuffle=True)

# In[10]:


y_pred = model.predict(x_test)
print(y_pred.shape)


# In[11]:


y_pred[0]


# In[12]:


np.argmax(y_pred[0])


# In[13]:


y_pred = np.argmax(y_pred, axis=1)


# ## DO NOT MODIFY CODE BELOW!
# please screen shot your results and post it on your report

# In[14]:


assert y_pred.shape == (10000,)


# In[15]:


y_test = np.load("y_test.npy")
print("Accuracy of my model on test-set: ", accuracy_score(y_test, y_pred))


# In[ ]:




