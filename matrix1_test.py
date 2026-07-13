import matrix1
import numpy as np
def test_matrix1():
    m1 = matrix1.MatrixInt(2, 2,1)
    m2 = matrix1.MatrixInt.ones(2,2)
    m3 = matrix1.MatrixInt.zeros(2,2)
    
    print(m1)
    print(m2)
    print(m3)
    assert m1 == m2
    assert m1 != m3
    

    m4 = m1 + m2
    assert m4 == matrix1.MatrixInt(2, 2, 2)

    m5 = m1 * 2
    assert m5 == matrix1.MatrixInt(2, 2, 2)

    m6 = m1 * m3
    print(m6)
    assert m6 == matrix1.MatrixInt(2, 2, 0)
    m1[0, 0] = 1
    print("*****SUMA+*****")
    print(m1 + m1)
    print("*****SUMA+=*****")
    m1 += m1
    print(m1)
    print("Accessing elements:", m1[0,0], m1[1][1])
    print("SUMA CON ESCALAR")
    print(m1 + 1)

    print(2 + m1)
    a = np.asarray(m1)
    print("Numpy array:\n", a)
    a = np.frombuffer(m1, dtype=np.int32).reshape(m1.rows(), m1.cols())
    print("Numpy array from buffer:\n", a)
    
    #MatrixFloat
    mf = matrix1.MatrixFloat(2, 2, 1.5)
    print("MatrixFloat:\n", mf)

if __name__ == "__main__":
    test_matrix1()
    print("All tests passed!")