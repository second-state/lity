add_executable(solfuzzer fuzzer.cpp)
target_link_libraries(solfuzzer PRIVATE liblityc evmasm ${Boost_PROGRAM_OPTIONS_LIBRARIES} ${Boost_SYSTEM_LIBRARIES})

add_executable(yulopti yulopti.cpp)
target_link_libraries(yulopti PRIVATE lity ${Boost_PROGRAM_OPTIONS_LIBRARIES} ${Boost_SYSTEM_LIBRARIES})

add_executable(isoltest
	isoltest.cpp
	../Options.cpp
	../Common.cpp
	../TestCase.cpp
	../libsolidity/SyntaxTest.cpp
	../libsolidity/AnalysisFramework.cpp
	../libsolidity/SolidityExecutionFramework.cpp
	../ExecutionFramework.cpp
	../RPCSession.cpp
	../libsolidity/ASTJSONTest.cpp
	../libsolidity/SMTCheckerJSONTest.cpp
	../libyul/ObjectCompilerTest.cpp
	../libyul/YulOptimizerTest.cpp
)
target_link_libraries(isoltest PRIVATE liblityc lity evmasm ${Boost_PROGRAM_OPTIONS_LIBRARIES} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES})
