import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.svm import SVC, SVR
from sklearn.metrics import accuracy_score, mean_squared_error


# ## Load data
x_train = np.load("x_train.npy")
y_train = np.load("y_train.npy")
x_test = np.load("x_test.npy")
y_test = np.load("y_test.npy")

# 550 data with 300 features
print(x_train.shape)

# It's a binary classification problem 
print(np.unique(y_train))


# ## Question 1
# K-fold data partition: Implement the K-fold cross-validation function. Your function should take K as an argument and return a list of lists (len(list) should equal to K), which contains K elements. Each element is a list contains two parts, the first part contains the index of all training folds, e.g. Fold 2 to Fold 5 in split 1. The second part contains the index of validation fold, e.g. Fold 1 in  split 1

def cross_validation(x_train, y_train, k=5):
	train_folds = np.arange(x_train.shape[0])
	np.random.shuffle(train_folds)
	train_folds = np.array_split(train_folds, k)

	folds = []
	for i in range(k):
		train_x = np.concatenate(train_folds[:i] + train_folds[i+1:])
		train_x = np.sort(train_x)
		valid = train_folds[i]
		valid = np.sort(valid)
		folds.append([train_x, valid])
	return folds


kfold_data = cross_validation(x_train, y_train, k=10)
assert len(kfold_data) == 10 # should contain 10 fold of data
assert len(kfold_data[0]) == 2 # each element should contain train fold and validation fold
assert kfold_data[0][1].shape[0] == 55 # The number of data in each validation fold should equal to training data divieded by K


# ## example
from sklearn.model_selection import KFold

X = np.arange(20)
kf = KFold(n_splits=5, shuffle=True)
kfold_data= []
for i, (train_index, val_index) in enumerate(kf.split(X)):
    print("Split: %s, Training index: %s, Validation index: %s" % (i+1, train_index, val_index))
    kfold_data.append([train_index, val_index])

assert len(kfold_data) == 5 # should contain 5 fold of data
assert len(kfold_data[0]) == 2 # each element should contains index of training fold and validation fold
assert kfold_data[0][1].shape[0] == 4 # The number of data in each validation fold should equal to training data divieded by K


# ## Question 2
# Using sklearn.svm.SVC to train a classifier on the provided train set and conduct the grid search of “C”, “kernel” and “gamma” to find the best parameters by cross-validation.
kfold_data = cross_validation(x_train, y_train, k=5)
clf = SVC(C=1.0, kernel='rbf', gamma=0.01)
var = 1e-3
gamma =[]
C = []
for i in range(7):
	if var*10*i != 0:
		C.append(var*10**i)
var = 1e-5
for i in range(7):
	if var*10*i != 0:
		gamma.append(var*10**i)
#print(gamma)
#print(C)
totalscores = np.zeros(shape=(len(gamma), len(C)))

for i, gg in enumerate(gamma):
	for j, cc in enumerate(C):
		clf = SVC(C=cc, gamma=gg)
		score = 0.0
		for idx in kfold_data:
			clf.fit(x_train[idx[0]], y_train[idx[0]])
			score += accuracy_score(clf.predict(x_train[idx[1]]), y_train[idx[1]])
		score = score/len(kfold_data)
		totalscores[i][j] = score
print(totalscores)
Mx = -1.0
Mi = 0
Mj = 0
for i in range(len(gamma)):
	for j in range(len(C)):
		if Mx < totalscores[i][j]:
			Mx = totalscores[i][j]
			Mi = i
			Mj = j
best_parameters = {"Gamma": gamma[Mi], "C": C[Mj]}
print(best_parameters)


# ## Question 3
# Plot the grid search results of your SVM. The x, y represents the hyperparameters of “gamma” and “C”, respectively. And the color represents the average score of validation folds
# You reults should be look like the reference image ![image](https://miro.medium.com/max/1296/1*wGWTup9r4cVytB5MOnsjdQ.png) 
f, axis = plt.subplots()
image = axis.imshow(totalscores, cmap = "seismic")
axis.set_title("Hyperparameter Gridsearch")
axis.set_xlabel("Gamma Parameter")
axis.set_ylabel("C Parameter")
axis.set_xticklabels(gamma)
axis.set_yticklabels(C)
axis.set_xticks(np.arange(len(gamma)))
axis.set_yticks(np.arange(len(C)))

cbar = axis.figure.colorbar(image, ax = axis)
for i in range(len(gamma)):
	for j in range(len(C)):
		text = axis.text(j, i, f"{totalscores[i, j]:.2f}", ha="center", va="center", color="yellow")
plt.show()
# ## Question 4
# Train your SVM model by the best parameters you found from question 2 on the whole training set and evaluate the performance on the test set. **You accuracy should over 0.85**
best_model = SVC(C=C[Mj], gamma=gamma[Mi])
best_model.fit(x_train,y_train)
y_pred = best_model.predict(x_test)
print("Accuracy score: ", accuracy_score(y_pred, y_test))


# ## Question 5
# Compare the performance of each model you have implemented from HW1

# ### HW1
train_df = pd.read_csv("../HW1/train_data.csv")
x_train = train_df['x_train'].to_numpy().reshape(-1,1)
y_train = train_df['y_train'].to_numpy().reshape(-1,1)

test_df = pd.read_csv("../HW1/test_data.csv")
x_test = test_df['x_test'].to_numpy().reshape(-1,1)
y_test = test_df['y_test'].to_numpy().reshape(-1,1)

a, b = 0, 0
rate = 1e-4
epochs = 100
error = []
p = []

for i in range(epochs):
	loss, da, db = 0, 0, 0
	for j in range(len(x_train)):
		y_pred = x_train[j] * a + b
		loss = loss + (y_train[j] - y_pred)**2
		da = da + (-2) * (y_train[j] - y_pred) * x_train[j]
		db = db + (-2) * (y_train[j] - y_pred)
	a -= rate * da
	b -= rate * db
	p.append(j+1)
	error.append(loss)

loss = 0
for i in range(len(x_test)):
	y_pred = a * x_test[i] + b
	loss = loss + (y_test[i] - y_pred)**2

print("Square error of Linear regression: ", loss/float(len(x_test)))
kfold_data = cross_validation(x_train, y_train, k=5)
clf = SVR(C=1.0, kernel="rbf", gamma=0.01)
var = 1e-5
for i in range(7):
	if var*10*i != 0:
		gamma.append(var*10**i)
#print(gamma)
var = 1e-3
for i in range(7):
	if var*10*i != 0:
		C.append(var*10**i)
#print(C)
totalscores = np.zeros(shape=(len(gamma),len(C)))
for i, gg in enumerate(gamma):
	for j, cc in enumerate(C):
		clf = SVR(C=cc, gamma =gg)
		score = 0.0
		for idx in kfold_data:
			clf.fit(x_train[idx[0]], y_train[idx[0]].ravel())
			score = score + mean_squared_error(clf.predict(x_train[idx[1]]), y_train[idx[1]].ravel()/len(idx[1]))
			score = score/len(kfold_data)
			totalscores[i][j] = score
	print(totalscores)
	Mx = 1e9
	Mi = 0
	Mj = 0
	for i in range(len(gamma)):
		for j in range(len(C)):
			if Mx > totalscores[i][j]:
				Mx = totalscores[i][j]
				Mi = i
				Mj = j
f, axis = plt.subplots()
image = axis.imshow(totalscores, cmap="seismic")
axis.set_title("Hyperparameter Gridsearch")
axis.set_xlabel("Gamma Parameter")
axis.set_ylabel("C Parameter")
axis.set_xticklabels(gamma)
axis.set_yticklabels(C)
axis.set_xticks(np.arange(len(gamma)))
axis.set_yticks(np.arange(len(C)))
cbar = axis.figure.colorbar(image, ax=axis)
for i in range(len(gamma)):
	for j in range(len(C)):
		text = axis.text(j, i, f"{totalscores[i, j]:.2f}", ha="center", va="center", color="yellow")
plt.show()
best_parameters = {"Gamma": gamma[Mi], "C":C[Mj]}
print(best_parameters)
model = SVR(C=C[Mj], gamma=gamma[Mi])
model.fit(x_train, y_train.ravel())
y_pred = model.predict(x_test)
print("Square error of SVM regresssion model: ", mean_squared_error(y_pred, y_test)/len(y_test))

