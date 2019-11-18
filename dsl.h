#define d(node)\
node->diff()

#define c(node)\
node->copy()

#define n(num)\
create_node (num, number, nullptr, nullptr)

#define PLUS(left_l, right_l)\
create_node (0, plus, left_l, right_l)

#define MINUS(left_l, right_l)\
create_node (0, minus, left_l, right_l)

#define MULT(left_l, right_l)\
create_node (0, mul, left_l, right_l)

#define DIVIDE(left_l, right_l)\
create_node (0, divide, left_l, right_l)

#define POW(left_l, right_l)\
create_node (0, power, left_l, right_l)

#define LN(lf)\
create_node (0, ln, lf, nullptr)

#define SIN(lf)\
create_node (0, sinus, lf, nullptr)

#define COS(lf)\
create_node (0, cosinus, lf, nullptr)