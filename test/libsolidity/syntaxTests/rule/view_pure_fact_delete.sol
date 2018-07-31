contract C {
    function deletePerson() public view {
        factDelete uint256(0);
    }
}
// ----
// Warning: (63-84): Function declared as view, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
