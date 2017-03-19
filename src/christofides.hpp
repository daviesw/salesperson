#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <climits>
#include <stack>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/program_options.hpp>
#include <boost/graph/simple_point.hpp>
#include <boost/graph/metric_tsp_approx.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>

template<typename PositionVector>
int christofides(PositionVector *, int, int *);
