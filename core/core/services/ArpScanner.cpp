#include "ArpScanner.h"

namespace core::services
{
void ArpScanner::DoTest()
{
}

nlohmann::json ArpScanner::serialize( nlohmann::json json ) const
{
    json["results"] = { { "test", "answer" } };
    return json;
}


} // namespace core::services