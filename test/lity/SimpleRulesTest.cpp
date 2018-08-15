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

static unique_ptr<bytes> s_compiledContract;

class SimpleRulesTestFramework: public ContractExecutionFramework
{
protected:
	void deployContract()
	{
		if (!s_compiledContract)
			s_compiledContract.reset(new bytes(compileContractFile(
				"test/lity/contracts/simplerules.sol",
				"C")));

		bytes args = encodeArgs();
		sendMessage(*s_compiledContract + args, true);
		BOOST_REQUIRE(!m_output.empty());
	}
};

/// This is a test suite that tests optimised code!
BOOST_FIXTURE_TEST_SUITE(LitySimpleRules, SimpleRulesTestFramework)

BOOST_AUTO_TEST_CASE(fireAllRules)
{
	deployContract();
	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(0)));
	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(18)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(24)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(10)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(12)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(55)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(5)));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(0)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(1)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(2)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(3)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(4)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("checkPeople()") == encodeArgs());
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(0)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(1)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(2)) == encodeArgs(false));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(3)) == encodeArgs(false));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(4)) == encodeArgs(true));
}

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
