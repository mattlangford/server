#include "resources/json_resource.hh"

#include <assert.h>
#include <vector>

struct resource_test : public resources::json_resource
{
    inline const std::string& get_resource_identifier() const override
    {
        static std::string resource_identifier = "test_identifier";
        return resource_identifier;
    }

    ///
    /// Get a json object from the resource, this will be converted to a string and passed along
    ///
    json::json get_json_resource() override
    {
        std::vector<json::json> vector;
        vector.emplace_back(static_cast<std::string>("test"));
        vector.emplace_back(static_cast<bool>(false));

        json::json j;
        j = vector;

        return j;
    }
};

int main()
{
    std::cout << "Running\n";
    resources::abstract_resource::ptr test = std::make_shared<resource_test>();

    assert(test->get_resource_type() == "text/json");
    std::cout << test->get_resource() << "\n";
    assert(test->get_resource() == "[\"test\", 0]");
    std::cout << "Passed!\n";
}
