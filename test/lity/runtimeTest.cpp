/** @file lityRuntimeTest.cpp
 *  @author Yi Huang <yi@skymizer.com>
 *  @date 2018
 *  Stub for generating main boost.test module.
 *  Original code taken from "solidity/test/boostTest.cpp".
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4535) // calling _set_se_translator requires /EHa
#endif
#include <boost/test/included/unit_test.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#pragma GCC diagnostic pop

#include <test/Options.h>
#include <libsolidity/interface/Exceptions.h>

using namespace boost::unit_test;

test_suite* init_unit_test_suite( int /*argc*/, char* /*argv*/[] )
{
	master_test_suite_t& master = framework::master_test_suite();
	master.p_name.value = "LityRuntimeTests";
	dev::test::Options::get().validate();
	solAssert(!dev::test::Options::get().disableIPC, "IPC is disabled.");
	return 0;
}
