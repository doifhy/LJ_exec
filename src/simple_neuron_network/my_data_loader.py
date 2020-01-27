# -*- coding:utf-8 -*-

#首先mnist数据集为压utLayer'缩文件
import gzip
import pickle as p
import numpy as np

def load_data():
	#只读打开
	f = gzip.open("mnist.pkl.gz", "rb")
	training, validation, test = p.load(f,encoding='unicode-escape')
	f.close()
	return (training,validation,test)

def value_to_row_vector(y):
	result = np.zeros((10,))
	result[y] = 1.0
	return result


def data_wrapper():
	#希望input:training_data的输入格式是x为（1，784）列,y为（1,10）的tuple(x,y)列表
	#[([1,784],[1,10]), ([1,784],[1,10])]
	training_data, validation_data, test_data = load_data();

	#training	
	training_inputs = [np.array(x).reshape(784,) for x in training_data[0]]
	training_results = [value_to_row_vector(y) for y in training_data[1]]
	training = (training_inputs, training_results)
 	
	#validation
	#validation格式: [([1,784],一个0-9的整数)]
	validation_inputs = [np.array(x).reshape(784,) for x in validation_data[0]]
	validation = (validation_inputs, training_data[1])

	#test
	#the same as validation
	test_inputs = [np.array(x).reshape(784,) for x in test_data[0]]
	test = (test_inputs, test_data[1])
	
	return (training, validation, test)	
	
