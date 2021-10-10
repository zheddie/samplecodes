#include <iostream>
#include <vector>
#include <cstdlib>

#include <cblas.h>

// 随机生成20以内的给定尺寸数组
static void RandomFill(std::vector<float>& numbers,size_t size);
static void ZeroFill(std::vector<float>& numbers,size_t size);
// 打印数组元素的函数
static void Print(const std::vector<float>& numbers);
// vector是一维的，输出是个矩阵，那输出的时候就要指定有几行几列
static void Print(const std::vector<float>& numbers, int rows, int cols);
// 寻找数组中最大的那个元素的索引和值
static void TestLevel1();
// 测试Level2里面最常用的函数：向量和矩阵的乘积
static void TestLevel2();
static void TestLevel3();

int main(int argc, const char * argv[]) {
    TestLevel1();
    TestLevel2();
    TestLevel3();
    return 0;
}

void RandomFill(std::vector<float>& numbers, size_t size) {
    // 预分配size的缓冲区，这样性能相对更好一点
    numbers.resize(size);
    for (size_t i = 0; i != size; ++ i) {
        numbers[i] = static_cast<float>(rand() % 20);
    }
}
void ZeroFill(std::vector<float>& numbers, size_t size) {
    // 预分配size的缓冲区，这样性能相对更好一点
    numbers.resize(size);
    for (size_t i = 0; i != size; ++ i) {
        numbers[i] = static_cast<float>(0);
    }
}
void Print(const std::vector<float>& numbers) {
    for (float number : numbers) {
        std::cout << number << ' ';
    }
    std::cout << std::endl;
}

void Print (const std:: vector<float>& numbers ,int  rows, int cols) {
    for (int row =0; row != rows; ++ row) {
        for (int col = 0; col != cols; ++ col) {
            // 取出每一列的数字
            std::cout << numbers[row * cols + col] << ' ';
        }
        // 没输出一行之后换一行
        std::cout << std::endl;
    }
}

static void TestLevel1() {
    const int VECTOR_SIZE = 4;
    std::vector<float> fv1;
    
    RandomFill(fv1, VECTOR_SIZE);
    Print(fv1);

    /**
     从数组里面找出最大的那个数的索引：cblas_isamax（）属于Level1的函数

      VECTOR_SIZE 数组长度
      fv1.data 数组缓冲区指针的首地址，怎么获得fv1内部缓冲区呢，用fv1.data
     third params: 跳跃数量，两个元素之间间隔几个元素，也就是每处理一个元素之后，+1得到下一个元素，如果想跳过一个元素 +2
     */
    size_t maxIndex = cblas_isamax(VECTOR_SIZE, fv1.data(), 1);
    std::cout << maxIndex << std::endl;
    std::cout << fv1[maxIndex] << std::endl; 
   }
 /*
     Prints:
     7 9 13 18
     3
     18
     */
/*
 测试Level2里面最常用的函数：向量和矩阵的乘积
 */
static void TestLevel2()
{
    // 假设我们有一个三行二列的矩阵
    const int M = 3;
    const int N = 2;
    
    /*
     A(M*N),x(N*1), y(M*1)
     我们定义三个矩阵a、x、y
     a：M*N的矩阵：维度为M*N矩阵
     x：N*1的矩阵，实际上是一个向量，维度为N矩阵
     y：M*1的矩阵，实际上是长度为M的向量，维度为M的矩阵
     */
    std::vector<float> a;
    std::vector<float> x;
    std::vector<float> y;
    
    RandomFill(a, M * N);
    RandomFill(x, N);
    RandomFill(y, M);
    
    std::cout << "A" << std::endl;
    Print(a, M, N);
    std::cout << "x" << std::endl;
    Print(x);
    std::cout << "y" << std::endl;
    Print(y);
    
    /*
     我们的目标是想计算这么一个公式：
     y := alpha * A * x + beta * y
     A:是一个矩阵，x是一个向量，所以我希望说去计算一个矩阵和向量的乘积。alpha是一个乘积的缩放，
     beta是对y的缩放，
     相当于把y里面的数字乘以beta，再加上A矩阵和向量的乘积。
     
     那这边有一个特例，假如我y里面都是0，或这beta是0的情况下，我就可以把公式看成：
    // y := alpha * A * x
     
     这个函数名称为：cblas_sgemv（）
     // s:single 单精度浮点数
     // ge: 是一个乘法
     // m: matrix
     // v: vector
     */

    /**
     参数解释：
     param CblasRowMajor 行主序还是列主序，默认行主序，何为主序：即数组存储元素的方式--按行存储还是按列存储，行主序：00，01，列主序00，10
     param CblasNoTrans 矩阵是否需要转置，不需要转置，如果需要转置的话，运算的时候它会自动做转置
     param M 矩阵的行数
     param N 矩阵的列数
     param 1.0f alpha ，我们设为1
     param a.data a矩阵的缓冲区首地址
     param lda a矩阵的列数
     param x.data x矩阵的缓冲区首地址
     param 1 x里面每次跳跃累加的个数，默认为1
     param 2.0f beta对y的缩放值
     param y.data y矩阵的缓冲区首地址
     param 1 y里面每次跳跃累加的个数，默认为1
     */
    int lda = N;
    cblas_sgemv(CblasRowMajor, CblasNoTrans, M, N, 1.0f, a.data(), lda, x.data(), 1, 2.0f, y.data(), 1);
    std::cout << "result y" << std::endl;
    Print(y);
}
/*
 Prints:
 A
 7 9
 13 18
 10 12
 x
 4 18
 y
 3 9 0
 result y
 196 394 256
*/

/**
 计算两个矩阵的乘积
 */
static void TestLevel3() {
    // 我们希望计算两个矩阵的乘积，我们就需要定义三个参数M、N、K。
    const int M = 3;
    const int N = 2;
    const int K = 4;
    
    std::vector<float> a;
    std::vector<float> b;
    std::vector<float> c;
    std::vector<float> d;

    RandomFill(a, M * K);
    RandomFill(b, K * N);
    ZeroFill(c, M * N);
    ZeroFill(d, M * N);
    
    // 输出A、B、C 三个矩阵
    std::cout << "A" << std::endl;
    Print(a, M, K);
    
    std::cout << "B" << std::endl;
    Print(b, K, N);
    
    std::cout << "C" << std::endl;
    Print(c, M, N);
    for(int i =0;i<M;i++){
        for(int j =0;j<N;j++){
            for(int k =0;k<K;k++){
                d[i*N+j]+=a[i*K+k]*b[k*N+j];
            }
        }
    }
    std::cout << "D" << std::endl;
    Print(d, M, N);
    /*
     我们的目标是计算这么一个公式：
    
    // C := alpha * A * B + beta * C
    // 如果只想做两个矩阵的乘法，beta设成0就好了，变为如下式子：
    // C := alpha * A * B
      */

    /*
     函数释义：
     sgemm:矩阵间的单精度乘法
     s:single 单精度
     ge：general 一般的，普通的
     m：matix 矩阵
     m：multiplication 乘法
     */
    
    /**
     参数释义：
     param CblasRowMajor 行主序还是列主序，默认行主序
     param CblasNoTrans A需不需要转置，不需要
     param CblasNoTrans B需不需要转置，不需要
     param M 系数M
     param N 系数N
     param K 系数K
     param 1.0f alpha 设为1
     param a.data a的缓冲区首地址
     param lda a的列数
     param b.data b的缓冲区首地址
     param ldb b的列数
     param 1.0f beta 设为1
     param c.data c的缓冲区首地址
     param ldc c的列数
     */
  
    int lda = K;
    int ldb = N;
    int ldc = N;
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, 1.0f, a.data(), lda, b.data(), ldb, 1.0f, c.data(), ldc);
    std::cout << "Result C" << std::endl;
    // 三行四列的矩阵 * 四行二列的矩阵 + 三行二列的矩阵，结果为一个三行二列的矩阵
    Print(c, M, N);
}
/*
 Prints:
 A
 7 9 13 18
 10 12 4 18
 3 9 0 5
 B
 12 2
 7 3
 7 9
 0 12
 C
 3 9
 9 17
 0 13
 Result C
 241 383
 241 325
 99 106
 */