#include <cassert>
#include <iostream>
#include <string>
#include <typeinfo>

using namespace std;

#define EXPRS(m) \
    m(AddExpr) \
    m(IdExpr) \
    m(NumExpr)

class Expr {
public:
    virtual ~Expr() {}

private:
    mutable int val;

    friend class Sum;
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
    R dispatch(const Expr* e, Args&&... args);
};

template<class Derived, class R, class... Args>
R Visitor<Derived, R, Args...>::dispatch(const Expr* e, Args&&... args) {
    if (false) {}
#define DISPATCH(E) \
    else if (typeid(*e) == typeid(E)) return derived().visit(static_cast<const AddExpr*>(e), std::forward<Args>(args)...);
    EXPRS(DISPATCH)
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

class Sum : public Visitor<Sum, int> {
public:
    int sum(const Expr* e) { return e->val = dispatch(e); }
    int visit(const AddExpr* e) {
        return sum(e->left) + sum(e->right);
    }

    int visit(const IdExpr*) { return 23; }
    int visit(const NumExpr* e) { return e->num; }
};

int main() {
    auto expr = new AddExpr(new IdExpr("x"), new NumExpr(42));
    Dumper d;
    d.dispatch(expr);
    std::cout << std::endl;
    Dumper2 d2;
    d2.dispatch(expr, std::cerr);
    std::cerr << std::endl;
    Sum sum;
    std::cout << sum.sum(expr) << std::endl;
    delete expr;
    return 0;
}
