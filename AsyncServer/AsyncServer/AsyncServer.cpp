#include <iostream>
#include <boost/asio.hpp>
#include "Session.h"

int main()
{
    try {
        boost::asio::io_context ioc;
        using namespace std;
        Server s(ioc, 3333);
        ioc.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}

