#include <boost/test/unit_test.hpp>
#include <test/lity/ContractExecutionFramework.h>

using namespace std;
using namespace dev::test;

namespace dev
{
namespace solidity
{
namespace test
{

static map<string, unique_ptr<bytes>> s_compiledContracts;

class RuleInheritanceTestFramework: public ContractExecutionFramework
{
protected:
	void deployContract(const string& _contractFileName)
	{
		if (!s_compiledContracts[_contractFileName])
			s_compiledContracts[_contractFileName].reset(new bytes(compileContractFile(
				"test/lity/contracts/" + _contractFileName,
				"C")));

		bytes args = encodeArgs();
		sendMessage(*(s_compiledContracts[_contractFileName]) + args, true);
		BOOST_REQUIRE(!m_output.empty());
	}
};

/// This is a test suite that tests optimised code!
BOOST_FIXTURE_TEST_SUITE(LityRulesInheritance, RuleInheritanceTestFramework)

BOOST_AUTO_TEST_CASE(inherit) // this test is for rule inheritance
{
	deployContract("inherit.sol");
	BOOST_REQUIRE(callContractFunction("setup()") == encodeArgs());
	BOOST_REQUIRE(callContractFunction("queryDiscount(uint256)", 0) == encodeArgs(u256(1)));
	BOOST_REQUIRE(callContractFunction("queryDiscount(uint256)", 1) == encodeArgs(u256(0)));
	BOOST_REQUIRE(callContractFunction("queryFreeParking(uint256)", 0) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("queryFreeParking(uint256)", 1) == encodeArgs(false));
}

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
