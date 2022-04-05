
#ifndef TEST_H_
#define TEST_H_

#include<vector>
#include "rdf_index.h"


bool test_rdf_ds_add(RdfIndex &);

bool test_rdf_ds_evaluate(RdfIndex &);
bool test_query(id_2_resource_type &  res_2_id_map, vector<tuple<std::string, bool>> & _id_2_res_v, RdfIndex & rdfIndex);
bool test_query_optimiser();

#endif //TEST_H_