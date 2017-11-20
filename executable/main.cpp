#include <executor/executor.hpp>

int main(int argc, char** argv)
{
    executor::executor e;
    return e.execute(argc, argv);
}
