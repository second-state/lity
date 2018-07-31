contract C {
    function pay() public view {
        fireAllRules;
    }
}
// ----
// Warning: (54-66): Function declared as view, but this expression (potentially) modifies the state and thus requires non-payable (the default) or payable.
