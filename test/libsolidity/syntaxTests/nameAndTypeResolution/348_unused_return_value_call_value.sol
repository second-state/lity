contract test {
    function f() public {
        address(0x12).call.value(2)("abc");
    }
}
// ----
// Warning: (50-84): Return value of low-level calls not used.
// Warning: (50-68): The use of low level "call" should be avoided. Use direct call to abstract contract instead.
