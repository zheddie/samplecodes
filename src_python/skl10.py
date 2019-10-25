#同样首先，我们调用模块
#matplotlib是python专门用于画图的库
#import matplotlib.pyplot as plt
from sklearn import datasets
#调用线性回归函数
from sklearn.linear_model import LinearRegression

#导入数据集
#这里将全部数据用于训练，并没有对数据进行划分，上例中
#将数据划分为训练和测试数据，后面会讲到交叉验证
loaded_data = datasets.load_boston()
data_X = loaded_data.data
data_y = loaded_data.target

#设置线性回归模块
model = LinearRegression()
#训练数据，得出参数
model.fit(data_X, data_y)

#利用模型，对新数据，进行预测，与原标签进行比较
print(model.predict(data_X[:4,:]))
print(data_y[:4])
