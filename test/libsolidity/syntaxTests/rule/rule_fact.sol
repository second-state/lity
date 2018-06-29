contract C {
    struct Person {
        int age;
    }

    rule "myRule" when {
        p: Person(true);
        q: Person(true, true);
    } then {
    }
}
// ----
