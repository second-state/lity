contract C {
    function f() public pure returns(uint) { return rand(); }
    function g() public view returns(uint) { return rand(); }
    function h() public returns(uint) { return rand(); }
}
// ----
// TypeError: (65-71): Function declared as pure, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
// TypeError: (127-133): Function declared as view, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
