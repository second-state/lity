contract C {
    struct Person {
        int age;
        bool eligible;
    }

    rule "myRule" when {
        p: Person(age > 10, eligible == true);
    } then {
    }
}
// ----
