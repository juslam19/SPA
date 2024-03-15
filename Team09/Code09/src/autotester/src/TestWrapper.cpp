#include "TestWrapper.h"

#include <vector>

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
    if (wrapper == 0)
        wrapper = new TestWrapper;
    return wrapper;
}
// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
// create any objects here as instance variables of this class
// as well as any initialization required for your spa program
#ifdef DEBUG_BUILD
    std::cout << "Running debug build" << std::endl;
#endif
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
    // call your parser to do the parsing
    // ...rest of your code...
    try {
        spa.parse(filename);
    } catch (SyntaxError& e) {
        std::cerr << "Syntax Error: " << e.what() << std::endl;
        ::exit(1);
    }
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
    // call your evaluator to evaluate the query here
    // ...code to evaluate query...
    std::vector<std::string> queryResults = spa.evaluate(query);
    // store the answers to the query in the results list (it is initially empty)
    // each result must be a string.
    for (auto queryResult : queryResults) {
        results.push_back(queryResult);
    }
}
