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

class ComplexRulesTestFramework: public ContractExecutionFramework
{
protected:
	void deployContract()
	{
		if (!s_compiledContract)
			s_compiledContract.reset(new bytes(compileContractFile(
				"test/lity/contracts/complexrules.sol",
				"C")));

		bytes args = encodeArgs();
		sendMessage(*s_compiledContract + args, true);
		BOOST_REQUIRE(!m_output.empty());
	}
};

/// This is a test suite that tests optimised code!
BOOST_FIXTURE_TEST_SUITE(LityComplexRules, ComplexRulesTestFramework)

BOOST_AUTO_TEST_CASE(fireAllRules)
{
	deployContract();
	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(0)));
	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(18)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(0)) == encodeArgs(13));

	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(3)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(0)) == encodeArgs(113));
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(1)) == encodeArgs(23));

	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(28)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(0)) == encodeArgs(213));
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(1)) == encodeArgs(133));
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(2)) == encodeArgs(13));

	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(3)));
}

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
