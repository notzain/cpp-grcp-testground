#include "ArpScanner.h"

namespace core::services
{
void ArpScanner::DoTest()
{
    i8 test = 0;
    spdlog::info( "test is {}", test );
}

nlohmann::json ArpScanner::serialize( nlohmann::json json ) const
{
    json["results"] = { { "test", "answer" } };
    return json;
}


} // namespace core::services