struct oper {
    const char* name;
    const int code;
    const char priority;                //* has higher priority than +
    const char complexity;
};

enum type_codes {
    unknown,
    number,
    variable,
    plus,
    minus,
    mul,
    divide,
    power,
    sinus,
    cosinus,
    tg,
    ln,
    arcsin,
    arccos,
    arctg,
    sqroot
};

const oper operations[] = {
        {"unknown", unknown,    0,      0},
        {"number",  number,     0,      1},
        {"var",     variable,   0,      4},
        {"+",       plus,       1,      10},
        {"-",       minus,      1,      10},
        {"*",       mul,        2,      25},
        {"/",       divide,     2,      40},
        {"^",       power,      3,      60},
        {"sin",     sinus,      3,      20},
        {"cos",     cosinus,    3,      20},
        {"tg",      tg,         3,      20},
        {"ln",      ln,         3,      20},
        {"arcsin",  arcsin,     3,      50},
        {"arccos",  arccos,     3,      50},
        {"arctg",   arctg,      3,      50},
        {"sqrt",    sqroot,     3,      40},
};