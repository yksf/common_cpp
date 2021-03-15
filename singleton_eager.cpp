#include "singleton_eager.h"

std::unique_ptr<singleton_eager> singleton_eager::instance_ {new singleton_eager};
singleton_eager *singleton_eager::instance()
{
    return instance_.get();
}

singleton_eager::singleton_eager()
{
}

singleton_eager::~singleton_eager()
{
}
