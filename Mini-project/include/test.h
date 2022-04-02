
#ifndef TEST_H_
#define TEST_H_

#include<vector>
#include "rdf_index.h"


bool test_rdf_ds_add(RdfIndex &);

bool test_rdf_ds_evaluate(RdfIndex &);
bool test_query(unordered_map<std::string, int> &  res_2_id_map, RdfIndex & rdfIndex);

#endif //TEST_H_