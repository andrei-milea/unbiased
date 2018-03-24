#include "lsa.h"

using namespace dlib;

void LSA::run_svd()
{
	svd(term_doc_mat_, U_, sigma_, V_);
}
