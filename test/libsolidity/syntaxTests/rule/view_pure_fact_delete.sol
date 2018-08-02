contract C {
    function deletePersonView() public view {
        factDelete uint256(0);
    }
    function deletePersonPure() public pure {
        factDelete uint256(0);
    }
}
// ----
// Warning: (67-88): Function declared as view, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
// TypeError: (150-171): Function declared as pure, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
