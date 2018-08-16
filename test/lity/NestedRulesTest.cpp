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

class MultiTypeRulesTestFramework: public ContractExecutionFramework
{
protected:
	void deployContract()
	{
		if (!s_compiledContract)
			s_compiledContract.reset(new bytes(compileContractFile(
				"test/lity/contracts/nestedrules.sol",
				"C")));

		bytes args = encodeArgs();
		sendMessage(*s_compiledContract + args, true);
		BOOST_REQUIRE(!m_output.empty());
	}
};

/// This is a test suite that tests optimised code!
BOOST_FIXTURE_TEST_SUITE(LityMultiTypeRules, MultiTypeRulesTestFramework)

BOOST_AUTO_TEST_CASE(rule_once)
{
	deployContract();

	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(0)));
	BOOST_REQUIRE(callContractFunction("numberOfAccount()") == encodeArgs(u256(0)));

	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(18)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addAccount(uint256,uint256)", u256(0), u256(0)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(1)));
	BOOST_REQUIRE(callContractFunction("numberOfAccount()") == encodeArgs(u256(1)));

	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(0)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(0)) == encodeArgs(0));
	BOOST_REQUIRE(callContractFunction("pay()") == encodeArgs());
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(0)) == encodeArgs(false));
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(0)) == encodeArgs(10000));
}

BOOST_AUTO_TEST_CASE(rule_modify_condition)
{
	deployContract();

	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(0)));
	BOOST_REQUIRE(callContractFunction("numberOfAccount()") == encodeArgs(u256(0)));

	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(18)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(63)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(82)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(7)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addPerson(uint256)", u256(28)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(5)));
	BOOST_REQUIRE(callContractFunction("numberOfAccount()") == encodeArgs(u256(0)));

	BOOST_REQUIRE(callContractFunction("pay()") == encodeArgs());
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(0)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(1)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(2)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(3)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(4)) == encodeArgs(true));

	BOOST_REQUIRE(callContractFunction("addAccount(uint256,uint256)", u256(0), u256(0)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addAccount(uint256,uint256)", u256(0), u256(1)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addAccount(uint256,uint256)", u256(0), u256(2)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addAccount(uint256,uint256)", u256(1), u256(0)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addAccount(uint256,uint256)", u256(1), u256(1)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addAccount(uint256,uint256)", u256(2), u256(2)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addAccount(uint256,uint256)", u256(3), u256(1)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addAccount(uint256,uint256)", u256(4), u256(0)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(5)));
	BOOST_REQUIRE(callContractFunction("numberOfAccount()") == encodeArgs(u256(8)));

	BOOST_REQUIRE(callContractFunction("pay()") == encodeArgs());
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(0)) == encodeArgs(false));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(1)) == encodeArgs(false));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(2)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(3)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("isEligible(uint256)", u256(4)) == encodeArgs(false));

	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(0)) == encodeArgs(10000));
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(1)) == encodeArgs(0));
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(2)) == encodeArgs(0));
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(3)) == encodeArgs(10777));
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(4)) == encodeArgs(777));
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(5)) == encodeArgs(777));
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(6)) == encodeArgs(0));
	BOOST_REQUIRE(callContractFunction("balance(uint256)", u256(7)) == encodeArgs(10000));
}

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
