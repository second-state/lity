contract test {
    function f() public {
        address(0x12).call("abc");
    }
}
// ----
// Warning: (50-75): Return value of low-level calls not used.
// Warning: (50-68): The use of low level "call" should be avoided. Use direct call to abstract contract instead.
