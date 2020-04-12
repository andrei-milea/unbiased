
import term_doc_mat
from scipy.sparse import csr_matrix
import numpy

def to_csr_mat(mat_cpp):
    rows_no = mat_cpp.nr()
    cols_no = mat_cpp.nc()
    print('size {}, {}'.format(rows_no, cols_no))
    np_mat = numpy.zeros((rows_no, cols_no), dtype=numpy.double)
    for row in range(rows_no):
        for col in range(cols_no):
            np_mat[row, col] = term_doc_mat.get_mat_elem(mat_cpp, row, col)
    return csr_matrix(np_mat)

