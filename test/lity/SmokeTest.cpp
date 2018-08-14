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

class SmokeTestFramework: public ContractExecutionFramework
{
protected:
	void deployContract()
	{
		if (!s_compiledContract)
			s_compiledContract.reset(new bytes(compileContractFile(
				"test/lity/contracts/smoke.sol",
				"Smoke")));

		bytes args = encodeArgs();
		sendMessage(*s_compiledContract + args, true);
		BOOST_REQUIRE(!m_output.empty());
	}
};

/// This is a test suite that tests optimised code!
BOOST_FIXTURE_TEST_SUITE(LitySmoke, SmokeTestFramework)

BOOST_AUTO_TEST_CASE(balance)
{
	BOOST_CHECK_EQUAL(balanceAt(account(1)), 0);
	u256 balance = 10 * ether;
	sendEther(account(1), balance);
	BOOST_CHECK_EQUAL(balanceAt(account(1)), balance);
}

BOOST_AUTO_TEST_CASE(contract)
{
	deployContract();
	BOOST_REQUIRE(callContractFunction("numberOfElements()") == encodeArgs(u256(0)));
	BOOST_REQUIRE(callContractFunction("addElement(uint256)", u256(77)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("numberOfElements()") == encodeArgs(u256(1)));
	BOOST_REQUIRE(callContractFunction("getElement(uint256)", u256(0)) == encodeArgs(u256(77)));
	BOOST_REQUIRE(callContractFunction("addElement(uint256)", u256(1)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addElement(uint256)", u256(2)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addElement(uint256)", u256(3)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("addElement(uint256)", u256(4)) == encodeArgs());
	BOOST_REQUIRE(callContractFunction("numberOfElements()") == encodeArgs(u256(5)));
	BOOST_REQUIRE(callContractFunction("getElement(uint256)", u256(3)) == encodeArgs(u256(3)));
}

BOOST_AUTO_TEST_CASE(gas_cost)
{
	deployContract();

	u256 balance = 10 * ether;
	sendEther(account(1), balance);
	BOOST_CHECK_EQUAL(balanceAt(account(1)), balance);

	BOOST_REQUIRE(callContractFunctionFrom(1, "addElement(uint256)", u256(1111)) == encodeArgs());

	balance -= gasCost();
	BOOST_CHECK_EQUAL(balanceAt(account(1)), balance);
}

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
