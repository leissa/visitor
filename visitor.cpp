#include <cassert>
#include <iostream>
#include <string>
#include <typeinfo>

using namespace std;

class Expr {
public:
    virtual ~Expr() {}
};

class AddExpr : public Expr {
public:
    AddExpr(const Expr* left, const Expr* right)
        : left(left)
        , right(right)
    {}
    virtual ~AddExpr() override {
        delete left;
        delete right;
    }

    const Expr* left;
    const Expr* right;
};

class IdExpr : public Expr {
public:
    IdExpr(string id)
        : id(id)
    {}
    virtual ~IdExpr() override {}

    string id;
};

class NumExpr : public Expr {
public:
    NumExpr(int num)
        : num(num)
    {}
    virtual ~NumExpr() override {}

    int num;
};

template<class Derived, class R, class... Args>
class Visitor {
public:
    Derived& derived() { return static_cast<Derived&>(*this); }
    R dispatch(const Expr* e, Args... args);
};

template<class Derived, class R, class... Args>
R Visitor<Derived, R, Args...>::dispatch(const Expr* e, Args... args) {
    if (false) {}
    else if (typeid(*e) == typeid(AddExpr)) return derived().visit(static_cast<const AddExpr*>(e), args...);
    else if (typeid(*e) == typeid( IdExpr)) return derived().visit(static_cast<const  IdExpr*>(e), args...);
    else if (typeid(*e) == typeid(NumExpr)) return derived().visit(static_cast<const NumExpr*>(e), args...);
    else assert(false);
}

class Dumper : public Visitor<Dumper, std::ostream&> {
public:
    std::ostream& visit(const AddExpr* e) {
        std::cout << '(';
        dispatch(e->left);
        std::cout << " + ";
        dispatch(e->right);
        return std::cout << ')';
    }
    std::ostream& visit(const IdExpr*  e) { return std::cout << e->id; }
    std::ostream& visit(const NumExpr* e) { return std::cout << e->num; }
};

class Dumper2 : public Visitor<Dumper2, std::ostream&, std::ostream&> {
public:
    std::ostream& visit(const AddExpr* e, std::ostream& o) {
        o << '(';
        dispatch(e->left, o);
        o << " + ";
        dispatch(e->right, o);
        return o << ')';
    }
    std::ostream& visit(const IdExpr*  e, std::ostream& o) { return o << e->id; }
    std::ostream& visit(const NumExpr* e, std::ostream& o) { return o << e->num; }
};

int main() {
    auto expr = new AddExpr(new IdExpr("x"), new NumExpr(42));
    Dumper d;
    d.dispatch(expr);
    std::cout << std::endl;
    Dumper2 d2;
    d2.dispatch(expr, std::cerr);
    std::cerr << std::endl;
    delete expr;
    return 0;
}
